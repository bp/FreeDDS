static char rcsid[]="$Id: cx_inhdr.c 60 2009-02-23 14:17:55Z user $";
/*
RCS-t=DDS API, convenience routine to open input data with specified 
               headers & Samples
*/

/***************************************/
/***   Copyright (C) 2006            ***/
/***   BP America, Houston, TX       ***/
/***   Jerry Ehlers, August, 2006    ***/
/***************************************/

#include "dds_sys.h"


/* This routine is to pass any union/struct along */
static void get_block(int bin, int tag)
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
}


BIN_TAG cddx_inhdr(
   const char *alias,
   const char *value,
   const char *title,
   int smplPrime,
   int hdrPrime,
   const char *localHdrs)
{
   int i, nhdrs, errors, *tags, count, *counts;
   char *lclhdrs, *hdr, **hdrs, **primes;
   const char *dict, *buf_dict, *name, *genus, *prime;
   BIN_TAG bin_tag, buf_tag, p_tag, tag;
   DDS_STRING fmt = DDS_NULL_STRING;
   DDS_STRING format = DDS_NULL_STRING;
   DDS_STRING data = DDS_NULL_STRING;
   DDS_STRING buf_alias = DDS_NULL_STRING;

   DDS_API_ENTRY("xinhdr");

   /* check if <alias>, <alias>_data or <value> are specified */

   if (value) while(isspace(*value)) value++;
   cdds_dict("par:", "scan");
   ddsStrCpyCat4(&data, alias, " ", alias, "_data");
   if (cdds_scanf(data.ptr,"")<0 && (!value || ! *value)) {
      DDS_API_EXIT((BIN_TAG)-2);
   }

   if (smplPrime >= 0) {
      VALID_PRIME(smplPrime, DDS_API_EXIT(DDS_BAD));
   }
   VALID_PRIME(hdrPrime, DDS_API_EXIT(DDS_BAD));
   if (localHdrs) while(isspace(*localHdrs)) localHdrs++;

   /* open input */

   ddsStrCpyCat2(&format, alias, "_format");
   ddsStrCpyCat2(&data, alias, "_data");
   dict = cdds_in(alias, value, title);
   bin_tag = cdds_open(dict, format.ptr, data.ptr, "r+");
   ddsStrFree(&format);
   ddsStrFree(&data);
   if (bin_tag < 0) {
      cdds_prterr("Unable to open input (%s) file\n", alias);
      cdds_dict(dict, "close");
      DDS_API_EXIT(bin_tag);
   }

   /* get list of input headers (can only handle simple scalars */

   nhdrs = 0;
   tag = cdds_member(bin_tag, 0, "");
   while(tag >= 0) {
      name = cdds_name(bin_tag, tag);
      if (strcmp(name, "Samples")) nhdrs++;
      tag = cdds_next(bin_tag, tag);
   }
   if (nhdrs) {
      hdrs = (char**)cdds_malloc(nhdrs*sizeof(char*));
      primes = (char**)cdds_malloc(nhdrs*sizeof(char*));
      tags = (int*)cdds_malloc(nhdrs*sizeof(int));
      counts = (int*)cdds_malloc(nhdrs*sizeof(int));
   }
 
   nhdrs = 0;
   tag = cdds_member(bin_tag, 0, "");
   while(tag >= 0) {
      name = cdds_name(bin_tag, tag);
      p_tag = cdds_prime(bin_tag, tag);
      prime = cdds_name(bin_tag, p_tag);
      count = cdds_count(bin_tag, tag);
      if (strcmp(name, "Samples")) {
         hdrs[nhdrs] = (char*)cdds_malloc(1+strlen(name));
         strcpy(hdrs[nhdrs], name);
         primes[nhdrs] = (char*)cdds_malloc(1+strlen(prime));
         strcpy(primes[nhdrs], prime);
         tags[nhdrs] = tag;
         counts[nhdrs] = count;
         nhdrs++;
      }
      tag = cdds_next(bin_tag, tag);
   }
  
   /* define buffer */

   ddsStrCpyCat2(&buf_alias, alias, "_buf");
   ddsStrCpyCat2(&format, buf_alias.ptr, "_format");
   ddsStrCpyCat2(&data, buf_alias.ptr, "_data");

   errors = 0;
   cdds_dict("override:", "print");
   ddsStrCpyCat3(&fmt, "fmt:*:", buf_alias.ptr, ".ASP_TRACE");
   cdds_printf(fmt.ptr, "typedef struct {\n");

   /* 1st: specified headers */
   if (localHdrs && *localHdrs) {
      lclhdrs = (char*)cdds_malloc(1+strlen(localHdrs));
      strcpy(lclhdrs, localHdrs);
      localHdrs = lclhdrs;
      while(*lclhdrs) {
         hdr = lclhdrs;
         while(*lclhdrs && !isspace(*lclhdrs)) lclhdrs++;
         if (*lclhdrs) {
            *lclhdrs = '\0';
            lclhdrs++;
         }

         for(i=0;i<nhdrs;i++) {
            if (!strcmp(hdrs[i],hdr)) {
               cdds_printf("", " %s %s;\n", dds_prime[hdrPrime].name, hdr);
               hdrs[i][0] = '\0';
               break;
            }
         }
         if (i >= nhdrs) {
            errors++;
            cdds_prterr("Selected input header, '%s', does not exist\n", hdr);
         }

         while(isspace(*lclhdrs)) lclhdrs++;
      }
      cdds_free((char*)localHdrs);
   }

   /* 2nd: Samples */
   cdds_printf("", " SAMPLE_TYPE Samples[axis_size(1)];\n");

   /* 3rd: any remaining headers */
   for(i=0;i<nhdrs;i++) {
      if (hdrs[i][0]) {
         if (!strcmp(primes[i], "union") || !strcmp(primes[i], "struct")) {
            cdds_printf("", " %s {\n", primes[i]);
            get_block(bin_tag, tags[i]);
            cdds_printf("", " } %s;\n", hdrs[i]);
         } else if (counts[i] <= 1)  {
            cdds_printf("", " %s %s;\n", primes[i], hdrs[i]);
         } else  {
            cdds_printf("", " %s %s[%d];\n", primes[i], hdrs[i], counts[i]);
         }
      }
   }

   cdds_printf("", "} ASP_TRACE;\n");

   for (i=0;i<nhdrs;i++) {
      cdds_free(hdrs[i]);
      cdds_free(primes[i]);
   }
   if (nhdrs) {
      cdds_free(hdrs);
      cdds_free(primes);
      cdds_free(tags);
      cdds_free(counts);
   }

   tag = cdds_member(bin_tag, 0, "Samples");
   if (tag >= 0) {
      genus = cdds_genus(bin_tag, tag);      
      if (smplPrime >= 0) {
         cdds_printf("fmt:SAMPLE_TYPE", "typedef %s SAMPLE_TYPE;\n",
                     dds_prime[smplPrime].name);
      } else if (!strcmp(genus,"complex")) {
         cdds_printf("fmt:SAMPLE_TYPE", "typedef complex SAMPLE_TYPE;\n");
      } else {
         cdds_printf("fmt:SAMPLE_TYPE", "typedef float SAMPLE_TYPE;\n");
      }
   }

   /* open buffer */

   buf_dict = cdds_out(buf_alias.ptr, "", dict);
   cdds_printf("$old_format", "format\n");
   cdds_printf(format.ptr, "asp\n");
   buf_tag = cdds_open(buf_dict, format.ptr, data.ptr, "m");
   if (buf_tag < 0) {
      cdds_prterr("Unable to open internal (%s) buffer\n", alias);
      cdds_dict("override:", "reset");
      cdds_dict(buf_dict, "close");
      cdds_dict(dict, "close");
      cdds_close(bin_tag);
      DDS_API_EXIT(buf_tag);
   }

   /* map input to buffer */

   cdds_openm(buf_tag,0,bin_tag,0);
   cdds_openm(bin_tag,0,buf_tag,0);
   cdds_dict("override:", "reset");

   /* save info */

   dds_bin[buf_tag]->bin_tag = bin_tag;

   if (errors) {
      cdds_close(buf_tag);
      buf_tag = -3;
   }

   DDS_API_EXIT(buf_tag);
}
