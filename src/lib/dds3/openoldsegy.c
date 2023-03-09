static char rcsid[]="$Id: openoldsegy.c 284 2014-12-08 21:37:19Z user $";
/*
RCS-t=DDS, open old binary data, segy format

*/

/***********************************/
/***   Copyright (C) 2004        ***/
/***   BP America, Houston, TX   ***/
/***   Jerry Ehlers, Oct 2004    ***/
/***********************************/

#include "dds_sys.h"
#include "segy_hdr.h"
#include "dds_dump.h"

void ddsOpenOldSegy(
   DDS_BIN *bin,
   DDS_CUBE *old_cube)
{
   SEGY_LINE dict_hdr, bin_hdr;
   DDS_PTYPE char_stype, ushort_stype, short_stype, int_stype, float_stype;
   DDS_PTYPE green_stype, samples_stype;
   DDS_BOOLEAN ignore_FmtCode, ascii_nor_ebcdic;
   DDS_TYPE *type;
   DDS_MEMBER *member;
   DDS_BLOCK *block, *hdr_block;
   DDS_MEDIA *media = bin->media;
   DDS_AXIS *axis;
   char *hdr_buf, *card_buf, *ext_buf, *binary_buf, *trace_buf;
   char *str, *end, *buf;
   int i, j, k, n;
   int rank, axis_t, axis_x, axis_y, axis_z;
   const char *segy_units, *cube_units;
   double sec_to_cube;
   int card_read, binary_read, trace_read, overload;
   int green_test, green_test4, green_test8;
   int green_prefix_prec, green_suffix_prec;
   int green_card, green_binary, green_trace;
   int NumSmp, NumTrc, SmpInt, SegyFmt, NumHdr;
   int FmtCode, segy_FmtCode, SU, isReg;
   int sample_prec, hdrswap;
   int card_pad, binary_pad, trace_pad;
   size_t card_max, binary_max, trace_max;
   unsigned char test_char;
   STRUCTSTAT stat_buf;
   HOST_INT6_T size;

   /***********************************/
   /* fake su format as segy for now  */
   /***********************************/

   SU = (bin->fmt == DDS_FMT_SU);
   if (SU) {
      cdds_dict(bin->dict_open, "print");
      cdds_printf("$fmt:su","fmt:segy\n");
      bin->fmt = DDS_FMT_SEGY;
      bin->fmt_name = ddsString("segy");
   }

   /***********************************/
   /* retrieve read/write trace flags */
   /***********************************/

   cdds_dict(bin->dict_open, "scan");

   /* current media offset is the start of the PREAMBLE (card header). */
   ggetpos(bin->media->fp, &bin->media->data_bias);

   /* is this seekable? */
   if (ddsFSTAT(gfileno(bin->media->fp), &stat_buf) >= 0) {
      isReg = S_ISREG(stat_buf.st_mode);
   } else {
      isReg = DDS_FALSE;
   }

   /*************************************/
   /* retrieve default cube description */
   /*************************************/

   ddsCubeScanBin(bin, old_cube);

   if(1 != bin->slice_max) {
      ddsError("open: one old file and cube slice count= %i\n\tdict= %s\n",
         (int)bin->slice_max, bin->dict_open);
      return;
   }

   cdds_dict(bin->dict_open, "scan");

   /************************/
   /* Check for SU Format  */
   /************************/

   if (SU) {
      bin->std.segy.flags |= DDS_SEGY_NumSmp;
   }
     
   /************************/
   /* Check for segy label */
   /************************/

   card_max = 128;
   card_buf = (char*)ddsMalloc(card_max);
   card_read = greadc(bin->media->fp, card_buf, card_max);
   if(card_read < 128) {
      ddsError("check segy label, read=%i bytes, minimum=128 errno=%s(%d)\n", 
               card_read, strerror(bin->media->fp->error), 
               bin->media->fp->error);
      return;
   }
   /* if not segy label, push the buffer back */
   if (memcmp(&card_buf[4], "SY1.0RECORD",11)) {
      gunread(bin->media->fp, card_buf, card_read);
   }

   /***********************************************************/
   /* Determine green words for segy card header.             */
   /* Save card length in green_card.                         */
   /* These are required to extract other card header values. */
   /***********************************************************/

   card_max = SEGY_CARD_MAX + 2 * SEGY_GREEN_WORD_MAX;
   card_buf = (char*)ddsRealloc(card_buf, card_max);
   card_read = greadc(bin->media->fp, card_buf, card_max);
   if(card_read < SEGY_CARD_MIN) {
      ddsError("segy card header read size= %i, minimum= %i\n",
         card_read, (int)SEGY_CARD_MIN);
      return;
   }

   /* Extract green word prefix from card header. */
   CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED4, &green_test4, card_buf);
   CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED8, &green_test8, card_buf);

   if (SU) {
      green_prefix_prec = 0;
      green_card = 0;
   } else if(0 < cdds_scank("format", "green_prefix")) {
      /* green word prefix is *required* */
      green_prefix_prec = 0;
      cdds_scanf("", "%*s %d", &green_prefix_prec);
      if(! green_prefix_prec) {
         /* guess the green word precision */
         if(green_test8 >= SEGY_CARD_MIN && green_test8 <= SEGY_CARD_MAX) {
            green_prefix_prec = 8;
            green_card = green_test8;
         } else {
            green_prefix_prec = 4;
            green_card = green_test4;
         }
      } else if(green_prefix_prec == 8) {
         green_card = green_test8;
      } else if(green_prefix_prec == 4) {
         green_card = green_test4;
      } else {
         ddsWarn("SEGY green prefix precision illegal= %i\n\tbinary= %s\n",
                 green_prefix_prec, bin->name);
      }
   } else {
      green_prefix_prec = 0;
      green_card = SEGY_CARD_MIN;
   /* Automatic green word detection disabled JWE Nov 06.
    * Rarely used and too dangerous.
   } else if(0 < cdds_scank("format", "no_green_prefix")) {
      green_prefix_prec = 0;
      green_card = SEGY_CARD_MIN;
   } else {
      if(green_test8 >= SEGY_CARD_MIN && green_test8 <= SEGY_CARD_MAX) {
         green_prefix_prec = 8;
         green_card = green_test8;
      } else if(green_test4 >= SEGY_CARD_MIN && green_test4 <= SEGY_CARD_MAX) {
         green_prefix_prec = 4;
         green_card = green_test4;
      } else {
         green_prefix_prec = 0;
         green_card = SEGY_CARD_MIN;
      }
   */
   }
   if((green_card < SEGY_CARD_MIN || green_card > SEGY_CARD_MAX)
      && !SU) {
      ddsWarn("SEGY green card prefix illegal= %i\n\tbinary= %s\n",
                  green_card, bin->name);
   }

   /* Extract green word suffix from card header. */
   CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED4,
      &green_test4, card_buf + green_prefix_prec + green_card);
   CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED8,
      &green_test8, card_buf + green_prefix_prec + green_card);

   if (SU) {
      green_suffix_prec = 0;
   } else if(0 < cdds_scank("format", "green_suffix")) {
      /* green word suffix is *required* */
      bin->state  |= DDS_green_suffix;
      green_suffix_prec = 0;
      cdds_scanf("", "%*s %d", &green_suffix_prec);
      if(! green_suffix_prec) {
         /* guess the green word precision */
         if(green_test8 == green_card) {
            green_suffix_prec = 8;
         } else {
            green_suffix_prec = 4;
            if (green_test4 != green_card) {
               ddsWarn("SEGY green card suffix illegal= %i\n\tbinary= %s\n",
                  green_test4, bin->name);
            }
         }
      } else if(green_suffix_prec == 8) {
         if (green_test8 != green_card) {
            ddsWarn("SEGY green card suffix illegal= %i\n\tbinary= %s\n",
               green_test8, bin->name);
         }
      } else if(green_suffix_prec == 4) {
         if (green_test4 != green_card) {
            ddsWarn("SEGY green card suffix illegal= %i\n\tbinary= %s\n",
               green_test4, bin->name);
         }
      } else {
         ddsWarn("SEGY green suffix precision illegal= %i\n\tbinary= %s\n",
            green_suffix_prec, bin->name);
      }
   } else {
      green_suffix_prec = 0;
   /* Automatic green word detection disabled JWE Nov 06.
    * Rarely used and too dangerous.
   } else if(0 < cdds_scank("format", "no_green_suffix")) {
      green_suffix_prec = 0;
   } else {
      if(green_test8 == green_card) {
         bin->state  |= DDS_green_suffix;
         green_suffix_prec = 8;
      } else if(green_test4  == green_card) {
         bin->state  |= DDS_green_suffix;
         green_suffix_prec = 4;
      } else {
         green_suffix_prec = 0;
      }
   */
   }

   test_char = (unsigned char)card_buf[green_prefix_prec];
   ascii_nor_ebcdic = DDS_FALSE;
   if(0 < cdds_scank("format", "ebcdic")) {
      char_stype = DDS_STYPE_EBCDIC1;
   } else if(0 < cdds_scank("format", "ascii")) {
      char_stype = DDS_STYPE_ASCII1;
   } else if((unsigned char)DDS_EBCDIC_BLANK == test_char ||
      (unsigned char)DDS_EBCDIC_t == test_char ||
      (unsigned char)DDS_EBCDIC_C == test_char ||
      (unsigned char)DDS_EBCDIC_c == test_char) {
      char_stype = DDS_STYPE_EBCDIC1;
   } else if((unsigned char)DDS_ASCII_BLANK == test_char ||
      (unsigned char)DDS_ASCII_t == test_char ||
      (unsigned char)DDS_ASCII_C == test_char ||
      (unsigned char)DDS_ASCII_c == test_char) {
      char_stype = DDS_STYPE_ASCII1;
   } else {
      ascii_nor_ebcdic = DDS_TRUE;
      char_stype = DDS_STYPE_EBCDIC1;
   }

   if (green_prefix_prec) {
      bin->state  |= DDS_green_prefix;
   }
   if (green_suffix_prec) {
      bin->state  |= DDS_green_suffix;
   }

   card_pad = 0;

   /********************************************/
   /* unread everything except the card header */
   /********************************************/

   card_max = green_prefix_prec + green_card + green_suffix_prec;
   overload = card_read - card_max;

   if(overload > 0) {
      /* push back everything except the segy card header */
      gunread(bin->media->fp, card_buf + card_max, overload);
   } else if(overload < 0) {
      ddsError("short SEGY card header, overload= %i\n\tbinary= %s\n",
         overload, bin->name);
   }

   /************************/
   /* Get the binary size. */
   /************************/

   binary_max = SEGY_BINARY_MAX + 2 * SEGY_GREEN_WORD_MAX;
   binary_buf = (char*)ddsMalloc(binary_max);
   binary_read = greadc(bin->media->fp, binary_buf, binary_max);
   if(binary_read < SEGY_BINARY_MIN) {
      ddsError("segy binary header read size= %i, minimum= %i\n",
         binary_read, (int)SEGY_BINARY_MIN);
      return;
   }

   /* Extract green word prefix from binary header. */
   if (green_prefix_prec == 8) {
      CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED8, &green_binary, binary_buf);
   } else if (green_prefix_prec == 4) {
      CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED4, &green_binary, binary_buf);
   } else if (SU) {
      green_binary = 0;
   } else {
      green_binary = SEGY_BINARY_MIN;
   }
   if((green_binary < SEGY_BINARY_MIN || green_binary > SEGY_BINARY_MAX)
      && !SU) {
      ddsWarn("SEGY green binary prefix illegal= %i\n\tbinary= %s\n",
            green_suffix_prec, bin->name);
   }

   if (green_suffix_prec == 8) {
      CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED8,
         &green_test, binary_buf + 8 + green_binary);
   } else if (green_suffix_prec == 4) {
      CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED4,
         &green_test, binary_buf + 4 + green_binary);
   } else {
      green_test = green_binary;
   }
   if(green_test != green_binary) {
      ddsWarn("SEGY green binary suffix illegal= %i\n\tbinary= %s\n",
                  green_test, bin->name);
   }

   binary_pad = 0;

   /**********************************************/
   /* unread everything except the binary header */
   /**********************************************/

   binary_max = green_prefix_prec + green_binary + green_suffix_prec;
   overload = binary_read - binary_max;

   if(overload > 0) {
      /* push back everything except the segy binary header */
      gunread(bin->media->fp, binary_buf + binary_max, overload);
   } else if(overload < 0) {
      ddsError("short SEGY binary header, overload= %i\n\tbinary= %s\n",
         overload, bin->name);
   }

   /**************************************/
   /* Check for Extended Textual Headers */
   /**************************************/

   if (bin->fmt == DDS_FMT_SEGY1) {
      SegyFmt = 0x0100;
   } else {
      SegyFmt = 0;
   }

   CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED2, &NumHdr, 
            binary_buf + green_prefix_prec + 304); 

   if (SegyFmt == 0x0100 && NumHdr != 0) {
      ext_buf = (char*)ddsMalloc(card_max);
      if (NumHdr < 0) {
         for(;;) {
            /* read each Extended Textual Header until EndText found */
            card_read = greadc(bin->media->fp, ext_buf, card_max);
            if(card_read != card_max) {
               ddsError("segy extended header read size= %i, minimum= %i\n",
                        card_read, card_max);
               return;
            }
            if (strstr(&ext_buf[green_prefix_prec], "((EndText))")) break;
         }
      } else if (NumHdr > 0) {
         for(i=0;i<NumHdr;i++) {
            /* read each Extended Textual Header specified */
            card_read = greadc(bin->media->fp, ext_buf, card_max);
            if(card_read != card_max) {
               ddsError("segy extended header read size= %i, minimum= %i\n",
                        card_read, card_max);
               return;
            }
         }
      }
      ddsFree(ext_buf);
   }

   /**************************************************************/
   /* current media offset is the start of the SEQUENCE (traces) */
   /**************************************************************/

   ggetpos(bin->media->fp, &bin->media->trace_bias);

   /********************************************************/
   /* Extract "cube" description from binary header, i.e.  */
   /* NumSmp, NumTrc, FmtCode.                             */
   /********************************************************/

   FmtCode = 0;
   dict_hdr.FmtCode = 0;

   if (SU) {
      NumTrc = 1;
      FmtCode = SEGY_FMTCODE_FLOAT4;
#ifdef HOST_SWAP
      CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED2X, &NumSmp, 
               binary_buf + 114);
      CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED2X, &SmpInt, 
               binary_buf + 116);
