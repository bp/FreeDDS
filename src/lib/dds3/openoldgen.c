static char rcsid[]="$Id: openoldgen.c 309 2018-12-18 22:15:38Z user $";
/*
RCS-t=DDS, open old binary data, generic format

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int ddsGetSEP(DDS_BIN* bin);


typedef struct _TYPES {
   char *name;
   DDS_OBJECT *object;
} TYPES;

static int ddsCompileRev2(DDS_BIN *bin);

void ddsOpenOldGen(
   DDS_BIN *bin,
   DDS_CUBE *old_cube)
{
   int sep_data_type;
   DDS_TYPE *type;
   DDS_MEDIA *media;
   char *ctype, *buf;
   const char *defn;
   int rev2;

   /******************************************/
   /* if "sep" format, modify the dictionary */
   /* and get the sep data type              */
   /******************************************/
   
   if (!strcmp(bin->fmt_name, "sep")) {
      sep_data_type = ddsGetSEP(bin);
   }

   /*************************************/
   /* retrieve default cube description */
   /*************************************/

   ddsCubeScanBin(bin, old_cube);

   if(1 != bin->slice_max) {
      ddsError("open: one old file and cube slice count= %i\n\tdict= %s\n",
         (int)bin->slice_max, bin->dict_open);
      return;
   }

   cdds_dict(bin->dict_open, "scan");

   /*************************************************/
   /* compile the description of the binary format. */
   /*************************************************/

   rev2 = DDS_FALSE;
   
   if (!strcmp(bin->fmt_name, "fcube")) {
      ddsSymTypedefObj("SAMPLE_TYPE", DDS_STYPE_FLOAT4, 0);
   } else if (!strcmp(bin->fmt_name, "fcubex")) {
      ddsSymTypedefObj("SAMPLE_TYPE", DDS_STYPE_FLOAT4X, 0);
   } else if (!strcmp(bin->fmt_name, "dcube")) {
      ddsSymTypedefObj("SAMPLE_TYPE", DDS_STYPE_FLOAT8, 0);
   } else if (!strcmp(bin->fmt_name, "dcubex")) {
      ddsSymTypedefObj("SAMPLE_TYPE", DDS_STYPE_FLOAT8X, 0);
   } else if (!strcmp(bin->fmt_name, "ccube")) {
      ddsSymTypedefObj("SAMPLE_TYPE", DDS_STYPE_COMPLEX8, 0);
   } else if (!strcmp(bin->fmt_name, "ccubex")) {
      ddsSymTypedefObj("SAMPLE_TYPE", DDS_STYPE_COMPLEX8X, 0);
   } else if (!strcmp(bin->fmt_name, "icube")) {
      ddsSymTypedefObj("SAMPLE_TYPE", DDS_STYPE_INTEGER4, 0);
   } else if (!strcmp(bin->fmt_name, "icubex")) {
      ddsSymTypedefObj("SAMPLE_TYPE", DDS_STYPE_INTEGER4X, 0);
   } else if (!strcmp(bin->fmt_name, "i2cube")) {
      ddsSymTypedefObj("SAMPLE_TYPE", DDS_STYPE_INTEGER2, 0);
   } else if (!strcmp(bin->fmt_name, "i2cubex")) {
      ddsSymTypedefObj("SAMPLE_TYPE", DDS_STYPE_INTEGER2X, 0);
   } else if (!strcmp(bin->fmt_name, "i1cube")) {
      ddsSymTypedefObj("SAMPLE_TYPE", DDS_STYPE_INTEGER1, 0);
   } else if (!strcmp(bin->fmt_name, "sep")) {
      if (sep_data_type == 1) {
         ddsSymTypedefObj("SAMPLE_TYPE", DDS_STYPE_INTEGER1, 0);
      } else if (sep_data_type == 8) {
         ddsSymTypedefObj("SAMPLE_TYPE", DDS_STYPE_COMPLEX8, 0);
      } else if (sep_data_type == -4) {
         ddsSymTypedefObj("SAMPLE_TYPE", DDS_STYPE_INTEGER4, 0);
      } else if (sep_data_type == 0) {
         ddsSymTypedefObj("SAMPLE_TYPE", DDS_CTYPE_FLOAT, 0);
      } else {
         ddsSymTypedefObj("SAMPLE_TYPE", DDS_STYPE_FLOAT4, 0);
      }
   } else if (!strcmp(bin->fmt_name, "cube")) {
      cdds_dict(bin->dict_open, "scan");
      /* if no SAMPLE_TYPE for cube, then check r2 format */
      if (cdds_scanf("fmt:SAMPLE_TYPE", "") < 0) {
         if (cdds_scant("type:Samples", &defn) > 0) {
            ctype = (char*)ddsMalloc(6+strlen(defn));
            sprintf(ctype, "type:%s", defn);
            cdds_scant(ctype, &defn);
            ddsFree(ctype);
            if (!strcmp(defn, "float4")) {
               ddsSymTypedefObj("SAMPLE_TYPE", DDS_STYPE_FLOAT4, 0);
            } else if (!strcmp(defn, "complex8")) {
               ddsSymTypedefObj("SAMPLE_TYPE", DDS_STYPE_COMPLEX8, 0);
            } else if (!strcmp(defn, "integer4")) {
               ddsSymTypedefObj("SAMPLE_TYPE", DDS_STYPE_INTEGER4, 0);
            } else if (!strcmp(defn, "integer2")) {
               ddsSymTypedefObj("SAMPLE_TYPE", DDS_STYPE_INTEGER2, 0);
            } else if (!strcmp(defn, "integer1")) {
               ddsSymTypedefObj("SAMPLE_TYPE", DDS_STYPE_INTEGER1, 0);
            }
         }
      }
   } else if (strcmp(bin->fmt_name, "asp")) {
      cdds_dict(bin->dict_fmt, "scan");
      /* Check if this is some generic dds r2 format */
      ctype = (char*)ddsMalloc(6+strlen(bin->fmt_name));
      sprintf(ctype, "fmt:%s", bin->fmt_name);
      if (cdds_scanf(ctype, "") < 0) {
         sprintf(ctype, "type:%s", bin->fmt_name);
         if (cdds_scant(ctype, &defn) > 0) {
            rev2 = DDS_TRUE;
            ddsFree(ctype);
            if (ddsCompileRev2(bin)) return;
         }
      }
      ddsFree(ctype);
   }

   if(!rev2 && ddsCompileFmt(bin, DDS_FALSE)) return;

   ddsUncompileFmt(bin);

   /********************************************/
   /* find trace object and create its API tag */
   /********************************************/

   if(ddsTraceTag(bin)) return;

   /*********************************************************/
   /* Update binary cube description.                       */
   /*                                                       */
   /* Cube print must follow HDR_MOD_* in order to maintain */
   /* maximum precision for space and time coordinates,     */
   /* i.e. sample rates and origins.                        */
   /* This is a problem for usp and segy.                   */
   /*********************************************************/

   ddsCubePrint(bin, old_cube);

   /*************************/
   /* link cube description */
   /*************************/

   ddsCubeLinker(bin);

   /*********************************************************/
   /* allocate buffer for one dead trace, and initialize it */
   /*********************************************************/

   type = bin->trace.type;
   bin->trace_buf = ddsMalloc(type->stroke);

   if(dds_state & DDS_DUMP_DIFF) {
      bin->diff1_buf = ddsMalloc(type->stroke);
      memset(bin->diff1_buf, DDS_NON_CHAR, type->stroke);
      bin->diff2_buf = ddsMalloc(type->stroke);
      memset(bin->diff2_buf, DDS_NON_CHAR, type->stroke);
   }

   bin->dead_trace = ddsZalloc(type->stroke);

   /* current media offset is the start of the SEQUENCE (traces). */
   ggetpos(bin->media->fp, &bin->media->data_bias);
   bin->media->trace_bias = bin->media->data_bias;

   /**********************************************/
   /* check if this is a zip-compressed binary   */
   /* if so, read the zip table from the end     */
   /**********************************************/

   cdds_dict(bin->dict_open, "scan");
   if(0 < cdds_scank("format", "zip")) {
      ddsError("Error: zip compression currently unsupported\n");
   }      

   return;
}



