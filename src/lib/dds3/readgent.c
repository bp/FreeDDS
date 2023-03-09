static char rcsid[]="$Id: readgent.c 309 2018-12-18 22:15:38Z user $";
/*
RCS-t=DDS API, read and edit generic trace binary data

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "cdds.h"
#include "dds_sys.h"
#include "dds_dump.h"

int ddsReadGenT(
   DDS_BIN *bin,
   DDS_TYPE *type,
   char *buffer,
   int count)
{
   DDS_MEDIA *media = bin->media;
   DDS_BOOLEAN dump;
   int ier,n, n_read, n_bytes, residual;
   char *buf;
   const char *rbuf, *ebuf;

   errno = 0;

   buf = buffer;
   n_bytes = 0;   /* number of bytes in rbuf */
   residual = 0;

   if((dds_state & DDS_DUMP_DATA) && (dds_state & DDS_DUMP_READ)) {
      dump = DDS_TRUE;
   } else {
      dump = DDS_FALSE;
   }

   for(n_read = 0; n_read < count; n_read++) {

      reject_trace:
      if(! n_bytes) {
         /* attempt to read another trace */
         if (bin->state & DDS_BIN_ZIP) {
            ddsError("Error: zip compression currently unsupported\n");
            return 0;
         } else {
            n_bytes = greada(media->fp, (const void**)&rbuf,
                             type->stroke, GIO_CHAR);
         }

         if(n_bytes != type->stroke) {
            /* no input trace */
            if(geof(media->fp)) errno = 0;
            residual = n_bytes;
            break;
         }
      }

      /* Edit the input trace in rbuf. */
      /* This "(void*)" violates "const". */
      ebuf = ddsEditTrace(bin, type, (void*)rbuf, DDS_BIN_MSG_READ);

      if(ebuf == rbuf) {
         /* trace is accepted by the API */
         memcpy(buf, ebuf, n_bytes);
         n_bytes = 0;

         if(dump) ddsDumpDataBuf(bin, "read", (DDS_SYM*)type, rbuf);
      } else if(ebuf == bin->dead_trace) {
         /* pass a dead trace to the API */
         memcpy(buf, ebuf, n_bytes);

         /* Counters reflect the media (not API). */
         bin->LineTrc--;
         bin->ReelTrc--;
         bin->FileTrc--;
      } else if(! ebuf) {
         /* Discard trace and read another. */
         if(dump) ddsDumpDataBuf(bin, "read", (DDS_SYM*)type, rbuf);

         /* Counters reflect the media (not API). */
         bin->LineTrc++;
         bin->ReelTrc++;
         bin->FileTrc++;
         n_bytes = 0;
         goto reject_trace;
      } else {
         /* edit error, terminate API */
         count = n_read;
         break;
      }

      buf += type->stroke;
      ddsCubeInc(bin);
   }

   if(n_bytes) {
      /* un-read some input data */
      gunread(media->fp, rbuf, n_bytes);
   }

   return ddsReadCheck(bin, type, buffer, count, n_read, residual);
}