#else
      CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED2, &NumSmp, 
               binary_buf + 114);
      CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED2, &SmpInt, 
               binary_buf + 116);
#endif
   } else if(0 < cdds_scank("format", "hdrswap")) {
      CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED2X, &NumTrc, 
               binary_buf + green_prefix_prec + 12);
      CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED2X, &SmpInt, 
               binary_buf + green_prefix_prec + 16);
      CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED2X, &NumSmp, 
               binary_buf + green_prefix_prec + 20);
      CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED2X, &FmtCode, 
               binary_buf + green_prefix_prec + 24);
   } else {
      CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED2, &NumTrc, 
               binary_buf + green_prefix_prec + 12);
      CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED2, &SmpInt, 
               binary_buf + green_prefix_prec + 16);
      CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED2, &NumSmp, 
               binary_buf + green_prefix_prec + 20);
      CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED2, &FmtCode, 
               binary_buf + green_prefix_prec + 24);
   }

   if(1 == cdds_scanf("segy_FmtCode", "%d", &segy_FmtCode)) {
      dict_hdr.FmtCode = segy_FmtCode;
      if(FmtCode != segy_FmtCode) {
         ddsWarn("dict segy_FmtCode= %i overrides binary FmtCode= %i\n\tbinary= %s\n",
            segy_FmtCode, FmtCode, bin->name);
         FmtCode = segy_FmtCode;
      }
   }

   if (green_prefix_prec == 8) {
      green_stype = DDS_STYPE_UNSIGNED8;
   } else if (green_prefix_prec == 4) {
      green_stype = DDS_STYPE_UNSIGNED4;
   } else {
      green_stype = DDS_CTYPE_VOID;
   }

   if (SU) {
#ifdef HOST_SWAP
      ushort_stype = DDS_STYPE_UNSIGNED2X;
      short_stype = DDS_STYPE_INTEGER2X;
      int_stype = DDS_STYPE_INTEGER4X;
#else
      ushort_stype = DDS_STYPE_UNSIGNED2;
      short_stype = DDS_STYPE_INTEGER2;
      int_stype = DDS_STYPE_INTEGER4;
#endif
   } else if(0 < cdds_scank("format", "hdrswap")) {
      ushort_stype = DDS_STYPE_UNSIGNED2X;
      short_stype = DDS_STYPE_INTEGER2X;
      int_stype = DDS_STYPE_INTEGER4X;
   } else {
      ushort_stype = DDS_STYPE_UNSIGNED2;
      short_stype = DDS_STYPE_INTEGER2;
      int_stype = DDS_STYPE_INTEGER4;
   }
   ignore_FmtCode = DDS_FALSE;
   if(0 < cdds_scank("format", "integer")) {
      sample_prec = 0;
      cdds_scanf("", "%*s %d", &sample_prec);
      if(sample_prec == 1) {
         if(FmtCode != SEGY_FMTCODE_INTEGER1) {
            FmtCode = SEGY_FMTCODE_INTEGER1;
            ignore_FmtCode = DDS_TRUE;
         }
         samples_stype = DDS_STYPE_INTEGER1;
      } else if(sample_prec == 2) {
         if(FmtCode != SEGY_FMTCODE_INTEGER2 &&
            FmtCode != SEGY_FMTCODE_INTEGER2a) {
            FmtCode = SEGY_FMTCODE_INTEGER2;
            ignore_FmtCode = DDS_TRUE;
         }
         samples_stype = DDS_STYPE_INTEGER2;
      } else if(sample_prec == 4) {
         if(FmtCode != SEGY_FMTCODE_INTEGER4) {
            FmtCode = SEGY_FMTCODE_INTEGER4;
            ignore_FmtCode = DDS_TRUE;
         }
         samples_stype = DDS_STYPE_INTEGER4;
      } else if(sample_prec == 8) {
         if(FmtCode != 0) ignore_FmtCode = DDS_TRUE;
         samples_stype = DDS_STYPE_INTEGER8;
      } else if(FmtCode == SEGY_FMTCODE_INTEGER4) {
         samples_stype = DDS_STYPE_INTEGER4;
      } else if(FmtCode == SEGY_FMTCODE_INTEGER2) {
         samples_stype = DDS_STYPE_INTEGER2;
      } else if(FmtCode == SEGY_FMTCODE_INTEGER2a) {
         samples_stype = DDS_STYPE_INTEGER4;
      } else if(FmtCode == SEGY_FMTCODE_INTEGER1) {
         samples_stype = DDS_STYPE_INTEGER1;
      } else {
         ignore_FmtCode = DDS_TRUE;
         FmtCode = SEGY_FMTCODE_INTEGER4;
         samples_stype = DDS_STYPE_INTEGER4;
      }
   } else if(0 < cdds_scank("format", "float")) {
      sample_prec = 0;
      cdds_scanf("", "%*s %d", &sample_prec);
      if(0 < cdds_scank("format", "ieee")) {
         if(sample_prec == 8) {
            if(FmtCode != 0) ignore_FmtCode = DDS_TRUE;
            samples_stype = DDS_STYPE_FLOAT8;
         } else {
            if(FmtCode != SEGY_FMTCODE_FLOAT4) {
               ignore_FmtCode = DDS_TRUE;
               FmtCode = SEGY_FMTCODE_FLOAT4;
            }
            samples_stype = DDS_STYPE_FLOAT4;
         }
      } else if(0 < cdds_scank("format", "ieeex")) {
         ignore_FmtCode = DDS_TRUE;
         if(sample_prec == 8) {
            samples_stype = DDS_STYPE_FLOAT8X;
         } else {
            samples_stype = DDS_STYPE_FLOAT4X;
         }
      } else if(sample_prec == 8) {
         if(FmtCode != 0) ignore_FmtCode = DDS_TRUE;
         samples_stype = DDS_STYPE_FLOAT8IBM;
      } else {
         if(FmtCode != SEGY_FMTCODE_FLOAT4IBM) {
            ignore_FmtCode = DDS_TRUE;
            FmtCode = SEGY_FMTCODE_FLOAT4IBM;
         }
         samples_stype = DDS_STYPE_FLOAT4IBM;
      }
   } else if(SU) {
#ifdef HOST_SWAP
      samples_stype = DDS_STYPE_FLOAT4X;
#else
      samples_stype = DDS_STYPE_FLOAT4;
#endif
   } else if(FmtCode == SEGY_FMTCODE_FLOAT4IBM) {
      samples_stype = DDS_STYPE_FLOAT4IBM;
   } else if(FmtCode == SEGY_FMTCODE_INTEGER4) {
      samples_stype = DDS_STYPE_INTEGER4;
   } else if(FmtCode == SEGY_FMTCODE_INTEGER2) {
      samples_stype = DDS_STYPE_INTEGER2;
   } else if(FmtCode == SEGY_FMTCODE_INTEGER2a) {
      samples_stype = DDS_STYPE_INTEGER4;
   } else if(FmtCode == SEGY_FMTCODE_FLOAT4) {
      samples_stype = DDS_STYPE_FLOAT4;
   } else if(FmtCode == SEGY_FMTCODE_INTEGER1) {
      samples_stype = DDS_STYPE_INTEGER1;
   } else {
      ignore_FmtCode = DDS_TRUE;
      FmtCode = SEGY_FMTCODE_FLOAT4IBM;
      samples_stype = DDS_STYPE_FLOAT4IBM;
   }

   if(bin->cube.axis[1].flag & DDS_AXIS_size) {
      if(bin->cube.axis[1].size != NumSmp) {
         ddsWarn("ddsOpenOldSegy: size.%s= %li overrides segy binary line header NumSmp= %i\n\tbinary=%s\n",
         bin->cube.axis[1].name,
         (long)bin->cube.axis[1].size,
         (int)NumSmp, bin->name);
      }
      NumSmp = bin->cube.axis[1].size;
   } else if(! cdds_scanf("segy_NumSmp_KLUDGE", "%d", &NumSmp)) {
      ddsWarn("Rev 2 'segy_NumSmp_KLUDGE' is obsolete\n\tDDS Rev 3 uses 'size.%s'\n",
         bin->cube.axis[1].name);
   }

   if (samples_stype == DDS_STYPE_INTEGER1 ||
       samples_stype == DDS_STYPE_INTEGER2 ||
       samples_stype == DDS_STYPE_INTEGER4 ||
       samples_stype == DDS_STYPE_INTEGER8) {
      float_stype = DDS_STYPE_FLOAT4;
   } else {
      float_stype = samples_stype;
   }

   /* check if complex */
   if(0 < cdds_scank("format", "complex")) {
      if(NumSmp % 2) {
         ddsError("complex data with an odd number of floats\n\tbinary=%s\n",
            bin->name);
      } else if(samples_stype == DDS_STYPE_FLOAT4) {
         samples_stype = DDS_STYPE_COMPLEX8;
         NumSmp = NumSmp/2;
      } else if(samples_stype == DDS_STYPE_FLOAT4IBM) {
         samples_stype = DDS_STYPE_COMPLEX8IBM;
         NumSmp = NumSmp/2;
      } else {
         ddsError("complex data only allowed for ieee or ibm\n\tbinary=%s\n",
            bin->name);
      }
   }

   /************************************************/
   /* Validate segy binary header essential values */
   /************************************************/

   if(!SU && ascii_nor_ebcdic) {
      ddsWarn("segy card header is neither ascii nor ebcdic\n\tbinary=%s\n",
         bin->name);
   }

   if(ignore_FmtCode) {
      ddsWarn("format definition overrides FmtCode= %i\n\tbinary= %s\n",
         FmtCode, bin->name);
      if(ascii_nor_ebcdic) {
         ddsWarn("attempt to open non-segy data using format=segy\n\tbinary=%s\n",
            bin->name);
         /* Continue anyway - JWE May 2011
         return;
         */
      }
   }

   if(NumSmp < 0) {
      ddsWarn("'NumSmp' binary header value is invalid (%i)\n\tbinary= %s\n",
         NumSmp, bin->name);
      NumSmp = 0;
   }

   if(SmpInt < 0) {
      ddsWarn("'SmpInt' binary header value is invalid (%i)\n\tbinary= %s\n",
         SmpInt, bin->name);
      SmpInt = 1;
   }

   if(bin->state & DDS_green_prefix) {
      if(green_binary < SEGY_BINARY_MIN) {
         ddsError("SEGY binary header green word= %i, minimum= %i\n\tbinary= %s\n",
            green_binary, SEGY_BINARY_MIN, bin->name);
         return;
      }
   }

   type = dds_prime[samples_stype].type;
   sample_prec = type->prec;
   bin->std.segy.sample_prec = sample_prec;

   /********************************/
   /* Compute size of trace buffer */
   /********************************/

   if (green_prefix_prec > 0) {
      trace_max = green_prefix_prec + SEGY_TRACE_MIN + NumSmp * sample_prec 
         + green_suffix_prec;
      trace_read = greada(bin->media->fp, (const void**)&trace_buf,
         trace_max, GIO_CHAR);
      if(trace_read < trace_max) {
         ddsError("segy trace read size= %i, expected size= %i\n",
            trace_read, trace_max);
         return;
      }
      CON_ANSS(DDS_CTYPE_INT, green_stype, &green_trace, trace_buf);
      gunread(bin->media->fp, trace_buf, trace_read);
   } else {
      green_trace = SEGY_TRACE_MIN + NumSmp * sample_prec;
   }

   /****************************************************/
   /* Given NumSmp, sample_prec, and green_trace,      */
   /* determine bin->cube.axis[1].count and trace_pad. */
   /****************************************************/

   if(green_trace >  SEGY_TRACE_MIN + NumSmp * sample_prec) {
      trace_pad = green_trace - (SEGY_TRACE_MIN + NumSmp * sample_prec);
      ddsWarn("segy trace green word implies %i byte pad\n\tbinary= %s\n",
         trace_pad, bin->name);
   } else if(green_trace < SEGY_TRACE_MIN + NumSmp * sample_prec) {
      NumSmp = (green_trace - SEGY_TRACE_MIN) / sample_prec;
      trace_pad = green_trace - (SEGY_TRACE_MIN + NumSmp * sample_prec);
      ddsError("segy trace green word implies NumSmp= %i, pad= %i bytes\n\tbinary = %s\n",
         NumSmp, trace_pad, bin->name);
   } else {
      trace_pad = 0;
   }

   /*****************************************************/
   /* Merge cube size information (needed by compiler). */
   /*****************************************************/

   cdds_dict(bin->dict_open, "scan");

   bin->cube.axis[1].size = NumSmp; /* exception */
   bin->cube.axis[1].flag |= DDS_AXIS_size;

   if(bin->cube.rank >= 2) {
      if(bin->cube.axis[2].flag & DDS_AXIS_size) {
         NumTrc = bin->cube.axis[2].size;
      } else {
         cdds_scanf("segy_NumTrc", "%d", &NumTrc);
         bin->cube.axis[2].size = NumTrc;
         bin->cube.axis[2].flag |= DDS_AXIS_size;
      }

      if(bin->cube.axis[2].size < 1) {
         ddsWarn("invalid segy_NumTrc= size.axis(2)= %li\n\t%s\n",
            (long)bin->cube.axis[2].size, media->title.ptr);
         bin->cube.axis[2].size = NumTrc = 1;
      }
   } else {
      NumTrc = 1;
      cdds_scanf("segy_NumTrc", "%d", &NumTrc);
      if(NumTrc < 1) NumTrc = 1;
   }

   /* possibly initialize NumRec by size of file */
   if (isReg && bin->cube.rank==3 && bin->cube.axis[3].size==1) {
      gseek(bin->media->fp, 0, SEEK_END);
      ggetpos(bin->media->fp, &size);
      gsetpos(bin->media->fp, &bin->media->trace_bias);

      size = HOST_INT6_DIV((size - bin->media->trace_bias),
         (green_trace + green_prefix_prec + green_suffix_prec));
      if (NumTrc > 0) {
         size = 1 + HOST_INT6_DIV(size-1,NumTrc);
      }
      bin->cube.axis[3].size = (long)size;
   }

   if(bin->cube.rank >= 3) {
      if(! (bin->cube.axis[3].flag & DDS_AXIS_size)) {
         cdds_scanf("segy_NumRec", "%ld", &bin->cube.axis[3].size);
         bin->cube.axis[3].flag |= DDS_AXIS_size;
      }

      if(bin->cube.axis[3].size < 1) {
         ddsWarn("invalid segy_NumRec= size.axis(3)= %li\n\t%s\n",
            (long)bin->cube.axis[3].size, media->title.ptr);
         bin->cube.axis[3].size = 1;
      }
   }

   /*************************************************/
   /* compile the description of the binary format. */
   /*************************************************/

   /* create explicit typedefs */
   ddsSymTypedefObj("GREEN_PREFIX",
      green_prefix_prec ? int_stype : DDS_CTYPE_VOID, 0);
   ddsSymTypedefObj("GREEN_SUFFIX",
      green_suffix_prec ? int_stype : DDS_CTYPE_VOID, 0);
   if (green_prefix_prec == 8) {
      ddsSymTypedefObj("SEGY_CHAR", char_stype, 8);
   } else {
      ddsSymTypedefObj("SEGY_CHAR", char_stype, 0);
   }
   ddsSymTypedefObj("SEGY_FmtCode", short_stype, 0);
   ddsSymTypedefObj("SEGY_TrcIdCode", short_stype, 0);
   ddsSymTypedefObj("SEGY_UnitsFlag", short_stype, 0);


   ddsSymTypedefObj("SEGY_USHORT", ushort_stype, 0);
   ddsSymTypedefObj("SEGY_SHORT", short_stype, 0);
   ddsSymTypedefObj("SEGY_INT", int_stype, 0);
   ddsSymTypedefObj("SEGY_FLOAT", float_stype, 0);
   ddsSymTypedefObj("SAMPLE_TYPE", samples_stype, 0);

   /* update SEGY_CARD_PAD, SEGY_BINARY_PAD, SEGY_TRACE_PAD */
   block = ddsSymTypedefEB(DDS_CTYPE_INT);
   ddsSymTypedefEV("SEGY_CARD_PAD", card_pad);
   ddsSymTypedefEV("SEGY_BINARY_PAD", binary_pad);
   ddsSymTypedefEV("SEGY_TRACE_PAD", trace_pad);
   ddsSymTypedefEE(block, "SEGY_ADJUST");

   if(ddsCompileFmt(bin, DDS_FALSE)) return;

   ddsUncompileFmt(bin);

   /*******************************************/
   /* link bin struct to card header fields */
   /*******************************************/

   if(ddsObjectTag(&bin->std.segy.card, bin->block_new, "PREAMBLE") ||
      bin->std.segy.card.type->ptype != DDS_STYPE_STRUCT ||
      ddsLinkObject(&bin->std.segy.card)) {
      ddsError("Bogus segy format card declaration\n\tcard= %s\n",
         bin->name);
      return;
   }

   type = bin->std.segy.card.type;

   ddsLinkMember(&bin->std.segy.card_prefix, type->block, "card_green_prefix");
   ddsLinkMember(&bin->std.segy.card_suffix, type->block, "card_green_suffix");

   /*******************************************/
   /* link bin struct to binary header fields */
   /*******************************************/

   if(ddsObjectTag(&bin->std.segy.binary, bin->block_new, "PREAMBLE2") ||
      bin->std.segy.binary.type->ptype != DDS_STYPE_STRUCT ||
      ddsLinkObject(&bin->std.segy.binary)) {
      ddsError("Bogus segy format binary declaration\n\tbinary= %s\n",
         bin->name);
      return;
   }

   type = bin->std.segy.binary.type;

   ddsLinkMember(&bin->std.segy.binary_prefix, type->block, "binary_green_prefix");
   ddsLinkMember(&bin->std.segy.binary_suffix, type->block, "binary_green_suffix");
   ddsLinkMember(&bin->std.segy.binary_NumSmp, type->block, "NumSmp");

   /*******************************************************/
   /* GET header values from dictionary or                */
   /*    line header binary buffer.                       */
   /*                                                     */
   /* If the dictionary defines a particular header,      */
   /* then its value is retrieved and saved in the        */
   /* corresponding dict_hdr and bin_hdr members.         */
   /*                                                     */
   /* If not defined, the dict_hdr member is defaulted    */
   /* (zero if numeric, blank if character).              */
   /* The bin_hdr value is taken from the binary header   */
   /* referenced by hdr_buf.                              */
   /*******************************************************/

   /* setup implicit arguments used by HDR_GET */
   cdds_dict(bin->dict_open, "scan");

   hdr_buf = card_buf;
   type = bin->std.segy.card.type;
   hdr_block = type->block;

   HDR_GET_CHAR(segy_, "", card3200)

   hdr_buf = binary_buf;
   type = bin->std.segy.binary.type;
   hdr_block = type->block;

   HDR_GET_INT(segy_, "", JobNum)
   HDR_GET_INT(segy_, "", LineNum)
   HDR_GET_INT(segy_, "", ReelNum)
   HDR_GET_INT(segy_, "", NumTrc)
   HDR_GET_INT(segy_, "", NumAux)
   HDR_GET_INT(segy_, "", SmpInt)
   HDR_GET_INT(segy_, "", OrigSmpInt)
   HDR_GET_INT(segy_, "", NumSmp)
   HDR_GET_INT(segy_, "", OrigNumSmp)
