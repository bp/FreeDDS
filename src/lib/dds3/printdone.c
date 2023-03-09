static char rcsid[]="$Id: printdone.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, de-select current print dict and defn

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int ddsPrintDone(
   void)
{
   int status = DDS_OK;
   size_t size, w_size;
   DDS_DEFN *eof_defn;

   if(dds_print_first) {
      eof_defn = (DDS_DEFN*)dds_print_first->defn.newest;
      dds_print_first->buf.ptr[eof_defn->start] = '\0';
      size = eof_defn->start - dds_print_first->buf_flush;
      if(size && dds_print_first->media &&
         dds_print_first->media->state & (DDS_MEDIA_OPENW | DDS_MEDIA_OPENA)) {
         /* append dictionary buffer to GFILE stream and flush */
         w_size = gwritec(dds_print_first->media->fp,
            dds_print_first->buf.ptr + dds_print_first->buf_flush, size);
         if(w_size != size) {
            dds_error = ddsError("dict: errno= %i, %s\n\twriting dictionary %s (%s)\n",
               errno, strerror(errno), dds_print_first->name,
               dds_print_first->media->title.ptr);
            status = DDS_BAD;
         } else if(gflush(dds_print_first->media->fp)) {
            dds_error = ddsError("dict: errno= %i, %s\n\tflushing dictionary %s (%s)\n",
               errno, strerror(errno), dds_print_first->name,
               dds_print_first->media->title.ptr);
            status = DDS_BAD;
         }
         if(w_size > 0) dds_print_first->buf_flush += w_size;
      }
   }

   return status;
}
