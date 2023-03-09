static char rcsid[]="$Id: opennewgen.c 223 2011-04-06 21:25:42Z user $";
/*
RCS-t=DDS, open new binary data, generic format

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int ddsSetSEP(DDS_BIN *bin);


void ddsOpenNewGen(
   DDS_BIN *bin,
   DDS_CUBE *old_cube)
{
   DDS_TYPE *type;

   cdds_dict(bin->dict_open, "scan");

   /*************************************************/
   /* check if this is a zip-compressed binary.     */
   /*************************************************/

   if(0 < cdds_scank("format", "zip")) {
      int level = 6;
      cdds_scanf("", "%*s %d", &level);
      bin->state  |= DDS_BIN_ZIP;
      bin->std.gen.level   = level;
      bin->std.gen.size    = 1000;
      bin->std.gen.nmaxtrc = 1;
      bin->std.gen.ntraces = 0;
      bin->std.gen.offset  = 0;
      bin->std.gen.table   = (HOST_ULLONG_T*)ddsMalloc(1000*sizeof(HOST_ULLONG_T));
      bin->std.gen.table[0]= 0;
   }      

   /*************************************************/
   /* compile the description of the binary format. */
   /*************************************************/

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
      ddsSymTypedefObj("SAMPLE_TYPE", DDS_STYPE_FLOAT4, 0);
      ddsSetSEP(bin);
      cdds_dict(bin->dict_open, "scan");
   }

   if(ddsCompileFmt(bin, DDS_TRUE)) return;

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
   bin->trace_buf = ddsZalloc(type->stroke);

   if(dds_state & DDS_DUMP_DIFF) {
      bin->diff1_buf = ddsMalloc(type->stroke);
      memset(bin->diff1_buf, DDS_NON_CHAR, type->stroke);
      bin->diff2_buf = ddsMalloc(type->stroke);
      memset(bin->diff2_buf, DDS_NON_CHAR, type->stroke);
   }

   bin->dead_trace = ddsZalloc(type->stroke);

   return;
}

/********************************/
/* set the SEP format informat. */
/********************************/

int ddsSetSEP(
   DDS_BIN *bin)
{
   int i, ndim, n[RANK_MAX];
   float d[RANK_MAX], o[RANK_MAX];
   char  text[16];
   const char *a[RANK_MAX], *u[RANK_MAX], *defn;

   cdds_dict(bin->dict_open, "scan");

   cdds_scanf("axis", "");
   for(ndim=0;ndim<RANK_MAX;ndim++) {
      if (cdds_scant("", &defn) <= 0) break;
      a[ndim] = ddsString(defn);
   }

   for(i=0;i<ndim;i++) {
      sprintf(text, "units.axis(%d)", i+1);
      if (cdds_scant(text, &defn) > 0) {
         u[i] = ddsString(defn);
      } else {
         u[i] = ddsString("?");
      }

      n[i] = 1;
      sprintf(text, "size.axis(%d)", i+1);
      cdds_scanf(text, "%d", &n[i]);

      d[i] = 1.0;
      sprintf(text, "delta.axis(%d)", i+1);
      cdds_scanf(text, "%f", &d[i]);

      o[i] = 0.0;
      sprintf(text, "origin.axis(%d)", i+1);
      cdds_scanf(text, "%f", &o[i]);
   }

   cdds_dict(bin->dict_open, "print");

   for(i=0;i<ndim;i++) {
      sprintf(text, "label%d", i+1);
      cdds_printf(text, "%s\n", a[i]);
      ddsFree((void*)a[i]);

      sprintf(text, "unit%d", i+1);
      cdds_printf(text, "%s\n", u[i]);
      ddsFree((void*)u[i]);

      sprintf(text, "n%d", i+1);
      cdds_printf(text, "%d\n", n[i]);

      sprintf(text, "d%d", i+1);
      cdds_printf(text, "%f\n", d[i]);

      sprintf(text, "o%d", i+1);
      cdds_printf(text, "%f\n", o[i]);
   }

   cdds_printf("esize","4\n");
   cdds_printf("data_format","xdr_float\n");

   return(0);
}
