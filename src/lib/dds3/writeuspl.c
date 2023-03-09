static char rcsid[]="$Id: writeuspl.c 219 2011-02-16 01:11:40Z user $";
/*
RCS-t=DDS API, write usp line header binary data

*/

/*****************************************/
/***   Copyright (C) 2011              ***/
/***   BP America, Houston, TX         ***/
/***   Jerry Ehlers, January 2011      ***/
/*****************************************/

#include "dds_sys.h"
#include "dds_dump.h"

int ddsWriteUspL(
   DDS_BIN *bin,
   DDS_TYPE *type,
   const char *buffer,
   int count)
{
   int n_bytes;
   DDS_MEDIA *media = bin->media;
   HOST_INT6_T cur_offset;

   if (count != 1) {
      ddsError("Can only write 1 usp binary preamble, not count=%d\n\tbinary= %s\n",
               count, bin->name);
      return DDS_BAD;
   }

   memcpy(bin->std.usp.line_buf, buffer, type->stroke);

   if (!(bin->state & (DDS_BIN_STATE)(DDS_BIN_FREEZE | DDS_BIN_EOF | DDS_BIN_EOM)) &&
       !(bin->slice) && /* media is sliced */
       media && media->fp && 
       !(media->state & DDS_MEDIA_SWAP) && /* Multi-media processing */
       (media->genus == DDS_MEDIA_REG)) { /* Regular disk file */

      /* save current position */
      if(ggetpos(media->fp, &cur_offset)) {
         ddsError("seek getpos, %s\n\t%s, LineTrc= %lu\n",
                  strerror(errno), media->title.ptr,
                  (unsigned long)bin->LineTrc);
         return DDS_BAD;
      }

      /* point back to beginning of the Line Header */
      if(gsetpos(media->fp, &media->data_bias)) {
         ddsError("seek setpos, %s\n\t%s, data_bias= %lu\n",
                  strerror(errno), media->title.ptr,
                  (unsigned long)media->data_bias);
         return DDS_BAD;
      }

      /* rewrite line header to the output media */
      n_bytes = gwritec(media->fp, buffer, type->stroke);
      if (n_bytes != type->stroke) {
         ddsWarn("Only wrote %d usp line header bytes of %d\n",
                 n_bytes, type->stroke);
      }
      
      /* point back to the original position */
      if(gsetpos(media->fp, &cur_offset)) {
         ddsError("seek setpos, %s\n\t%s, current offset= %lu\n",
                  strerror(errno), media->title.ptr,
                  (unsigned long)cur_offset);
         return DDS_BAD;
      }
   }

   return(count);
}
