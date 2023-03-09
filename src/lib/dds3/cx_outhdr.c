static char rcsid[]="$Id: cx_outhdr.c 230 2011-07-05 23:53:12Z user $";
/*
  RCS-t=DDS API, convenience routine to open output data with datatype 
                 and specified headers
*/

/***************************************/
/***   Copyright (C) 2006            ***/
/***   BP America, Houston, TX       ***/
/***   Jerry Ehlers, August, 2006    ***/
/***************************************/

#include "dds_sys.h"

/* This routine is to pass any union/struct along */
static int get_block(int bin, int tag)
{
   int count, p_tag;
   const char *name, *prime;

   tag = cdds_member(bin, tag, "");
   while(tag >= 0) {
      name  = cdds_name(bin, tag);
      p_tag = cdds_prime(bin, tag);
      prime = cdds_name(bin, p_tag);
      count = cdds_count(bin, tag);

      if (!strcmp(name, "Samples")) {
      } else if (!strcmp(prime, "union") || !strcmp(prime, "struct")) {
         cdds_printf("", " %s {\n", prime);
         get_block(bin, tag);
         cdds_printf("", " } %s;\n", name);
      } else if (count <= 1)  {
         cdds_printf("", " %s %s;\n", prime, name);
      } else  {
         cdds_printf("", " %s %s[%d];\n", prime, name, count);
      }

      tag = cdds_next(bin, tag);
   }
   return tag;
}


