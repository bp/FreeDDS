static char rcsid[]="$Id: readpreamble.c 219 2011-02-16 01:11:40Z user $";
/*
RCS-t=DDS API, read preamble data

*/

/*******************************************/
/***   Copyright (C) 2011                ***/
/***   BP Americam, Houston, TX          ***/
/***   Jerry Ehlers - January 2011       ***/
/*******************************************/

#include "dds_sys.h"
#include "dds_dump.h"

int ddsReadPreamble(
   DDS_BIN *bin)
{
   DDS_TYPE *type;
   DDS_MEDIA *media = bin->media;
   DDS_BOOLEAN dump;
   int n_read, n_bytes, residual;
   int green, n_prefix, n_suffix;
   int NumSmp, line_HlhByt, prefix, suffix; 
   char *buf;

   if((dds_state & DDS_DUMP_DATA) && (dds_state & DDS_DUMP_READ)) {
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
   /* Read USP line header    */
   /***************************/
      errno = 0;
      residual = n_read = 0;
      buf = bin->std.usp.line_buf;
      type = bin->std.usp.line.type;
      n_prefix = 0;

      if(bin->state & DDS_green_prefix) {
         n_prefix = bin->prefix.type->prec;
         green = type->stroke - n_prefix;
      }

      n_bytes = greadc(media->fp, buf, type->stroke);

      if(n_bytes != type->stroke) {
         /* no input line header */
         residual = n_bytes;
         if(geof(media->fp)) errno = 0;
         /* un-read some input data */
         gunread(media->fp, buf, n_bytes);
      } else {
         n_read = 1;
      }
            
      if(bin->std.usp.line.r_seam.thread) {
         /* evaluate map read expressions. */
         bin->std.usp.line.r_object->ptr = (void*)buf;
         RELOC_VALUE(bin->std.usp.line.r_object);
         ddsEvaluate(bin->std.usp.line.r_seam.thread); 
      }

      if(n_prefix) { 
         /* validate green word prefix */
         CON_ANSS(DDS_CTYPE_INT, bin->std.usp.line_prefix.type->ptype,
            &prefix, buf + bin->std.usp.line_prefix.offset);

         if(green != prefix) {
            ddsError("usp line header green word prefix= %i\n\tbinary= %s, ReelTrc= %lu\n",
               prefix,
               bin->name, (unsigned long)bin->ReelTrc);
         }
      }

      if (n_read == 1); {
         CON_ANSS(DDS_CTYPE_INT, bin->std.usp.line_HlhByt.type->ptype,
                  &line_HlhByt, buf + bin->std.usp.line_HlhByt.offset);
         if(line_HlhByt < 2 * bin->std.usp.line_HlhByt.type->prec ||
            line_HlhByt > 0x7fff) {
            ddsError("bogus usp line header HlhByt\n\tbinary= %s, ReelTrc= %lu\n",
                     bin->name, (unsigned long)bin->ReelTrc);
            break;
         } else if(bin->std.usp.line_green_min + line_HlhByt != type->stroke) {
            ddsWarn("bogus usp line header HlhByt\n\tbinary= %s, ReelTrc= %lu\n",
                    bin->name, (unsigned long)bin->ReelTrc);
         }
      }

      if(dump) ddsDumpDataBuf(bin, "read", (DDS_SYM*)type, buf);

      n_read = ddsReadCheck(bin, type, buf, 1, n_read, residual);
      if (n_read != 1) { return DDS_BAD; }

      break;
   case DDS_FMT_SEGY:
   case DDS_FMT_SEGY1:
      /***************************/
      /* Read SEGY card header  */
      /***************************/
      errno = 0;
      residual = n_read = 0;
      buf = bin->std.segy.card_buf;
      type = bin->std.segy.card.type;
      n_prefix = n_suffix = 0;

      if(bin->state & DDS_green_prefix) {
         n_prefix = bin->prefix.type->prec;
         if(bin->state & DDS_green_suffix) {
            n_suffix = bin->suffix.type->prec;
         }
         green = type->stroke - n_prefix - n_suffix;
      }

      n_bytes = greadc(media->fp, buf, type->stroke);

      if(n_bytes != type->stroke) {
         /* no input card header */
         residual = n_bytes;
         if(geof(media->fp)) errno = 0;
         /* un-read some input data */
         gunread(media->fp, buf, n_bytes);
      } else {
         n_read = 1;
      }

      if(bin->std.segy.card.r_seam.thread) {
         /* evaluate map read expressions. */
         bin->std.segy.card.r_object->ptr = (void*)buf;
         RELOC_VALUE(bin->std.segy.card.r_object);
         ddsEvaluate(bin->std.segy.card.r_seam.thread);
      }

      if(n_prefix) {
         /* validate green word prefix */
         CON_ANSS(DDS_CTYPE_INT, bin->prefix.type->ptype,
            &prefix, buf + bin->prefix.offset);

         if(n_suffix) {
            /* validate green word suffix */
            CON_ANSS(DDS_CTYPE_INT, bin->suffix.type->ptype,
               &suffix, buf + bin->suffix.offset);

            if(green != prefix || green != suffix) {
               ddsError("segy card header green word prefix= %i, suffix= %i\n\tbinary= %s, ReelTrc= %lu\n",
                  prefix, suffix,
                  bin->name, (unsigned long)bin->ReelTrc);
            }
         } else if(green != prefix) {
            ddsError("segy card header green word prefix= %i\n\tbinary= %s, ReelTrc= %lu\n",
               prefix, bin->name, (unsigned long)bin->ReelTrc);
         }
      }

      if(dump) ddsDumpDataBuf(bin, "read", (DDS_SYM*)type, buf);

      n_read = ddsReadCheck(bin, type, buf, 0, n_read, residual);
      if (n_read != 1) { return DDS_BAD; }

      /****************************/
      /* Read SEGY binary header */
      /****************************/
      errno = 0;
      residual = n_read = 0;
      buf = bin->std.segy.binary_buf;
      type = bin->std.segy.binary.type;
      n_prefix = n_suffix = 0;

      if(bin->state & DDS_green_prefix) {
         n_prefix = bin->prefix.type->prec;
         if(bin->state & DDS_green_suffix) {
            n_suffix = bin->suffix.type->prec;
         }
         green = type->stroke - n_prefix - n_suffix;
      }

      n_bytes = greadc(media->fp, buf, type->stroke);

      if(n_bytes != type->stroke) {
         /* no input binary header */
         residual = n_bytes;
         if(geof(media->fp)) errno = 0;
         /* un-read some input data */
         gunread(media->fp, buf, n_bytes);
      } else {
         n_read = 1;
      }

      if(bin->std.segy.binary.r_seam.thread) {
         /* evaluate map read expressions. */
         bin->std.segy.binary.r_object->ptr = (void*)buf;
         RELOC_VALUE(bin->std.segy.binary.r_object);
         ddsEvaluate(bin->std.segy.binary.r_seam.thread);
      }

      if(n_prefix) {
         /* validate green word prefix */
         CON_ANSS(DDS_CTYPE_INT, bin->prefix.type->ptype,
            &prefix, buf + bin->prefix.offset);

         if(n_suffix) {
            /* validate green word suffix */
            CON_ANSS(DDS_CTYPE_INT, bin->suffix.type->ptype,
               &suffix, buf + bin->suffix.offset);

            if(green != prefix || green != suffix) {
               ddsError("segy binary header green word prefix= %i, suffix= %i\n\tbinary= %s, ReelTrc= %lu\n",
                  prefix, suffix,
                  bin->name, (unsigned long)bin->ReelTrc);
            }
         } else if(green != prefix) {
            ddsError("segy binary header green word prefix= %i\n\tbinary= %s, ReelTrc= %lu\n",
               prefix, bin->name, (unsigned long)bin->ReelTrc);
         }
      }

      /* process NumSmp field */
      CON_ANSS(DDS_CTYPE_INT, bin->std.segy.binary_NumSmp.type->ptype,
               &NumSmp,  buf + bin->std.segy.binary_NumSmp.offset);
      if(bin->cube.axis[1].size != NumSmp) {
         ddsWarn("segy binary NumSmp= %i, expected %li\n\tbinary= %s, ReelTrc= %lu\n",
                 NumSmp, (long)bin->cube.axis[1].size,
                 bin->name, (unsigned long)bin->ReelTrc);
      }

      if(dump) ddsDumpDataBuf(bin, "read", (DDS_SYM*)type, buf);

      n_read = ddsReadCheck(bin, type, buf, 0, n_read, residual);
      if (n_read != 1) { return DDS_BAD; }

      break;
   }

   return DDS_OK;
}