static char* scanvDefn(const char *name, DDS_BIN *bin)
{
   int i, n;
   const char *defn = 0;
   char *text, *s;
	
   text = (char*)ddsMalloc(6+strlen(name));
   sprintf(text, "type:%s", name);
   if (cdds_scanv(text, &defn) <= 0) {
      ddsFree(text);
      ddsError("Unable to find defn=%s\n\tdict=%s\n", text, 
         bin->dict_open);
      return 0;
   }
   ddsFree(text);

   if (s = strchr(defn, '{')) s++;
   else s = (char*)defn;
   while (s && isspace(*s)) s++;
   text = (char*)ddsRealloc(text, 1+strlen(s));
   strcpy(text, s);
   if (s = strchr(text, '}')) *s = '\0';
   n = strlen(text);
   for (i=n-1;i>=0;i--) if (!isspace(text[i])) {
      text[i+1] = '\0';
      break;
   }
	
   return text;
}


static char* scantDefn(char *name, int *n, DDS_BIN *bin)
{
   const char *defn = 0;
   char *text;
	
   text = (char*)ddsMalloc(6+strlen(name));
   sprintf(text, "type:%s", name);
   *n = 0;
   cdds_scanf(text, "%*s %d", n);
   if (cdds_scant(text, &defn) <= 0) {
      ddsFree(text);
      ddsError("Unable to find defn=%s\n\tdict=%s\n", text, 
         bin->dict_open);
      return 0;
   }
  
   text = (char*)ddsRealloc(text, 1+strlen(defn));
   strcpy(text, defn);
   return text;
}