/***
   HDR_GET_INT(segy_, "", FmtCode)
***/
   HDR_GET_INT(segy_, "", CdpFold)
   HDR_GET_INT(segy_, "", SortCode)
   HDR_GET_INT(segy_, "", VertSumFlag)
   HDR_GET_INT(segy_, "", SwpFreqStart)
   HDR_GET_INT(segy_, "", SwpFreqEnd)
   HDR_GET_INT(segy_, "", SwpLength)
   HDR_GET_INT(segy_, "", SwpCode)
   HDR_GET_INT(segy_, "", SwpChan)
   HDR_GET_INT(segy_, "", SwpTprStart)
   HDR_GET_INT(segy_, "", SwpTprEnd)
   HDR_GET_INT(segy_, "", SwpTprType)
   HDR_GET_INT(segy_, "", CorrFlag)
   HDR_GET_INT(segy_, "", GainFlag)
   HDR_GET_INT(segy_, "", AmpFlag)
   HDR_GET_INT(segy_, "", UnitsFlag)
   HDR_GET_INT(segy_, "", PolarityFlag)
   HDR_GET_INT(segy_, "", VibLagFlag)
   HDR_GET_INT(segy_, "SpareBin.", SpareBinI2)
   HDR_GET_INT(segy_, "", SegyFmt)
   HDR_GET_INT(segy_, "", FixedLength)
   HDR_GET_INT(segy_, "", NumHdr)
   HDR_GET_INT(segy_, "SpareBin.", Spare2BinI2)

   /***************************************/
   /* force rational binary header values */
   /***************************************/

   bin_hdr.NumSmp = NumSmp;
   bin_hdr.FmtCode = FmtCode;
   if (SU) {
      bin_hdr.NumTrc = NumTrc;
      bin_hdr.SmpInt = SmpInt;
   }

   /*******************************************************/
   /* Merge line header and nominal cube description.     */
   /*                                                     */
   /* The dictionary has precedence over the binary       */
   /* line header, if the parameters are ambiguous.       */
   /* Within the dictionary, the cube description has     */
   /* precedence over any format specific definitions.    */
   /*    Example: "delta.t= M" has precedence over        */
   /*       "segy_SmpInt= N", if both are defined.        */
   /*******************************************************/

   cdds_dict(bin->dict_open, "scan");

   /* Discover rank of standard axis names, if they exist. */
   axis_t = axis_x = axis_y = axis_z = 0;

   for(rank = 1; rank <= bin->cube.rank; rank++) {
      axis = bin->cube.axis + rank;

      if(! strcmp(axis->name, "t")) {
         axis_t = rank;
      } else if(! strcmp(axis->name, "x")) {
         axis_x = rank;
      } else if(! strcmp(axis->name, "y")) {
         axis_y = rank;
      } else if(! strcmp(axis->name, "z")) {
         axis_z = rank;
      }
   }

   if(axis_t == 1) {
      /* "time" axis DOES exist. */
      segy_units = "usec";

      if(strcmp(bin->cube.axis[axis_t].units, "?")) {
         cube_units = bin->cube.axis[axis_t].units;
      } else {
         cube_units = segy_units;
         bin->cube.axis[axis_t].units = ddsString(segy_units);
      }

      sec_to_cube = cdds_scale(cube_units, "sec");

      if(! sec_to_cube) {
         ddsWarn("delta.t %s unknown, given segy Sample %s\n\t%s\n",
            cube_units, segy_units,
            media->title.ptr);
         sec_to_cube = 1.;
      }

      if(! (bin->cube.axis[axis_t].flag & DDS_AXIS_delta)) {
         /* Derive cube parameter from binary header. */
         bin->cube.axis[axis_t].delta = 1.e-6 * bin_hdr.SmpInt *
            sec_to_cube;
         bin->cube.axis[axis_t].flag |= DDS_AXIS_delta;
      }
   } else if(! (bin->cube.axis[1].flag & DDS_AXIS_delta)) {
      /* Derive cube parameter from binary header. */
      bin->cube.axis[1].flag |= DDS_AXIS_delta;
      if (bin_hdr.SmpInt < 1000) {
         bin->cube.axis[1].delta = bin_hdr.SmpInt;
      } else {
         bin->cube.axis[1].delta = bin_hdr.SmpInt/1000.0;
      }
   }

   if(bin_hdr.UnitsFlag == SEGY_UNITSFLAG_FEET) {
      segy_units = "feet";
   } else if(bin_hdr.UnitsFlag == SEGY_UNITSFLAG_METERS) {
      segy_units = "meters";
   } else {
      segy_units = NULL;
   }

   if(segy_units) {
      if(axis_x && ! strcmp(bin->cube.axis[axis_x].units, "?")) {
         bin->cube.axis[axis_x].units = ddsString(segy_units);
      }
      if(axis_y && ! strcmp(bin->cube.axis[axis_y].units, "?")) {
         bin->cube.axis[axis_y].units = ddsString(segy_units);
      }
      if(axis_z && ! strcmp(bin->cube.axis[axis_z].units, "?")) {
         bin->cube.axis[axis_z].units = ddsString(segy_units);
      }
   }

   /***********************************************************/
   /* MODify header value within dictionary.                  */
   /*                                                         */
   /* If the values in dict_hdr and bin_hdr are different,    */
   /* then the dictionary is updated using the bin_hdr value. */
   /* If the values are the same, nothing is changed.         */
   /* This brings the dictionary up to date with the current  */
   /* values in the binary buffer.                            */
   /***********************************************************/

   /* setup implicit arguments used by HDR_MOD */
   cdds_dict(bin->dict_open, "print");

   HDR_MOD_CHAR(segy_, card3200, 40)

   HDR_MOD_INT(segy_, JobNum)
   HDR_MOD_INT(segy_, LineNum)
   HDR_MOD_INT(segy_, ReelNum)
   HDR_MOD_INT(segy_, NumTrc)
   HDR_MOD_INT(segy_, NumAux)
   HDR_MOD_INT(segy_, SmpInt)
   HDR_MOD_INT(segy_, OrigSmpInt)
   HDR_MOD_INT(segy_, NumSmp)
   HDR_MOD_INT(segy_, OrigNumSmp)
   HDR_MOD_INT(segy_, FmtCode)
   HDR_MOD_INT(segy_, CdpFold)
   HDR_MOD_INT(segy_, SortCode)
   HDR_MOD_INT(segy_, VertSumFlag)
   HDR_MOD_INT(segy_, SwpFreqStart)
   HDR_MOD_INT(segy_, SwpFreqEnd)
   HDR_MOD_INT(segy_, SwpLength)
   HDR_MOD_INT(segy_, SwpCode)
   HDR_MOD_INT(segy_, SwpChan)
   HDR_MOD_INT(segy_, SwpTprStart)
   HDR_MOD_INT(segy_, SwpTprEnd)
   HDR_MOD_INT(segy_, SwpTprType)
   HDR_MOD_INT(segy_, CorrFlag)
   HDR_MOD_INT(segy_, GainFlag)
   HDR_MOD_INT(segy_, AmpFlag)
   HDR_MOD_INT(segy_, UnitsFlag)
   HDR_MOD_INT(segy_, PolarityFlag)
   HDR_MOD_INT(segy_, VibLagFlag)
   HDR_MOD_INT(segy_, SpareBinI2)
   HDR_MOD_INT(segy_, SegyFmt)
   HDR_MOD_INT(segy_, FixedLength)
   HDR_MOD_INT(segy_, NumHdr)
   HDR_MOD_INT(segy_, Spare2BinI2)

   /******************************************************/
   /* put any non-blank header cards to previous_history */
   /* definitions in dictionary if not already there.    */
   /******************************************************/

   cdds_dict(bin->dict_open, "scan");
   if (!SU && cdds_scanf("previous_history", "") < 0) {
      cdds_dict(bin->dict_open, "print");
      str = (char*)malloc(81);
      buf = (char*)malloc(161);
      for(k=0;k<40;k++) {
         strncpy(str, bin_hdr.card3200[k], 80);
         end = str + 79;
         while (end >= str && isspace(*end)) end--;
         if (end >= str) {
            end[1] = '\0';
            /* build string replacing unprintable characters, escaping internal quotes & "=" */
            n = strlen(str);
            for(i=j=0;i<n;i++,j++) {
               if (!isprint(0377 & str[i]) && 
                   !isspace(0377 & str[i])) {
                  buf[j] = '~';
               } else if (str[i] == '"') {
                  buf[j] = '\''; 
               } else if (str[i] == '\\') {
                  buf[j] = '\\'; 
                  buf[++j] = str[++i]; 
               } else if (str[i] == '=') {
                  buf[j] = '\\'; 
                  buf[++j] = '='; 
               } else {
                  buf[j] = str[i];
               }
            }
            buf[j] = '\0';
            cdds_printf("previous_history", "\"%s\"\n", buf);
         }
      }
      free(str);
      free(buf);
   }

   /********************************************/
   /* find trace object and create its API tag */
   /********************************************/

   if(ddsTraceTag(bin)) return;

   /*********************************************************/
   /* Update binary cube description.                       */
   /*                                                       */
   /* Cube print must follow HDR_MOD_* in order to maintain */
   /* maximum precision for space and time coordinates,     */
   /* i.e. sample rates and origins.                        */
   /* This is a problem for usp and segy.                   */
   /*********************************************************/

   ddsCubePrint(bin, old_cube);

   if(bin->trace.type->ptype != DDS_STYPE_STRUCT) {
      ddsError("Bogus segy format trace declaration\n\tbinary= %s\n",
         bin->name);
      return;
   }

   /*************************/
   /* link cube description */
   /*************************/

   ddsCubeLinker(bin);

   type = bin->trace.type;

   ddsLinkMember(&bin->prefix,  type->block, "green_prefix");
   ddsLinkMember(&bin->suffix,  type->block, "green_suffix");
   ddsLinkMember(&bin->TrcType, type->block, "TrcIdCode");
   ddsLinkMember(&bin->Samples, type->block, "Samples");
   ddsLinkMember(&bin->std.segy.TrcNumSmp,  type->block, "TrcNumSmp");
   ddsLinkMember(&bin->std.segy.TrcSmpInt,  type->block, "TrcSmpInt");

   /****************/
   /* sanity check */
   /****************/

   type = bin->std.segy.card.type;
   green_test = type->stroke - green_prefix_prec - green_suffix_prec;
   if(!SU && green_card != green_test) {
      ddsError("SEGY card green word= %i, expected %i\n\tbinary= %s\n",
         (int)green_card, (int)green_test, bin->name);
   }

   type = bin->std.segy.binary.type;
   green_test = type->stroke - green_prefix_prec - green_suffix_prec;
   if(!SU && green_binary != green_test) {
      ddsError("SEGY binary green word= %i, expected %i\n\tbinary= %s\n",
         (int)green_binary, (int)green_test, bin->name);
   }

   type = bin->trace.type;
   green_test = type->stroke - green_prefix_prec - green_suffix_prec;
   member = bin->Samples.member;
   if(member->offset != SEGY_TRACE_MIN + green_prefix_prec) {
      ddsError("SEGY trace Samples offset= %lu, expected %i\n\tbinary= %s\n",
         (unsigned long)member->offset,
         (int)(SEGY_TRACE_MIN + green_prefix_prec), bin->name);
   } else if(green_trace != green_test) {
      ddsError("SEGY trace green word= %i, expected %i\n\tbinary= %s\n",
         (int)green_trace, (int)green_test, bin->name);
   }

   /********************************************************************/
   /* allocate buffer for one card & binary header and initialize them */
   /********************************************************************/
 
   type = bin->std.segy.card.type;
   bin->std.segy.card_buf = ddsRealloc(card_buf, type->stroke);
   bin->std.segy.card_green_min = SEGY_CARD_MIN;
   bin->std.segy.card_green_max = SEGY_CARD_MAX;

   if((dds_state & DDS_DUMP_DATA) && (dds_state & DDS_DUMP_READ)) {
      ddsDumpDataBuf(bin, "read", (DDS_SYM*)type, card_buf);
   }
 
   type = bin->std.segy.binary.type;
   bin->std.segy.binary_buf = ddsRealloc(binary_buf, type->stroke);
   bin->std.segy.binary_green_min = SEGY_BINARY_MIN;
   bin->std.segy.binary_green_max = SEGY_BINARY_MAX;

   if((dds_state & DDS_DUMP_DATA) && (dds_state & DDS_DUMP_READ)) {
      ddsDumpDataBuf(bin, "read", (DDS_SYM*)type, binary_buf);
   }

   /*********************************************************/
   /* allocate buffer for one dead trace, and initialize it */
   /*********************************************************/

   type = bin->trace.type;
   bin->trace_buf = ddsMalloc(type->stroke);

   if(dds_state & DDS_DUMP_DIFF) {
      bin->diff1_buf = ddsMalloc(type->stroke);
      memset(bin->diff1_buf, DDS_NON_CHAR, type->stroke);
      bin->diff2_buf = ddsMalloc(type->stroke);
      memset(bin->diff2_buf, DDS_NON_CHAR, type->stroke);
   }

   bin->dead_trace = ddsZalloc(type->stroke);

   /**************************************/
   /* restore the su format from here on */
   /**************************************/

   if (SU) {
      bin->fmt = DDS_FMT_SU;
      bin->fmt_name = ddsString("su");
   }

   return;
}
