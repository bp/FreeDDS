static char rcsid[]="$Id: writegen.c 81 2009-05-19 13:59:35Z user $";
/*
RCS-t=DDS API, write generic binary data

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

int ddsWriteGen(
   DDS_BIN *bin,
   DDS_TYPE *type,
   const char *buffer,
   int count)
{
   DDS_MEDIA *media = bin->media;
   DDS_BOOLEAN dump;
   int n_write, n_bytes, n_want, residual;

   if (bin->state & DDS_BIN_ZIP) {
      ddsError("Unable to write to 'zip' files on a byte-basis, MUST write on a trace-basis\n");
      return 0;
   }

   errno = 0;

   if((dds_state & DDS_DUMP_DATA) && (dds_state & DDS_DUMP_WRITE)) {
      dump = DDS_TRUE;
   } else {
      dump = DDS_FALSE;
   }

   /* write type to the output media */
   n_want = count * type->stroke;
   n_bytes = gwritec(media->fp, buffer, n_want);

   if(n_bytes != n_want) {
      /* error writing type */
      residual = n_want - n_bytes;
      n_write = n_bytes / type->stroke;
   } else {
      residual = 0;
      n_write = count;
   }

   if(dump) ddsDumpDataBuf(bin, "write", (DDS_SYM*)type, buffer);

   return ddsWriteCheck(bin, type, buffer, count, n_write, residual);
}
