static char rcsid[]="$Id: readgen.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, read generic binary data

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

int ddsReadGen(
   DDS_BIN *bin,
   DDS_TYPE *type,
   char *buffer,
   int count)
{
   DDS_MEDIA *media = bin->media;
   DDS_BOOLEAN dump;
   int n_read, n_bytes, n_want, n_chunk, residual;
   char *buf;

   if (bin->state & DDS_BIN_ZIP) {
      ddsError("Unable to read 'zip' files on a byte-basis, MUST read on a trace-basis\n");
      return 0; 
   }

   errno = 0;

   buf = buffer;
   residual = 0;

   if((dds_state & DDS_DUMP_DATA) && (dds_state & DDS_DUMP_READ)) {
      dump = DDS_TRUE;
   } else {
      dump = DDS_FALSE;
   }

   for(n_read = 0; n_read < count; n_read += n_chunk) {
      /* read data vector using gio */
      n_want = (count - n_read) * type->stroke;
      n_bytes = greadc(media->fp, buf, n_want);

      n_chunk = n_bytes / type->stroke;

      if(n_bytes != n_want) {
         n_want = n_chunk * type->stroke;
         residual = n_bytes - n_want;
         if(residual) {
            if(geof(media->fp)) errno = 0;
            /* un-read some input data */
            gunread(media->fp, buf + n_want, residual);
            break;
         }
         n_bytes = n_want;
      }

      if(dump) ddsDumpDataBuf(bin, "read", (DDS_SYM*)type, buf);

      buf += n_bytes;

      if(! n_chunk) break; /* give up */
   }

   return ddsReadCheck(bin, type, buffer, count, n_read, residual);
}