BIN_TAG cddx_outhdr(
   const char *alias,
   const char *value,
   const char *title,
   BIN_TAG old_tag,
   int smplPrime,
   int hdrPrime,
   const char *localHdrs)
{
   int i, n, ier, nhdrs, *tags, count, *counts;
   char *lclhdrs, *hdr, **hdrs, **primes;
   const char *old_dict, *buf_dict, *name, *prime;
   DDS_BIN *buf_bin;
   BIN_TAG bin_tag, buf_tag, p_tag, tag;
   DDS_STRING buf_alias = DDS_NULL_STRING;
   DDS_STRING tmp = DDS_NULL_STRING;

   DDS_API_ENTRY("xouthdr");

   /***********************************************************/
   /* check if <alias>, <alias>_data or <value> are specified */
   /***********************************************************/

   if (value) while(isspace(*value)) value++;
   cdds_dict("par:", "scan");
   ddsStrCpyCat4(&tmp, alias, " ", alias, "_data");
   if (cdds_scanf(tmp.ptr,"")<0 && (!value || ! *value)) {
      DDS_API_EXIT((BIN_TAG)-2);
   }

   VALID_PRIME(smplPrime, DDS_API_EXIT(DDS_BAD));
   VALID_PRIME(hdrPrime, DDS_API_EXIT(DDS_BAD));
   if (localHdrs) while(isspace(*localHdrs)) localHdrs++;

   /********************/
   /* reserve bin tags */
   /********************/

   bin_tag = ddsReserveBin(alias, value, title, "w+");
   if (bin_tag < 0) {
      cdds_prterr("Unable to reserve output (%s) bin\n", alias);
      DDS_API_EXIT(bin_tag);
   }

   ddsStrCpyCat2(&buf_alias, alias, "_buf");
   buf_tag = ddsReserveBin(buf_alias.ptr, "", title, "m");
   if (buf_tag < 0) {
      cdds_prterr("Unable to reserve internal (%s) bin\n", 
                  buf_alias.ptr);
      ddsStrFree(&buf_alias);
      DDS_API_EXIT(buf_tag);
   }

   /**************************************************************/
   /* Get list of input headers (can only handle simple scalars) */
   /* This only needs to be done if we have specified headers    */
   /**************************************************************/

   nhdrs = 0;
   if (old_tag>=0 && localHdrs && *localHdrs) {
      tag = cdds_member(old_tag, 0, "");
      while(tag >= 0) {
         nhdrs++;
         tag = cdds_next(old_tag, tag);
      }
      if (nhdrs) {
         hdrs = (char**)cdds_malloc(nhdrs*sizeof(char*));
         primes = (char**)cdds_malloc(nhdrs*sizeof(char*));
         tags = (int*)cdds_malloc(nhdrs*sizeof(int));
         counts = (int*)cdds_malloc(nhdrs*sizeof(int));
      }
 
      nhdrs = 0;
      tag = cdds_member(old_tag, 0, "");
      while(tag >= 0) {
         name = cdds_name(old_tag, tag);
         p_tag = cdds_prime(old_tag, tag);
         prime = cdds_name(old_tag, p_tag);
         count = cdds_count(old_tag, tag);
         hdrs[nhdrs] = (char*)cdds_malloc(1+strlen(name));
         strcpy(hdrs[nhdrs], name);
         primes[nhdrs] = (char*)cdds_malloc(1+strlen(prime));
         strcpy(primes[nhdrs], prime);
         tags[nhdrs] = tag;
         counts[nhdrs] = count;
         nhdrs++;
         tag = cdds_next(old_tag, tag);
      }
   }

   /**********************************/
   /* setup buffer output dictionary */
   /**********************************/

   if (old_tag >= 0 && old_tag < dds_bin_max) {
      old_dict = dds_bin[old_tag]->dict_open;
      buf_dict = cdds_out(buf_alias.ptr, "", old_dict);
   } else {
      old_dict = cdds_in("", "", title);
      buf_dict = cdds_out(buf_alias.ptr, "", old_dict);
      cdds_printf("fmt:ASP_TRACE", "typedef struct {\n");
      cdds_printf("", "   SAMPLE_TYPE Samples[axis_size(1)];\n");
      cdds_printf("", "} ASP_TRACE;\n");
   } 

   /* Make sure default out_format is asp if headers exist */
   cdds_dict(buf_dict,"scan");
   ier = cdds_scanf("old_format","");
   cdds_dict(buf_dict,"print");
   if (ier < 0) cdds_printf("old_format", "asp\n");

   ddsStrCpyCat2(&tmp, buf_alias.ptr, "_format");
   cdds_printf(tmp.ptr, "asp\n");

   if (localHdrs && *localHdrs) {
      lclhdrs = (char*)cdds_malloc(1+strlen(localHdrs));
      strcpy(lclhdrs, localHdrs);
      localHdrs = lclhdrs;
      cdds_printf("fmt:ASP_TRACE", "typedef struct {\n");

      /* 1st: any newly defined output headers */
      while(*lclhdrs) {
         hdr = lclhdrs;
         while(*lclhdrs && !isspace(*lclhdrs)) lclhdrs++;
         if (*lclhdrs) {
            *lclhdrs = '\0';
            lclhdrs++;
         }

         n = -1;
         if (strlen(hdr)==6) sscanf(hdr,"Horz%2d",&n);
         if (n>=1 && n<=20 && !strncmp(hdr,"Horz",4)) {
#ifdef HOST_SWAP
           cdds_printf("", " integer2x TVPV%02d, TVPT%02d;\n", n ,n);
#else	
           cdds_printf("", " integer2 TVPT%02d, TVPV%02d;\n", n ,n);
#endif
           sprintf(hdr,"TVPT%02d",n);
           for(i=0;i<nhdrs;i++) {
             if (!strcmp(hdrs[i],hdr)) {
               hdrs[i][0] = '\0';
               break;
             }
           }
           sprintf(hdr,"TVPV%02d",n);
         } else {
           cdds_printf("", " %s %s;\n", dds_prime[hdrPrime].name, hdr);
         }
         for(i=0;i<nhdrs;i++) {
           if (!strcmp(hdrs[i],hdr)) {
             hdrs[i][0] = '\0';
             break;
           }
         }

         while(isspace(*lclhdrs)) lclhdrs++;
      }
      cdds_free((char*)localHdrs);

      /* 2nd: any previous incoming headers        */
      /* (including Samples in it's correct place) */
      for(i=0;i<nhdrs;i++) {
         if (hdrs[i][0]) {
            if (!strcmp(primes[i],"union") || !strcmp(primes[i],"struct")) {
               cdds_printf(""," %s {\n", primes[i]);
               get_block(old_tag, tags[i]);
               cdds_printf(""," } %s;\n", hdrs[i]);
            } else if (!strcmp(hdrs[i],"Samples")) {
               cdds_printf(""," SAMPLE_TYPE Samples[axis_size(1)];\n");
            } else if (counts[i] <= 1)  {
               cdds_printf(""," %s %s;\n",primes[i],hdrs[i]);
            } else  {
               cdds_printf(""," %s %s[%d];\n",primes[i],hdrs[i],counts[i]);
            }
         }
      }

      if (nhdrs) {
         for (i=0;i<nhdrs;i++) {
            cdds_free(hdrs[i]);
            cdds_free(primes[i]);
         }
         cdds_free(hdrs);
         cdds_free(primes);
         cdds_free(tags);
         cdds_free(counts);
      } else {
         /* 3rd: add Samples if no previous buffer */
         cdds_printf(""," SAMPLE_TYPE Samples[axis_size(1)];\n");
      }

      cdds_printf("", "} ASP_TRACE;\n");
   }

   cdds_printf("fmt:SAMPLE_TYPE", "typedef %s SAMPLE_TYPE;\n",
               dds_prime[smplPrime].name);

   /* clear local info */

   ddsStrFree(&tmp);
   ddsStrFree(&buf_alias);
   if (old_tag < 0 || old_tag >= dds_bin_max) {
      cdds_dict(old_dict, "close");
      cdds_free((void*)old_dict);
   }

   /* save info for delayed opening */

   buf_bin = dds_bin[buf_tag];
   buf_bin->dict_open = buf_dict;
   buf_bin->tag = old_tag;
   buf_bin->bin_tag = bin_tag;

   DDS_API_EXIT(buf_tag);
}
