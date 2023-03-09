static char rcsid[]="$Id: writepreamble.c 219 2011-02-16 01:11:40Z user $";
/*
RCS-t=DDS API, write preamble data data

*/

/*******************************************/
/***   Copyright (C) 2011                ***/
/***   BP Americam, Houston, TX          ***/
/***   Jerry Ehlers - January 2011       ***/
/*******************************************/

#include "dds_sys.h"
#include "dds_dump.h"

int ddsWritePreamble(
   DDS_BIN *bin)
{
   DDS_TYPE *type;
   DDS_MEDIA *media = bin->media;
   DDS_BOOLEAN dump;
   int n_write, n_bytes, residual;
   int green, n_prefix, n_suffix;
   int line_HlhByt;
   const char *buf;

   if((dds_state & DDS_DUMP_DATA) && (dds_state & DDS_DUMP_WRITE)) {
      dump = DDS_TRUE;
   } else {
      dump = DDS_FALSE;
   }

   /****************************************/
   /* process format specific line headers */
   /****************************************/

   switch(bin->fmt) {
   case DDS_FMT_USP:
   /***************************/
   /* Write USP line header   */
   /***************************/
      errno = 0;
      residual = n_write = 0;
      buf = bin->std.usp.line_buf;
      type = bin->std.usp.line.type;

      CON_ANSS(DDS_CTYPE_INT, bin->std.usp.line_HlhByt.type->ptype,
               &line_HlhByt, buf + bin->std.usp.line_HlhByt.offset);
      if(line_HlhByt < 2 * bin->std.usp.line_HlhByt.type->prec ||
         line_HlhByt > 0x7fff) {
         ddsError("bogus usp line header HlhByt\n\tbinary= %s, ReelTrc= %lu\n",
                  bin->name, (unsigned long)bin->ReelTrc);
      } else {
         if(bin->std.usp.line_green_min + line_HlhByt != type->stroke) {
            ddsWarn("bogus usp line header HlhByt\n\tbinary= %s, ReelTrc= %lu\n",
                    bin->name, (unsigned long)bin->ReelTrc);
         }

         if(bin->std.usp.line.w_seam.thread) {
            /* Evaluate map write expressions. */
            /* This "(void*)" violates "const". */
            bin->std.usp.line.w_object->ptr = (void*)buf;
            RELOC_VALUE(bin->std.usp.line.w_object);
            ddsEvaluate(bin->std.usp.line.w_seam.thread);
         }

         /* write line header to the output media */
         n_bytes = gwritec(media->fp, buf, type->stroke);

         if(n_bytes != type->stroke) {
            /* error writing type */
            residual = n_bytes;
         } else {
            n_write = 1;
         }

         if(dump) ddsDumpDataBuf(bin, "write", (DDS_SYM*)type, buf);
      }
      n_write = ddsWriteCheck(bin, type, buf, 1, n_write, residual);
      if(n_write != 1) { return DDS_BAD; }

      break;
   case DDS_FMT_SEGY:
   case DDS_FMT_SEGY1:
      /***************************/
      /* Write SEGY card header  */
      /***************************/
      errno = 0;
      residual = n_write = 0;
      buf = bin->std.segy.card_buf;
      type = bin->std.segy.card.type;
      n_prefix = n_suffix = 0;

      if(bin->state & DDS_green_prefix) {
         n_prefix = bin->std.segy.card_prefix.type->prec;
         if(bin->state & DDS_green_suffix) {
            n_suffix = bin->std.segy.card_suffix.type->prec;
         }
         green = type->stroke - n_prefix - n_suffix;
      }

      if(n_prefix) {
         /* initialize green word prefix */
         CON_NASS(bin->std.segy.card_prefix.type->ptype, DDS_CTYPE_INT,
                  buf + bin->std.segy.card_prefix.offset, &green);
         if(n_suffix) {
            /* initialize green word suffix */
            CON_NASS(bin->std.segy.card_suffix.type->ptype, DDS_CTYPE_INT,
                     buf + bin->std.segy.card_suffix.offset, &green);
         }
      }

      if(bin->std.segy.card.w_seam.thread) {
         /* Evaluate map write expressions. */
         /* This "(void*)" violates "const". */
         bin->std.segy.card.w_object->ptr = (void*)buf;
         RELOC_VALUE(bin->std.segy.card.w_object);
         ddsEvaluate(bin->std.segy.card.w_seam.thread);
      }

      /* write card header to the output media */
      n_bytes = gwritec(media->fp, buf, type->stroke);

      if(n_bytes != type->stroke) {
         /* error writing type */
         residual = n_bytes;
      } else {
         n_write = 1;
      }

      if(dump) ddsDumpDataBuf(bin, "write", (DDS_SYM*)type, buf);
   
      n_write = ddsWriteCheck(bin, type, buf, 1, n_write, residual);
      if(n_write != 1) { return DDS_BAD; }

      /****************************/
      /* Write SEGY binary header */
      /****************************/
      errno = 0;
      residual = n_write = 0;
      buf = bin->std.segy.binary_buf;
      type = bin->std.segy.binary.type;
      n_prefix = n_suffix = 0;

      if(bin->state & DDS_green_prefix) {
         n_prefix = bin->std.segy.binary_prefix.type->prec;
         if(bin->state & DDS_green_suffix) {
            n_suffix = bin->std.segy.binary_suffix.type->prec;
         }
         green = type->stroke - n_prefix - n_suffix;
      }

      if(n_prefix) {
         /* initialize green word prefix */
         CON_NASS(bin->std.segy.binary_prefix.type->ptype, DDS_CTYPE_INT,
                  buf + bin->std.segy.binary_prefix.offset, &green);
         if(n_suffix) {
            /* initialize green word suffix */
            CON_NASS(bin->std.segy.binary_suffix.type->ptype, DDS_CTYPE_INT,
                     buf + bin->std.segy.binary_suffix.offset, &green);
         }
      }

      if(bin->std.segy.binary.w_seam.thread) {
         /* Evaluate map write expressions. */
         /* This "(void*)" violates "const". */
         bin->std.segy.binary.w_object->ptr = (void*)buf;
         RELOC_VALUE(bin->std.segy.binary.w_object);
         ddsEvaluate(bin->std.segy.binary.w_seam.thread);
      }

      /* write binary header to the output media */
      n_bytes = gwritec(media->fp, buf, type->stroke);

      if(n_bytes != type->stroke) {
         /* error writing type */
         residual = n_bytes;
      } else {
         n_write = 1;
      }

      if(dump) ddsDumpDataBuf(bin, "write", (DDS_SYM*)type, buf);

      n_write = ddsWriteCheck(bin, type, buf, 1, n_write, residual);
      if(n_write != 1) { return DDS_BAD; }

      break;
   }

   return DDS_OK;
}
