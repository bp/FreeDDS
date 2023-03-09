static char rcsid[]="$Id: writeuspt.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, edit and write usp trace binary data

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "usp_hdr.h"
#include "dds_dump.h"

int ddsWriteUspT(
   DDS_BIN *bin,
   DDS_TYPE *type,
   const char *buffer,
   int count)
{
   DDS_MEDIA *media = bin->media;
   DDS_BOOLEAN dump;
   int n_write, n_bytes, residual;
   const char *buf;
   char *ebuf;

   errno = 0;

   buf = buffer;
   residual = 0;

   if((dds_state & DDS_DUMP_DATA) && (dds_state & DDS_DUMP_WRITE)) {
      dump = DDS_TRUE;
   } else {
      dump = DDS_FALSE;
   }

   for(n_write = 0; n_write < count; n_write++) {

      /* Edit the output trace in buf. */
      /* This "(void*)" violates "const". */
      pad_dead:
      ebuf = ddsEditTrace(bin, type, (void*)buf, DDS_BIN_MSG_WRITE);

      if(ebuf == buf) {
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
         buf += type->stroke;
         continue;
      } else {
         /* edit error, terminate API */
         count = n_write;
         break;
      }

      buf += n_bytes;
      ddsCubeInc(bin);
   }

   return ddsWriteCheck(bin, type, buffer, count, n_write, residual);
}
