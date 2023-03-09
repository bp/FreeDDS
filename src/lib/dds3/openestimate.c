static char rcsid[]="$Id: openestimate.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, estimate required media (quantity and minimum i/o time).

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

#define SEC_PER_MOUNT 90

#define REWIND_INCHES_PER_SEC 8

#define SEC_PER_IO_REQUEST 0.014

#define MARGIN_PERCENT .10

void ddsOpenEstimate(
   DDS_BIN *bin)
{
   DDS_MEDIA *chain, *media = bin->media;
   int block_size, blocks_per_media;
   int n, bytes_per_margin, blocks_per_margin;
   double block_toll, bytes_per_media;
   double total_bytes, total_blocks;
   double media_count;
   int media_mount;
   int total_hours, total_minutes, total_seconds;

   if(bin->state & DDS_BIN_UNBLOCK) {
      /* unbuffered physical i/o */
      if(bin->cube.io_rank <= bin->cube.rank) {
         block_size = bin->cube.axis[bin->cube.io_rank].stride;
      } else {
         block_size = 1000; /* arbitrary kludge */
      }
   } else if(bin->block_size > 0) {
      /* nominal block size */
      block_size = bin->block_size;
   }

   /* inches of media per i/o block */
   block_toll = media->gap + block_size * media->toll;

   if(media->capacity > 0) {
      blocks_per_media = media->capacity / block_size;
      /* back out effective media length, given byte capacity */
      media->length = (block_toll * blocks_per_media) / INCHES_PER_FOOT;

      if(! media->length) {
         /* limit, when capacity is artificially reduced */
         media->length = 1;
         blocks_per_media = (INCHES_PER_FOOT * media->length) / block_toll;
      }

      /* update length in chained media */
      for(chain = media->next; chain; chain = chain->next) {
         chain->length = media->length;
      }
   } else {
      blocks_per_media = (media->length * media->passes * INCHES_PER_FOOT) /
         block_toll;
   }

   /* reduce the number of blocks per media by the duty factor */
   blocks_per_media *= media->duty;

   /* estimate the effective number of bytes per media */
   bytes_per_media = (double)blocks_per_media * (double)block_size;

   /* assume the hyper cube size is reliable */
   total_bytes = (double)bin->cube.axis[bin->cube.rank].stride *
      (double)bin->cube.axis[bin->cube.rank].size;

   total_blocks = total_bytes / block_size;

   if(bin->state & DDS_BIN_OPEN_CREATE) {
      media_count = total_bytes / bytes_per_media;
      media_mount = media_count + 1;
   } else {
      media_mount = 1;
      for(chain = media->next; chain; chain = chain->next) {
         media_mount++;
      }
      media_count = media_mount;
   }

   /* overhead to mount and dismount tapes */
   total_seconds = media_mount * SEC_PER_MOUNT;

   /* overhead for media rewind */
   total_seconds += (total_blocks * block_toll) /
      REWIND_INCHES_PER_SEC;

   /* overhead per physical i/o request */
   total_seconds += total_blocks * SEC_PER_IO_REQUEST;

   /* overhead for physical i/o */
   total_seconds += total_bytes / media->rate;

   /* convert to hours, minutes, seconds */
   total_hours = total_seconds / 3600;
   total_seconds -= 3600 * total_hours;
   total_minutes = total_seconds / 60;
   total_seconds -= 60 * total_minutes;

   ddsOperator("estimated media= %.1f tapes, at capacity= %.3f Gbytes per tape\n\tminimum time= %i hrs %i min at %.3f Mbytes/sec\n\t%s %s\n",
       media_count, (double)(bytes_per_media / DDS_GB),
       total_hours, total_minutes, (double)(media->rate / DDS_MB),
       ((bin->state & DDS_BIN_OPEN_CREATE) ? "write" : "read"),
       media->title.ptr);

   media->balance = media->length * INCHES_PER_FOOT * media->duty;

   bin->cube.chunk = (1 + 0.02 * blocks_per_media) * block_toll;

   /* Margin and cleave are used to change media on record boundaries. */
   /* Maximum number of bytes that may be wasted, when changing media */
   /* on a sub-cube boundary and only a little media capacity remains. */
   bytes_per_margin = (MARGIN_PERCENT * blocks_per_media) * block_size;

   for(n = bin->cube.rank; n; n--) {
      if(bytes_per_margin > bin->cube.axis[n].stride ||
         n <= bin->cube.io_rank) {
         bin->cube.cleave = n ? (n - 1) : 0;
         blocks_per_margin = 1 + bin->cube.axis[n].size / block_size;
         bin->cube.chunk = block_toll * blocks_per_margin;
         break;
      }
   }

   return;
}
