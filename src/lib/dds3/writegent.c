static char rcsid[]="$Id: writegent.c 309 2018-12-18 22:15:38Z user $";
/*
RCS-t=DDS API, edit and write generic trace binary data

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "cdds.h"
#include "dds_sys.h"
#include "dds_dump.h"


int ddsWriteGenT(
   DDS_BIN *bin,
   DDS_TYPE *type,
   const char *buffer,
   int count)
{
   DDS_MEDIA *media = bin->media;
   DDS_BOOLEAN dump;
   int ier,n, n_write, n_bytes, residual;
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
         if (bin->state & DDS_BIN_ZIP) {
           ddsError("Error: zip compression currently unsupported\n");
           return 0;
         } else {
            n_bytes = gwritec(media->fp, ebuf, type->stroke);

            if(n_bytes != type->stroke) {
               /* error writing trace */
               residual = n_bytes;
               break;
            }
         }

         if(dump) ddsDumpDataBuf(bin, "write", (DDS_SYM*)type, ebuf);

      } else if(ebuf == bin->dead_trace) {
         /* write a dead trace before the API trace */
         if (bin->state & DDS_BIN_ZIP) {
            ddsError("Error: zip compression currently unsupported\n");
            return 0;
         } else {
            n_bytes = gwritec(media->fp, ebuf, type->stroke);

            if(n_bytes != type->stroke) {
               /* error writing dead trace */
               residual = n_bytes;
               break;
            }
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
