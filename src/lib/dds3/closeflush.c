static char rcsid[]="$Id: closeflush.c 309 2018-12-18 22:15:38Z user $";
/*
RCS-t=DDS API, close, flush binary data

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int ddsCloseFlush(
   DDS_BIN *bin)
{
   DDS_MEDIA *media;
   DDS_BOOLEAN flush_trace, flush_pad;
   DDS_COORD coord;
   DDS_AXIS *axis;
   gpos_t offset_now, offset_end;
   int rank, rank_max;
   size_t reset, n_bytes;
   char *buf;

   media = bin->media;
   rank_max = bin->cube.rank;

   if(media->fp && (bin->state & DDS_BIN_WRITTEN)) {
      /* Last binary operation wrote a trace. */
      if(bin->state & DDS_TRACE_WRITE) {
         /* Flush needed to dispose of trace_buf. */
         flush_trace = DDS_TRUE;
      } else if(media->state & DDS_MEDIA_SWAP) {
         /* Last binary i/o appended a trace to the end. */
         /* Flush needed to pad dead traces onto hyper-cube. */
         flush_trace = DDS_TRUE;
      } else if(! ggetpos(media->fp, &offset_now) &&
                ! gseek(media->fp, 0, SEEK_END) &&
                ! ggetpos(media->fp, &offset_end) &&
                ! memcmp((char*)&offset_now, (char*)&offset_end,
                     sizeof(gpos_t))) {
         /* Last binary i/o appended a trace to the end. */
         /* Flush needed to pad dead traces onto hyper-cube. */
         flush_trace = DDS_TRUE;
      } else if(media->genus == DDS_MEDIA_SWAP) {
         /* This is a FIFO. */
         /* Flush needed to pad dead traces onto hyper-cube. */
         flush_trace = DDS_TRUE;
      } else {
         flush_trace = DDS_FALSE;
      }

      if(flush_trace) {
         /* Create a dummy trace to flush the output buffers. */
         reset = dds_tmp.top;
         buf = ddsPushTmp(bin->trace.type->stroke);
         flush_pad = DDS_FALSE;

         for(rank = bin->cube.io_rank; rank <= rank_max; rank++) {
            axis = bin->cube.axis + rank;

            if(axis->flag & DDS_AXIS_SORT) {
               /* This axis has a sort index, force its value. */
               if(rank < rank_max) {
                  coord = ddsGetExpected((long)0, axis);
                  if(axis->ordinal && ! (axis->flag & DDS_AXIS_SPARSE)) {
                     /* Write dummy trace to pad hypercube. */
                     flush_pad = DDS_TRUE;
                  }
               } else {
                  coord = ddsGetExpected(axis->ordinal+1, axis);
               }
               CON_NASS(axis->sort.type->ptype, DDS_CTYPE_DOUBLE,
                  buf + axis->sort.offset, &coord);
            }
         }

         if(flush_pad || (bin->state & DDS_TRACE_WRITE)) {
            /* Pretend to write ONE trace (with DDS_BIN_PADDING set). */
            /* This will cause dead traces to be inserted as needed */
            /* and will flush the trace buffer if needed. */ 
            bin->state |= DDS_BIN_PADDING;
            cdds_write(bin->tag, 0, buf, 1);
         }

         ddsStackSet(&dds_tmp, reset);
      }

      /* flush compression table if zip file */
      if (bin->state & DDS_BIN_ZIP) {
         ddsError("Error: zip compression currently unsupported\n");
      }
   }

   /* Flush read and write buffers. */
   if(media->fp) {
      /* Nominal case for gio access. */
      if(gflush(media->fp)) {
         ddsWarn("%s\n\tflushing %s, LineTrc= %lu\n",
            strerror(errno), media->title.ptr, (unsigned long)bin->ReelTrc);
      }
   }

   /* Print any remaining summary messages. */
   ddsBinMessage(bin, (DDS_BIN_STATE)0);

   return DDS_OK;
}