static char *parseDefn(char **text)
{
   char *str, *txt;
   
   txt = *text;
   str = (char*)ddsMalloc(1+strlen(txt));
   if (sscanf(txt, "%s", str) == 1) {
      txt = strstr(txt, str);
      txt = txt + strlen(str);
      while(*txt && isspace(*txt)) {
         txt = txt + 1;
      }
      *text = txt;
      return str;
   }
   
   ddsFree(str);
   return 0;
}


static DDS_PTYPE getPtype(char *name)
{
   int i;
   
   for (i=1;i<DDS_STYPE_MAX;i++) {
      if (!strcmp(name, dds_prime[i].name)) return dds_prime[i].stype;
   }
   
   return (DDS_PTYPE)0;
}


static DDS_OBJECT * definePrime(char *type)
{
   DDS_PTYPE ptype;
   char *text, *prime=0; 
   const char *defn;  
   
   text = (char*)ddsMalloc(6+strlen(type));
   sprintf(text, "type:%s", type);
   while (cdds_scant(text, &defn) > 0) {
      if (prime) {
         prime = (char*)ddsRealloc(prime, 1+strlen(defn));
      } else {
         prime = (char*)ddsMalloc(1+strlen(defn));
      }
      strcpy(prime, defn);
      text = (char*)ddsRealloc(text, 6+strlen(prime));
      sprintf(text, "type:%s", prime);
   }
   ddsFree(text);
   if (!prime) return UNIQUE_OBJECT(dds_symbol, type, dds_block);

   
   ptype = getPtype(prime);
   ddsFree(prime);
   return ddsSymTypedefObj(type, ptype, 0);
}


static int ddsCompileRev2(DDS_BIN *bin)
{
   int		i, n, ntypes;
   char		*s, *str;
   char		*trace, *seq, *field, *ctype;
   const char	*name;
   DDS_BLOCK	*block;
   DDS_OBJECT	*object;
   DDS_TYPE	*type;
   TYPES	*types;
   
   if (cdds_dict(bin->dict_open, "scan")) {
      ddsError("can't open old dictionary, dict= %s\n", bin->dict_open);
      return DDS_BAD;
   }


   /* Get the trace */
   
   if (!(trace = scanvDefn(bin->fmt_name, bin))) return DDS_BAD;
   str = seq = trace;
   while(s = parseDefn(&str)) { seq = s; }
   
   /* Get the sequence */
   
   if (!(seq = scanvDefn(seq, bin))) {
      ddsFree(trace);
      return DDS_BAD;
   }
   ddsFree(trace);
      
   /* 1st pass: parse for dependencies */
   
   ntypes = 0;
   str = seq;
   while(field = parseDefn(&str)) { 
      if (!(ctype = scantDefn(field, &n, bin))) {
         for (i=0;i<ntypes;i++) ddsFree(types[i].name);
         if (ntypes) ddsFree(types);
         ddsFree(seq);
         return DDS_BAD;
      }
      
      for (i=0;i<ntypes;i++) if (!strcmp(types[i].name, ctype)) break;
      if (i >= ntypes) {
         ntypes++;
         if (ntypes == 1) {
            types = (TYPES*)ddsMalloc(sizeof(TYPES));
         } else {
            types = (TYPES*)ddsRealloc(types, ntypes*sizeof(TYPES));
         }
         types[i].name = (char*)ddsMalloc(1+strlen(ctype));
         strcpy(types[i].name, ctype);
         types[i].object = definePrime(ctype);
         if (!types[i].object) {
            ddsError("Unable to find prime object=%s\n\tdict=%s\n",  
               ctype, bin->dict_open);
            for (i=0;i<ntypes;i++) ddsFree(types[i].name);
            ddsFree(types);
            ddsFree(ctype);
            ddsFree(seq);
            return DDS_BAD;
         }
      }
      ddsFree(ctype);
   }
      
   /* 2nd pass: build ASP_TRACE */
   
   block = ddsSymTypedefSB();
   str = seq;
   while(field = parseDefn(&str)) { 
      ctype = scantDefn(field, &n, bin);
      for (i=0;i<ntypes;i++) if (!strcmp(types[i].name, ctype)) break;
      ddsSymTypedefSM(field, types[i].object, n);
      ddsFree(ctype);
   }
   ddsFree(seq);

   object = ddsSymTypedefSE(block, "ASP_TRACE");

   /* define SEQUENCE */
   type = ddsSymTypeCopy(object->type);
   type->align = 0;
   type->trail = object;

   name = ddsTablePushStr(dds_symbol, "SEQUENCE");
   object = PUSH_OBJECT(dds_symbol, name, DDS_SYM_TYPEDEF, dds_block,
      type);

   return DDS_OK;
}

