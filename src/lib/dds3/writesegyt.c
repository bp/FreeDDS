static char rcsid[]="$Id: writesegyt.c 120 2009-10-06 18:00:21Z user $";
/*
RCS-t=DDS API, edit and write segy trace binary data

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

int ddsWriteSegyT(
   DDS_BIN *bin,
   DDS_TYPE *type,
   const char *buffer,
   int count)
{
   DDS_MEDIA *media = bin->media;
   DDS_BOOLEAN dump;
   int n_write, n_bytes, residual;
   int green, n_prefix, n_suffix;
   int NumSmp, SmpInt;
   double cube_to_usec;
   const char *cube_units, *wbuf;
   char *ebuf;

   errno = 0;

   wbuf = buffer;
   residual = 0;

   if((dds_state & DDS_DUMP_DATA) && (dds_state & DDS_DUMP_WRITE)) {
      dump = DDS_TRUE;
   } else {
      dump = DDS_FALSE;
   }

   if(bin->state & DDS_green_prefix) {
      n_prefix = bin->prefix.type->prec;
      if(bin->state & DDS_green_suffix) {
         n_suffix = bin->suffix.type->prec;
         green = type->stroke - n_prefix - n_suffix;
      } else {
         n_suffix = 0;
         green = type->stroke - n_prefix;
      }
   } else {
      n_prefix = n_suffix = 0;
   }

   for(n_write = 0; n_write < count; n_write++) {

      if(n_prefix) {
         /* initialize green word prefix */
         CON_NASS(bin->prefix.type->ptype, DDS_CTYPE_INT,
            wbuf + bin->prefix.offset, &green);
         if(n_suffix) {
            /* initialize green word suffix */
            CON_NASS(bin->suffix.type->ptype, DDS_CTYPE_INT,
               wbuf + bin->suffix.offset, &green);
         }
      }

      /* set NumSmp & SmpInt trace headers */

      NumSmp = bin->cube.axis[1].size;
      CON_NASS(bin->std.segy.TrcNumSmp.type->ptype, DDS_CTYPE_INT,
               wbuf + bin->std.segy.TrcNumSmp.offset, &NumSmp);

      SmpInt = bin->std.segy.sample_prec; /* Used overloaded sample_prec */
      CON_NASS(bin->std.segy.TrcSmpInt.type->ptype, DDS_CTYPE_INT,
               wbuf + bin->std.segy.TrcSmpInt.offset, &SmpInt);

      /* Edit the output trace in wbuf. */
      /* This "(void*)" violates "const". */
      pad_dead:
      ebuf = ddsEditTrace(bin, type, (void*)wbuf, DDS_BIN_MSG_WRITE);

      if(ebuf == wbuf) {
         /* trace is accepted by the API */
         if(bin->state & DDS_BIN_PADDING) {
            /* Just kidding; close is padding dead traces. */
            count = n_write;
            break;
         }

         /* write trace to the output media */
         n_bytes = gwritec(media->fp, ebuf, type->stroke);

         if(n_bytes != type->stroke) {
            /* error writing trace */
            residual = n_bytes;
            break;
         }

         if(dump) ddsDumpDataBuf(bin, "write", (DDS_SYM*)type, ebuf);

      } else if(ebuf == bin->dead_trace) {
         /* write a dead trace before the API trace */
         n_bytes = gwritec(media->fp, ebuf, type->stroke);

         if(n_bytes != type->stroke) {
            /* error writing dead trace */
            residual = n_bytes;
            break;
         }

         if(dump) ddsDumpDataBuf(bin, "write", (DDS_SYM*)type, ebuf);

         ddsCubeInc(bin);
         goto pad_dead;
      } else if(! ebuf) {
         /* discard API trace and edit the next one */
         wbuf += type->stroke;
         continue;
      } else {
         /* edit error, terminate API */
         count = n_write;
         break;
      }

      wbuf += n_bytes;
      ddsCubeInc(bin);
   }

   return ddsWriteCheck(bin, type, buffer, count, n_write, residual);
}
