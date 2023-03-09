static char rcsid[]="$Id: c_lseek8.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, seek (8 byte precision) within binary data

*/

/**********************************************/
/***   Copyright (C) 1996,2004              ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

HOST_INT6_T cdds_lseek8(
   BIN_TAG bin_tag,
   SYM_TAG sym_tag,
   HOST_INT6_T offset,
   int whence)
{
   DDS_BIN *bin;
   DDS_TYPE *type;
   HOST_INT6_T new_offset;
   /* This are for John's special I/O */
   int swap=0; /* for STREAM_TO_SYM_TAG */

   char buffer[1];

   DDS_API_ENTRY("lseek8")

   BIN_TAG_TO_BIN(bin, bin_tag, DDS_API_EXIT(DDS_BAD));
   if (bin->bin_tag >= 0) {
	   bin_tag = bin->bin_tag;
      BIN_TAG_TO_BIN(bin, bin_tag, DDS_API_EXIT(DDS_BAD));
   }

   STREAM_TO_SYM_TAG(sym_tag, swap);

   SYM_TAG_TO_TYPE(type, bin, sym_tag, DDS_API_EXIT(DDS_BAD));

   WHENCE_CODE(whence, bin, DDS_API_EXIT(DDS_BAD));

   /******************************/
   /* clear DDS_BIN_PADDING flag */
   /******************************/

   bin->state &= (DDS_BIN_STATE) ~ DDS_BIN_PADDING;

   if(bin->state & (DDS_BIN_FREEZE | DDS_TRACE_READ | DDS_TRACE_WRITE |
      DDS_BIN_EOF | DDS_BIN_EOM |
      DDS_BIN_DESEEK |
      DDS_BIN_MSG_READ | DDS_BIN_MSG_WRITE)) {
      /*****************************/
      /* seek exception processing */
      /*****************************/

      bin->state &= (DDS_BIN_STATE) ~ DDS_BIN_DESEEK;

      /*********************************************************/
      /* Caution: bin->media will change, if media is mounted. */
      /*********************************************************/

      /* flush message summary */ 
      ddsBinMessage(bin, (DDS_BIN_STATE)0);

      if(bin->slice) goto nominal;

      if(bin->media->genus == DDS_MEDIA_NULL) {
         /* nop, media is /dev/null or "memory/buffer" */
         DDS_API_EXIT(0);
      }

      /* Freeze dictionary and/or process line header. */
      if(bin->state & DDS_BIN_OPEN_CREATE) {
         /* Create line headers by writing _zero_ traces. */
         if(! (bin->state & DDS_BIN_EOM)) {
            cdds_write(bin_tag, sym_tag, buffer, 0);
         }
      } else {
         /* Read/update existing mode. */
         if(! (bin->state & (DDS_BIN_EOF | DDS_BIN_EOM))) {
            /* Consume line headers by reading _zero_ traces. */
            cdds_read(bin_tag, sym_tag, buffer, 0);
         }
      }

      if(bin->state & DDS_BIN_FREEZE) {
         /* freeze the history and begin binary i/o */
         ddsBinFreeze(bin);
      }
   }

   /***************************/
   /* Nominal seek processing */
   /***************************/

   nominal:
   switch(bin->fmt) {
   case DDS_FMT_GENERIC:
      if(bin->slice) {
         new_offset = ddsSeekSlice(bin, type, offset, whence);
      } else if (bin->state & DDS_BIN_ZIP && sym_tag) {
         ddsError("'zip' file, can only seek on a trace-basis\n"); 
         new_offset = -1;
      } else {
         new_offset = ddsSeekType(bin, type, offset, whence);
      }
      break;
   case DDS_FMT_USP:
      if(type == bin->std.usp.line.type) {
         new_offset = ddsSeekHeader(bin, type, offset, whence);
      } else if(bin->slice) {
         new_offset = ddsSeekSlice(bin, type, offset, whence);
      } else {
         new_offset = ddsSeekType(bin, type, offset, whence);
      }
      break;
   case DDS_FMT_SEGY:
   case DDS_FMT_SEGY1:
      if(type == bin->std.segy.card.type) {
         new_offset = ddsSeekHeader(bin, type, offset, whence);
      } else if(type == bin->std.segy.binary.type) {
         new_offset = ddsSeekHeader(bin, type, offset, whence);
      } else if(bin->slice) {
         new_offset = ddsSeekSlice(bin, type, offset, whence);
      } else {
         new_offset = ddsSeekType(bin, type, offset, whence);
      }
      break;
   case DDS_FMT_SU:
      if(bin->slice) {
         new_offset = ddsSeekSlice(bin, type, offset, whence);
      } else {
         new_offset = ddsSeekType(bin, type, offset, whence);
      }
      break;
   }

   DDS_API_EXIT(new_offset);
}