/**************************************/
/* get the SEP format informat.       */
/* 1=byte; 4=float; 8=complex; -4=int */
/**************************************/

int ddsGetSEP(
   DDS_BIN* bin)
{
   int i, ndim, n[RANK_MAX];
   float d[RANK_MAX], o[RANK_MAX];
   char  text[16], *a[RANK_MAX];
   const char *defn, *u[RANK_MAX];

   cdds_dict(bin->dict_open, "scan");

   for(ndim=0;ndim<RANK_MAX;ndim++) {
      sprintf(text, "label%d", ndim+1);
      if (cdds_scant(text, &defn) <= 0) break;
      for(i=0;i<ndim;i++) if (!strcmp(defn,a[i])) break;
      if (i<ndim) {
         ddsWarn("Input axis %d has same label (%s) as axis %d\n\tRnaming it to %s%d\n", 
                 ndim+1, defn, i+1, defn, ndim+1);
         a[ndim] = cdds_malloc(3+strlen(defn));
         sprintf(a[ndim],"%s%d",defn,ndim+1);
      } else {         
         a[ndim] = (char*)ddsString(defn);
      }
   }

   for(i=0;i<ndim;i++) {
      sprintf(text, "unit%d", i+1);
      if (cdds_scant(text, &defn) > 0) {
         u[i] = ddsString(defn);
      } else {
         u[i] = ddsString("?");
      }

      n[i] = 1;
      sprintf(text, "n%d", i+1);
      cdds_scanf(text, "%d", &n[i]);

      d[i] = 1.0;
      sprintf(text, "d%d", i+1);
      cdds_scanf(text, "%f", &d[i]);

      o[i] = 0.0;
      sprintf(text, "o%d", i+1);
      cdds_scanf(text, "%f", &o[i]);
   }

   cdds_dict(bin->dict_open, "print");

   cdds_printf("axis", "");
   for(i=0;i<ndim;i++) {
     cdds_printf("", " %s", a[i]);
     ddsFree((void*)a[i]);
   }
   cdds_printf("", "\n");
   cdds_dict(bin->dict_open, "scan");
   cdds_dict(bin->dict_open, "print");

   for(i=0;i<ndim;i++) {
      sprintf(text, "units.axis(%d)", i+1);
      cdds_printf(text, "%s\n", u[i]);
      ddsFree((void*)u[i]);

      sprintf(text, "size.axis(%d)", i+1);
      cdds_printf(text, "%d\n", n[i]);

      sprintf(text, "delta.axis(%d)", i+1);
      cdds_printf(text, "%f\n", d[i]);

      sprintf(text, "origin.axis(%d)", i+1);
      cdds_printf(text, "%f\n", o[i]);
   }

   cdds_dict(bin->dict_open, "scan");
   i = 4;
   cdds_scanf("esize","%d", &i);
   if (i == 4 && cdds_scant("data_format", &defn) > 0) {
      if (strstr(defn, "xrd_int")) {
         i = -4;
      } else if (strstr(defn, "native_float")) {
         i = 0;
      }
   }

   return(i);
}
