static char rcsid[]="$Id: openoldusp.c 211 2011-01-11 14:32:42Z user $";
/*
RCS-t=DDS, open old binary data, usp format

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"
#include "usp_hdr.h"

void ddsOpenOldUsp(
   DDS_BIN *bin,
   DDS_CUBE *old_cube)
{
   static DDS_STRING tmp_str = NULL_STRING;
   const char *FmtCod_ptr;
   char sample_style[8];
   USP_LINE dict_hdr, bin_hdr;
   DDS_TYPE *sample_type;
   DDS_PTYPE char_stype, short_stype, int_stype, float_stype;
   DDS_PTYPE samples_stype;
   DDS_BOOLEAN ignore_FmtCod, hlh_cube;
   DDS_TYPE *type;
   DDS_MEMBER *member;
   DDS_BLOCK *block, *hdr_block;
   DDS_MEDIA *media = bin->media;
   DDS_AXIS *axis;
   const char *hdr_buf, *line_buf, *trace_buf;
   int swap;
   int i, j, n, i_value;
   int rank, axis_t, axis_x, axis_y, axis_z;
   const char *usp_units, *cube_units;
   double usp_to_cube, sec_to_cube;
   float FakeFloat;
   int usp_line_min, usp_line_max, usp_trace_min;
   int line_read, overload, compressed;
   int green_test4, green_test8;
   int green_prec, green_line, green_trace;
   int NumSmp, NumTrc, NumRec;
   float UnitSc;
   int FmtCod, HlhEnt, HlhByt;
   int HlhByt_prec, sample_prec;
   int hlh_size, line_pad, trace_pad;
   size_t line_max;
   unsigned int hlh_ptr, len;
   char *hlh, *hlh_str;

   /***********************************/
   /* retrieve read/write trace flags */
   /***********************************/

   cdds_dict(bin->dict_open, "scan");

   /* current media offset is the start of the PREAMBLE (line header). */
   ggetpos(media->fp, &media->data_bias);

   line_max = USP_LINE_IEEE_MAX + USP_GREEN_WORD_MAX;
   line_read = greada(media->fp, (const void**)&line_buf,
      line_max, GIO_CHAR);

   if(line_read < USP_LINE_IEEE_MIN) {
      ddsError("usp line header read size= %i, minimum= %i\n",
         line_read, (int)USP_LINE_IEEE_MIN);
      return;
   }

   /************************************************************/
   /* Given a "format=" and line header, determine green_prec. */
   /* If green_prec is non-zero, save green_line.              */
   /* These are required to extract other line header values.  */
   /************************************************************/

   /* Determine if FIFO on a byte swapped host */
#ifdef HOST_SWAP
   swap = (bin->media->genus == DDS_MEDIA_FIFO);
#else
   swap = 0;
#endif

   /* Extract green words from line header. */
   if (swap) { 
      CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED4X, &green_test4, line_buf);
      green_test8 = 0;
   } else {
      CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED4, &green_test4, line_buf);
      CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED8, &green_test8, line_buf);
   }

   if(0 < cdds_scank("format", "green_prefix")) {
      /* green word prefix is *required* */
      bin->state  |= DDS_green_prefix;
      green_prec = 0;
      cdds_scanf("", "%*s %d", &green_prec);
      if(! green_prec) {
         /* guess the green word precision */
         if(green_test4 >= USP_LINE_IEEE_MIN &&
            green_test4 <= USP_LINE_IEEE_MAX) {
            green_prec = 4;
            green_line = green_test4;
         } else {
            green_prec = 8;
            green_line = green_test8;
         }
      } else if(green_prec == 8) {
         green_line = green_test8;
      } else {
         green_line = green_test4;
      }
   } else if(0 < cdds_scank("format", "no_green_prefix")) {
      /* green word prefix is *not* allowed */
      green_prec = 0;
   } else if(green_test4 >= USP_LINE_IEEE_MIN &&
             green_test4 <= USP_LINE_IEEE_MAX) {
      /* automatic green word detection */
      bin->state  |= DDS_green_prefix;
      green_prec = 4;
      green_line = green_test4;
   } else {
      green_prec = 0;
   }

   /******************************************************/
   /* Extract "cube" description from line header, i.e.  */
   /* NumSmp, NumTrc, NumRec, FmtCod, HlhEnt, HlhByt.    */
   /******************************************************/

   if(swap) {
      CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED4X,
         &NumSmp, line_buf + green_prec +  60);
      CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED4X,
         &NumTrc, line_buf + green_prec +  48);
      CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED4X,
         &NumRec, line_buf + green_prec +  52);

      CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED2X,
         &FmtCod, line_buf + green_prec +  64);
      CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED2X,
         &HlhEnt, line_buf + green_prec + 1000);
      CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED2X,
         &HlhByt, line_buf + green_prec + 1002);

      FmtCod_ptr = line_buf + green_prec + 64;
      usp_line_min = USP_LINE_IEEE_MIN;
      usp_line_max = USP_LINE_IEEE_MAX;
      usp_trace_min = USP_TRACE_IEEE_MIN;
      HlhByt_prec = 2;
      short_stype = DDS_STYPE_INTEGER2X;
      int_stype = DDS_STYPE_INTEGER4X;
      if(0 < cdds_scank("format", "ibm")) {
         char_stype = DDS_STYPE_EBCDIC1;
         float_stype = DDS_STYPE_FLOAT4IBM;
         samples_stype = DDS_STYPE_FLOAT4IBM;
      } else {
         char_stype = DDS_STYPE_ASCII1;
         float_stype = DDS_STYPE_FLOAT4X;
         samples_stype = DDS_STYPE_FLOAT4X;
      }
   } else {
      CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED4,
         &NumSmp, line_buf + green_prec +  60);
      CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED4,
         &NumTrc, line_buf + green_prec +  48);
      CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED4,
         &NumRec, line_buf + green_prec +  52);

      CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED2,
         &FmtCod, line_buf + green_prec +  64);
      CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED2,
         &HlhEnt, line_buf + green_prec + 1000);
      CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED2,
         &HlhByt, line_buf + green_prec + 1002);

      FmtCod_ptr = line_buf + green_prec + 64;
      usp_line_min = USP_LINE_IEEE_MIN;
      usp_line_max = USP_LINE_IEEE_MAX;
      usp_trace_min = USP_TRACE_IEEE_MIN;
      HlhByt_prec = 2;
      short_stype = DDS_STYPE_INTEGER2;
      int_stype = DDS_STYPE_INTEGER4;
      if(0 < cdds_scank("format", "ibm")) {
         char_stype = DDS_STYPE_EBCDIC1;
         float_stype = DDS_STYPE_FLOAT4IBM;
         samples_stype = DDS_STYPE_FLOAT4IBM;
      } else {
         char_stype = DDS_STYPE_ASCII1;
         float_stype = DDS_STYPE_FLOAT4;
         samples_stype = DDS_STYPE_FLOAT4;
      }
   }

   /*********************************************/
   /* Validate usp line header essential values */
   /*********************************************/

   if(line_read < usp_line_min + green_prec) {
      ddsError("short read on USP line header\n\tbinary= %s\n",
         bin->name);
      return;
   } else if(*FmtCod_ptr == DDS_ASCII_BLANK  || /* segy ? */
      *FmtCod_ptr == DDS_EBCDIC_BLANK || /* segy ? */
      (! green_prec && (FmtCod <= 0 || FmtCod > 9 ||
         NumTrc <= 0 || NumTrc > 25000 || NumSmp <= 0 || NumSmp > 8000))) {
      ddsError("%s\n\tbinary= %s\n\tFmtCod=%i %i green_prec=%d NumTrc=%d NumSmp=%d\n",
         "bogus usp line header (wrong format?)", bin->name,
         (int)(*FmtCod_ptr), FmtCod, green_prec, NumTrc, NumSmp);
      return;
   } else if(NumSmp < 0) {
      ddsWarn("'NumSmp' line header value is invalid (%i)\n\tbinary= %s\n",
         NumSmp, bin->name);
      NumSmp = 0;
   }

   if(bin->state & DDS_green_prefix) {
      if(green_line < usp_line_min) {
         ddsError("USP line header green word= %i, minimum= %i\n\tbinary= %s\n",
            green_line, usp_line_min, bin->name);
         return;
      } else if(HlhByt < 2 * HlhByt_prec) {
         ddsWarn("USP line header HlhByt= %i, minimum= %i\n\tbinary= %s\n",
            HlhByt, 2 * HlhByt_prec, bin->name);
         HlhByt = 2 * HlhByt_prec;
      }
   } else {
      green_line = usp_line_min + HlhByt - 2 * HlhByt_prec;
   }

   /***********************************************/
   /* given HlhByt_prec, HlhByt and green_line... */
   /* determine HlhByt, hlh_size, line_pad. */
   /***********************************************/

   if(green_line > usp_line_min + HlhByt - 2 * HlhByt_prec) {
      hlh_size = HlhByt - 2 * HlhByt_prec;
      line_pad = green_line - (usp_line_min + hlh_size);
      ddsWarn("usp line header green word implies %i byte pad\n\tbinary= %s\n",
         line_pad, bin->name);
   } else if(green_line < usp_line_min + HlhByt - 2 * HlhByt_prec) {
      line_pad = 0;
      hlh_size = green_line - usp_line_min;
      HlhByt = green_line - usp_line_min + 2 * HlhByt_prec;
      ddsError("usp line header green word implies HlhByt = %i\n\tbinary= %s\n",
         HlhByt, bin->name);
   } else {
      hlh_size = HlhByt - 2 * HlhByt_prec;
      line_pad = 0;
   }

   cdds_dict(bin->dict_open, "scan");

   if(! cdds_scank("format", "ignore_hlh")) {
      /********************************************************/
      /* use HLH dict: information to revise cube description */
      /********************************************************/
      if(cdds_scank("format", "hlh_cube")) {
         hlh_cube = DDS_TRUE;
      } else {
         hlh_cube = DDS_FALSE;
      }

      /* save any new dict: info to open dictionary */
      cdds_dict(bin->dict_open, "scan");
      hlh_ptr = green_prec + usp_line_min;
      while(hlh_ptr < green_line + green_prec) {
         CON_ANSS(DDS_CTYPE_UINT, short_stype, &len, line_buf + hlh_ptr);
         if(len > 1024) break;
         hlh_ptr += HlhByt_prec;
         ddsStrCpyN(&tmp_str, line_buf + hlh_ptr, len);
         hlh_ptr += len;
         if((HlhByt_prec == 8) && (len % HlhByt_prec)) {
            hlh_ptr += HlhByt_prec - len % HlhByt_prec;
         }

         if(hlh_cube && ! strncmp(tmp_str.ptr, "dict:", 5)) {
            hlh_str = strchr(tmp_str.ptr, '=');
            if(hlh_str) {
               *hlh_str = '\0';
               hlh_str++;
               if (cdds_scanf(tmp_str.ptr + 5, "") < 0) {
                  cdds_dict(bin->dict_open, "print");
                  cdds_printf(tmp_str.ptr + 5, " %s\n", hlh_str);
                  cdds_dict(bin->dict_open, "scan");
               }
            }
         } else if(hlh_cube && ! strncmp(tmp_str.ptr, " dict:", 6)) {
            hlh_str = strchr(tmp_str.ptr, '=');
            if(hlh_str) {
               *hlh_str = '\0';
               hlh_str++;
               if (cdds_scanf(tmp_str.ptr + 6, "") < 0) {
                  cdds_dict(bin->dict_open, "print");
                  cdds_printf(tmp_str.ptr + 6, " %s\n", hlh_str);
                  cdds_dict(bin->dict_open, "scan");
               }
            }
         }
      }
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

   /*********************************************************/
   /* Format definition keywords may override samples_stype */
   /*********************************************************/

   ignore_FmtCod = DDS_FALSE;
   sample_prec = 0;
   if(0 < cdds_scank("format", "integer")) {
      /* integer sample type */
      cdds_scanf("", "%*s %d", &sample_prec);
      if(sample_prec == 1) {
         ignore_FmtCod = DDS_TRUE;
         samples_stype = DDS_STYPE_INTEGER1;
      } else if(sample_prec == 2) {
         if(FmtCod != 1) ignore_FmtCod = DDS_TRUE;
         samples_stype = DDS_STYPE_INTEGER2;
      } else if(sample_prec == 4) {
         if(FmtCod != 2) ignore_FmtCod = DDS_TRUE;
         samples_stype = DDS_STYPE_INTEGER4;
      } else if(sample_prec == 8) {
         ignore_FmtCod = DDS_TRUE;
         samples_stype = DDS_STYPE_INTEGER8;
      } else if(FmtCod == 1) {
         samples_stype = DDS_STYPE_INTEGER2;
      } else if(FmtCod == 2) {
         samples_stype = DDS_STYPE_INTEGER4;
      } else {
         ignore_FmtCod = DDS_TRUE;
         FmtCod = 1;
         samples_stype = DDS_STYPE_INTEGER2;
      }
   } else if(0 < cdds_scank("format", "float")) {
      /* float sample type */
      *sample_style = '\0';
      cdds_scanf("", "%*s %d %7s", &sample_prec, sample_style);
      if(FmtCod != 3) ignore_FmtCod = DDS_TRUE;
      if(sample_prec == 4 && ! strcmp(sample_style, "ieee")) {
         samples_stype = DDS_STYPE_FLOAT4;
      } else if(sample_prec == 4 && ! strcmp(sample_style, "ibm")) {
         samples_stype = DDS_STYPE_FLOAT4IBM;
      }
   } else if(FmtCod == 1) {
      samples_stype = DDS_STYPE_INTEGER2;
   } else if(FmtCod == 2) {
      samples_stype = DDS_STYPE_INTEGER4;
   } else if(FmtCod != 3) {
      ignore_FmtCod = DDS_TRUE;
   }

   /* check if complex */
   if(0 < cdds_scank("format", "complex")) {
      if(samples_stype == DDS_STYPE_FLOAT4) {
         samples_stype = DDS_STYPE_COMPLEX8;
         NumSmp = NumSmp / 2;
      } else if(samples_stype == DDS_STYPE_FLOAT4IBM) {
         samples_stype = DDS_STYPE_COMPLEX8IBM;
         NumSmp = NumSmp / 2;
      }
   }

   sample_type = dds_prime[samples_stype].type;
   sample_prec = sample_type->prec;

   if(ignore_FmtCod) {
      ddsWarn("format definition overrides FmtCod= %i\n\tbinary= %s\n",
         FmtCod, bin->name);
   }

   /*********************************************/
   /* Compute and validate size of trace buffer */
   /*********************************************/

   trace_buf = line_buf + green_line + green_prec;

   if (0 < cdds_scank("format", "compressed")) {
      green_trace = usp_trace_min + NumSmp * sample_prec;
   } else if(green_prec && line_read > green_line + 2 * green_prec) {
      if(green_prec == 8) {
         CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED8, &green_trace, trace_buf);
      } else if(swap) {
         CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED4X, &green_trace, trace_buf);
      } else {
         CON_ANSS(DDS_CTYPE_INT, DDS_STYPE_UNSIGNED4, &green_trace, trace_buf);
      }
   } else {
      green_trace = usp_trace_min + NumSmp * sample_prec;
   }

   /****************************************************/
   /* Given NumSmp, sample_prec, and green_trace,      */
   /* determine bin->cube.axis[1].count and trace_pad. */
   /****************************************************/

   if(green_trace >  usp_trace_min + NumSmp * sample_prec) {
      trace_pad = green_trace -
         (usp_trace_min + NumSmp * sample_prec);
      ddsWarn("usp trace green word implies %i byte pad\n\tbinary= %s\n",
         trace_pad, bin->name);
   } else if(green_trace <  usp_trace_min + NumSmp * sample_prec) {
      NumSmp = (green_trace - usp_trace_min) / sample_prec;
      trace_pad = green_trace - (usp_trace_min + NumSmp * sample_prec);
      ddsError("usp trace green word implies NumSmp = %i, pad= %i bytes\n\tbinary = %s\n",
         bin->cube.axis[1].size, trace_pad, bin->name);
   } else {
      bin->cube.axis[1].size = NumSmp;
      trace_pad = 0;
   }

   /********************************************/
   /* unread everything except the line header */
   /********************************************/

   overload = line_read - (green_line + green_prec);

   if(overload > 0) {
      /* push back everything except the usp line header */
      gunread(media->fp, trace_buf, overload);
   } else if(overload < 0) {
      ddsError("short USP line header, overload= %i\n\tbinary= %s\n",
         overload, bin->name);
   }

   /* current media offset is the start of the SEQUENCE (traces). */
   ggetpos(media->fp, &media->trace_bias);

   /*****************************************************/
   /* Merge cube size information (needed by compiler). */
   /*****************************************************/

   cdds_dict(bin->dict_open, "scan");

   bin->cube.axis[1].size = NumSmp; /* exception */
   bin->cube.axis[1].flag |= DDS_AXIS_size;

   if(bin->cube.rank >= 2) {
      if(! (bin->cube.axis[2].flag & DDS_AXIS_size)) {
         cdds_scanf("usp_NumTrc", "%d", &NumTrc);
         bin->cube.axis[2].size = NumTrc;
         bin->cube.axis[2].flag |= DDS_AXIS_size;
      }

      if(bin->cube.axis[2].size < 1) {
         ddsWarn("invalid usp_NumTrc= size.axis(2)= %li\n\t%s\n",
            (long)bin->cube.axis[2].size, media->title.ptr);
         bin->cube.axis[2].size = NumTrc = 1;
      }
   } else {
      NumTrc = 1;
      cdds_scanf("usp_NumTrc", "%d", &NumTrc);
      if(NumTrc < 1) NumTrc = 1;
   }

   if(bin->cube.rank >= 3) {
      if(! (bin->cube.axis[3].flag & DDS_AXIS_size)) {
         cdds_scanf("usp_NumRec", "%d", &NumRec);
         bin->cube.axis[3].size = NumRec;
         bin->cube.axis[3].flag |= DDS_AXIS_size;
      }

      if(bin->cube.axis[3].size < 1) {
         ddsWarn("invalid usp_NumRec= size.axis(3)= %li\n\t%s\n",
            (long)bin->cube.axis[3].size, media->title.ptr);
         bin->cube.axis[3].size = NumRec = 1;
      }
   } else {
      NumRec = 1;
      cdds_scanf("usp_NumRec", "%d", &NumRec);
      if(NumRec < 1) NumRec = 1;
   }

   /*************************************************/
   /* compile the description of the binary format. */
   /*************************************************/

   /* create explicit typedefs */
   ddsSymTypedefObj("GREEN_PREFIX", green_prec ? int_stype : DDS_CTYPE_VOID, 0);
   ddsSymTypedefObj("USP_CHAR", char_stype, 0);
   ddsSymTypedefObj("USP_SHORT", short_stype, 0);
   ddsSymTypedefObj("USP_INT", int_stype, 0);
   ddsSymTypedefObj("USP_FLOAT", float_stype, 0);
   ddsSymTypedefObj("SAMPLE_TYPE", samples_stype, 0);

   /* update USP_HLH_SIZE, USP_LINE_PAD, USP_TRACE_PAD */
   block = ddsSymTypedefEB(DDS_CTYPE_INT);
   ddsSymTypedefEV("USP_LINE_PAD", line_pad);
   ddsSymTypedefEV("USP_TRACE_PAD", trace_pad);
   ddsSymTypedefEV("USP_HLH_SIZE", hlh_size);
   ddsSymTypedefEE(block, "USP_ADJUST");

   if(ddsCompileFmt(bin, DDS_FALSE)) return;

   ddsUncompileFmt(bin);

   /*****************************************/
   /* link bin struct to line header fields */
   /*****************************************/

   if(ddsObjectTag(&bin->std.usp.line, bin->block_new, "PREAMBLE") ||
      bin->std.usp.line.type->ptype != DDS_STYPE_STRUCT ||
      ddsLinkObject(&bin->std.usp.line)) {
      ddsError("Bogus usp format line declaration\n\tbinary= %s\n",
         bin->name);
      return;
   }

   type = bin->std.usp.line.type;

   ddsLinkMember(&bin->std.usp.line_prefix, type->block, "line_green_prefix");
   ddsLinkMember(&bin->std.usp.line_HlhEnt, type->block, "HlhEnt");
   ddsLinkMember(&bin->std.usp.line_HlhByt, type->block, "HlhByt");
   ddsLinkMember(&bin->std.usp.line_HlhBuf, type->block, "HlhBuf");

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
   hdr_buf = line_buf;
   type = bin->std.usp.line.type;
   hdr_block = type->block;

   HDR_GET_CHAR(usp_, "", EqpCod)
   HDR_GET_CHAR(usp_, "", CrwNam)
   HDR_GET_CHAR(usp_, "", DatTyp)
   HDR_GET_CHAR(usp_, "", DgTrkS)
   HDR_GET_CHAR(usp_, "", PrcNam)
   HDR_GET_CHAR(usp_, "", PrcDat)
   HDR_GET_CHAR(usp_, "", OACLin)
   HDR_GET_CHAR(usp_, "", JobNum)
   HDR_GET_INT(usp_, "", NumTrc)
   HDR_GET_INT(usp_, "", NumRec)
   HDR_GET_INT(usp_, "", SmpInt)
   HDR_GET_INT(usp_, "", NumSmp)
   HDR_GET_INT(usp_, "", Format)
   HDR_GET_INT(usp_, "", IndAdj)
   HDR_GET_INT(usp_, "", OpGrFl)
   HDR_GET_CHAR(usp_, "", SrtTyp)
   HDR_GET_INT(usp_, "", CDPFld)
   HDR_GET_CHAR(usp_, "", GrpInt)
   HDR_GET_INT(usp_, "_DtInFl.", DtInFl)

   if(1 == cdds_scanf("usp_DtInFl usp_Dy1000", "%d", &i_value) &&
      strcmp("usp_DtInFl", dds_scan_name)) {
      dict_hdr.DtInFl = i_value;
      bin_hdr.DtInFl = i_value;
   }
   HDR_GET_INT(usp_, "", MxSPEl)
   HDR_GET_INT(usp_, "", MnSPEl)
   HDR_GET_INT(usp_, "", MxRSEL)
   HDR_GET_INT(usp_, "", MnRSEL)
   HDR_GET_INT(usp_, "", MxGrEl)
   HDR_GET_INT(usp_, "", MnGrEl)
   HDR_GET_INT(usp_, "", MxTrSt)
   HDR_GET_INT(usp_, "", MnTrSt)
   HDR_GET_CHAR(usp_, "", LinDir)
   HDR_GET_INT(usp_, "", AERcPr)
   HDR_GET_INT(usp_, "", OrNTRC)
   HDR_GET_INT(usp_, "", OrNREC)
   HDR_GET_FLOAT(usp_, "", MutVel)
   HDR_GET_INT(usp_, "", ZDMuTm)
   HDR_GET_INT(usp_, "", IntInc)
   HDR_GET_INT(usp_, "_WatVel.", WatVel)

   if(1 == cdds_scanf("usp_WatVel usp_RefVel", "%d", &i_value) &&
      strcmp("usp_WatVel", dds_scan_name)) {
      dict_hdr.WatVel = i_value;
      bin_hdr.WatVel = i_value;
   }
   HDR_GET_INT(usp_, "", MxShDp)
   HDR_GET_INT(usp_, "", MnShDp)
   HDR_GET_INT(usp_, "", MxUHTm)
   HDR_GET_INT(usp_, "", MnUHTm)
   HDR_GET_INT(usp_, "", MxTrOf)
   HDR_GET_INT(usp_, "", MnTrOf)
   HDR_GET_INT(usp_, "_RATTrc.", RATTrc)

   if(1 == cdds_scanf("usp_RATTrc usp_AnglSt", "%d", &i_value) &&
      strcmp("usp_RATTrc", dds_scan_name)) {
      dict_hdr.RATTrc = i_value;
      bin_hdr.RATTrc = i_value;
   }
   HDR_GET_INT(usp_, "_RATFld.", RATFld)

   if(1 == cdds_scanf("usp_RATFld usp_AngInc", "%d", &i_value) &&
      strcmp("usp_RATFld", dds_scan_name)) {
      dict_hdr.RATFld = i_value;
      bin_hdr.RATFld = i_value;
   }
   HDR_GET_INT(usp_, "", UnitFl)
   HDR_GET_INT(usp_, "", FrstSP)
   HDR_GET_INT(usp_, "", DpN1SP)
   HDR_GET_INT(usp_, "", NmDpIn)
   HDR_GET_INT(usp_, "", StWdFl)
   HDR_GET_INT(usp_, "", DptInt)
   HDR_GET_INT(usp_, "_TmMsSl.", TmMsSl)

   if(1 == cdds_scanf("usp_TmMsSl usp_Dx1000", "%d", &i_value) &&
      strcmp("usp_TmMsSl", dds_scan_name)) {
      dict_hdr.TmMsSl = i_value;
      bin_hdr.TmMsSl = i_value;
   }
   HDR_GET_INT(usp_, "_TmSlIn.", TmSlIn)

   if(1 == cdds_scanf("usp_TmSlIn usp_Dz1000", "%d", &i_value) &&
      strcmp("usp_TmSlIn", dds_scan_name)) {
      dict_hdr.TmSlIn = i_value;
      bin_hdr.TmSlIn = i_value;
   }
   HDR_GET_FLOAT(usp_, "", TmMsFS)
   HDR_GET_INT(usp_, "", NTrLnS)
   HDR_GET_INT(usp_, "", PltDir)
   HDR_GET_INT(usp_, "", LgRfEl)
   HDR_GET_INT(usp_, "", MnLnIn)
   HDR_GET_INT(usp_, "", MxLnIn)
   HDR_GET_INT(usp_, "", MnDpIn)
   HDR_GET_INT(usp_, "", MxDpIn)
   HDR_GET_FLOAT(usp_, "", ILClIn)
   HDR_GET_FLOAT(usp_, "", CLClIn)
   HDR_GET_INT(usp_, "", ReSpFm)
   HDR_GET_INT(usp_, "", VlFnRF)
   HDR_GET_CHAR(usp_, "", OACUsr)
   HDR_GET_CHAR(usp_, "", LinNam)
   HDR_GET_INT(usp_, "", MutFlg)
   HDR_GET_CHAR(usp_, "", SPBias)
   HDR_GET_INT(usp_, "", SpBiFl)
   HDR_GET_CHAR(usp_, "", APIWNo)
   HDR_GET_FLOAT(usp_, "", NmSpMi)

   HDR_GET_INT(usp_, "_VelFun.", VelFun)

   /*******************************************************/
   /* Scan the dictionary for these union values, but     */
   /* ONLY AFTER VelFun is retrieved and ONLY if it comes */
   /* AFTER the VelFin definition in the dictionary.      */
   /*******************************************************/

   if(1 == cdds_scanf("usp_VelFun usp_MinVel", "%d", &i_value) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[0] = i_value;
      dict_hdr.VelFun[0] = i_value;
   }
   if(1 == cdds_scanf("usp_VelFun usp_MaxVel", "%d", &i_value) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[1] = i_value;
      dict_hdr.VelFun[1] = i_value;
   }
   if(1 == cdds_scanf("usp_VelFun usp_Nx_Pad", "%d", &i_value) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[2] = i_value;
      dict_hdr.VelFun[2] = i_value;
   }
   if(1 == cdds_scanf("usp_VelFun usp_Nx_Win", "%d", &i_value) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[3] = i_value;
      dict_hdr.VelFun[3] = i_value;
   }
   if(1 == cdds_scanf("usp_VelFun usp_Nx_Rec", "%d", &i_value) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[4] = i_value;
      dict_hdr.VelFun[4] = i_value;
   }
   if(1 == cdds_scanf("usp_VelFun usp_NxLine", "%d", &i_value) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[5] = i_value;
      dict_hdr.VelFun[5] = i_value;
   }
   if(1 == cdds_scanf("usp_VelFun usp_Nx_Trc", "%d", &i_value) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[6] = i_value;
      dict_hdr.VelFun[6] = i_value;
   }
   if(1 == cdds_scanf("usp_VelFun usp_Ny_Pad", "%d", &i_value) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[7] = i_value;
      dict_hdr.VelFun[7] = i_value;
   }
   if(1 == cdds_scanf("usp_VelFun usp_Ny_Win", "%d", &i_value) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[8] = i_value;
      dict_hdr.VelFun[8] = i_value;
   }
   if(1 == cdds_scanf("usp_VelFun usp_Ny_Rec", "%d", &i_value) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[9] = i_value;
      dict_hdr.VelFun[9] = i_value;
   }
   if(1 == cdds_scanf("usp_VelFun usp_NyLine", "%d", &i_value) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[10] = i_value;
      dict_hdr.VelFun[10] = i_value;
   }
   if(1 == cdds_scanf("usp_VelFun usp_Ny_Trc", "%d", &i_value) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[11] = i_value;
      dict_hdr.VelFun[11] = i_value;
   }
   if(1 == cdds_scanf("usp_VelFun usp_MCTrSp", "%d", &i_value) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[12] = i_value;
      dict_hdr.VelFun[12] = i_value;
   }
   if(1 == cdds_scanf("usp_VelFun usp_NumCmp", "%d", &i_value) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[13] = i_value;
      dict_hdr.VelFun[13] = i_value;
   }
   if(1 == cdds_scanf("usp_VelFun usp_MCLE01", "%d", &i_value) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[14] = i_value;
      dict_hdr.VelFun[14] = i_value;
   }
   if(1 == cdds_scanf("usp_VelFun usp_OrNSMP", "%d", &i_value) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[40] = i_value;
      dict_hdr.VelFun[40] = i_value;
   }
   if(1 == cdds_scanf("usp_VelFun usp_AziIln", "%f", &FakeFloat) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[41] = 0;
      bin_hdr.VelFun[42] = 0;
      ddsFloat2Fake(&FakeFloat, bin_hdr.VelFun+41, bin_hdr.VelFun+42);
      dict_hdr.VelFun[41] = bin_hdr.VelFun[41];
      dict_hdr.VelFun[42] = bin_hdr.VelFun[42];
   }
   if(1 == cdds_scanf("usp_VelFun usp_AziXln", "%f", &FakeFloat) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[43] = 0;
      bin_hdr.VelFun[44] = 0;
      ddsFloat2Fake(&FakeFloat, bin_hdr.VelFun+43, bin_hdr.VelFun+44);
      dict_hdr.VelFun[43] = bin_hdr.VelFun[43];
      dict_hdr.VelFun[44] = bin_hdr.VelFun[44];
   }
   if(1 == cdds_scanf("usp_VelFun usp_HrzNul", "%f", &FakeFloat) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[45] = 0;
      bin_hdr.VelFun[46] = 0;
      ddsFloat2Fake(&FakeFloat, bin_hdr.VelFun+45, bin_hdr.VelFun+46);
      dict_hdr.VelFun[45] = bin_hdr.VelFun[45];
      dict_hdr.VelFun[46] = bin_hdr.VelFun[46];
   }
   if(1 == cdds_scanf("usp_VelFun usp_UnitSc", "%f", &FakeFloat) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[48] = 0;
      bin_hdr.VelFun[49] = 0;
      ddsFloat2Fake(&FakeFloat, bin_hdr.VelFun+48, bin_hdr.VelFun+49);
      dict_hdr.VelFun[48] = bin_hdr.VelFun[48];
      dict_hdr.VelFun[49] = bin_hdr.VelFun[49];
   }
   if(1 == cdds_scanf("usp_VelFun usp_SmpFlt", "%f", &FakeFloat) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[50] = 0;
      bin_hdr.VelFun[51] = 0;
      ddsFloat2Fake(&FakeFloat, bin_hdr.VelFun+50, bin_hdr.VelFun+51);
      dict_hdr.VelFun[50] = bin_hdr.VelFun[50];
      dict_hdr.VelFun[51] = bin_hdr.VelFun[51];
   }
   if(1 == cdds_scanf("usp_VelFun usp_MaxAmp", "%f", &FakeFloat) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[52] = 0;
      bin_hdr.VelFun[53] = 0;
      ddsFloat2Fake(&FakeFloat, bin_hdr.VelFun+52, bin_hdr.VelFun+53);
      dict_hdr.VelFun[52] = bin_hdr.VelFun[52];
      dict_hdr.VelFun[53] = bin_hdr.VelFun[53];
   }
   if(1 == cdds_scanf("usp_VelFun usp_AvgAmp", "%f", &FakeFloat) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[54] = 0;
      bin_hdr.VelFun[55] = 0;
      ddsFloat2Fake(&FakeFloat, bin_hdr.VelFun+54, bin_hdr.VelFun+55);
      dict_hdr.VelFun[54] = bin_hdr.VelFun[54];
      dict_hdr.VelFun[55] = bin_hdr.VelFun[55];
   }
   if(1 == cdds_scanf("usp_VelFun usp_RmsAmp", "%f", &FakeFloat) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[56] = 0;
      bin_hdr.VelFun[57] = 0;
      ddsFloat2Fake(&FakeFloat, bin_hdr.VelFun+56, bin_hdr.VelFun+57);
      dict_hdr.VelFun[56] = bin_hdr.VelFun[56];
      dict_hdr.VelFun[57] = bin_hdr.VelFun[57];
   }
   if(1 == cdds_scanf("usp_VelFun usp_FreQst", "%f", &FakeFloat) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[68] = 0;
      bin_hdr.VelFun[69] = 0;
      ddsFloat2Fake(&FakeFloat, bin_hdr.VelFun+68, bin_hdr.VelFun+69);
      dict_hdr.VelFun[68] = bin_hdr.VelFun[68];
      dict_hdr.VelFun[68] = bin_hdr.VelFun[69];
   }
   if(1 == cdds_scanf("usp_VelFun usp_FreQnd", "%f", &FakeFloat) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[70] = 0;
      bin_hdr.VelFun[71] = 0;
      ddsFloat2Fake(&FakeFloat, bin_hdr.VelFun+70, bin_hdr.VelFun+71);
      dict_hdr.VelFun[70] = bin_hdr.VelFun[70];
      dict_hdr.VelFun[71] = bin_hdr.VelFun[71];
   }
   if(1 == cdds_scanf("usp_VelFun usp_RaMmin", "%f", &FakeFloat) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[72] = 0;
      bin_hdr.VelFun[73] = 0;
      ddsFloat2Fake(&FakeFloat, bin_hdr.VelFun+72, bin_hdr.VelFun+73);
      dict_hdr.VelFun[72] = bin_hdr.VelFun[72];
      dict_hdr.VelFun[73] = bin_hdr.VelFun[73];
   }
   if(1 == cdds_scanf("usp_VelFun usp_RaMmax", "%f", &FakeFloat) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[74] = 0;
      bin_hdr.VelFun[75] = 0;
      ddsFloat2Fake(&FakeFloat, bin_hdr.VelFun+74, bin_hdr.VelFun+75);
      dict_hdr.VelFun[74] = bin_hdr.VelFun[74];
      dict_hdr.VelFun[75] = bin_hdr.VelFun[75];
   }
   if(1 == cdds_scanf("usp_VelFun usp_RaXmax", "%f", &FakeFloat) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[76] = 0;
      bin_hdr.VelFun[77] = 0;
      ddsFloat2Fake(&FakeFloat, bin_hdr.VelFun+76, bin_hdr.VelFun+77);
      dict_hdr.VelFun[76] = bin_hdr.VelFun[76];
      dict_hdr.VelFun[77] = bin_hdr.VelFun[77];
   }
   if(1 == cdds_scanf("usp_VelFun usp_RaZref", "%f", &FakeFloat) &&
      strcmp("usp_VelFun", dds_scan_name)) {
      bin_hdr.VelFun[78] = 0;
      bin_hdr.VelFun[79] = 0;
      ddsFloat2Fake(&FakeFloat, bin_hdr.VelFun+78, bin_hdr.VelFun+79);
      dict_hdr.VelFun[78] = bin_hdr.VelFun[78];
      dict_hdr.VelFun[79] = bin_hdr.VelFun[79];
   }
   HDR_GET_INT(usp_, "", HlhEnt)
   HDR_GET_INT(usp_, "", HlhByt)

   /*************************************/
   /* Force rational line header values */
   /*************************************/

   bin_hdr.NumSmp = NumSmp;
/***  bad idea
   bin_hdr.NumTrc = NumTrc;
   bin_hdr.NumRec = NumRec;
***/
   bin_hdr.HlhByt = HlhByt;

   /*******************************************************/
   /* Merge line header and nominal cube description.     */
   /*                                                     */
   /* The dictionary has precedence over the binary       */
   /* line header, if the parameters are ambiguous.       */
   /* Within the dictionary, the cube description has     */
   /* precedence over any format specific definitions.    */
   /*    Example: "delta.t= M" has precedence over        */
   /*       "usp_SmpInt= N", if both are defined.         */
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

   if(axis_t) {
      /* "time" axis DOES exist. */
      ddsFake2Float(&UnitSc, bin_hdr.VelFun+48, bin_hdr.VelFun+49);
      usp_units = "sec";
      if(! UnitSc) {
         UnitSc = 1.e-3;
      } else if(! ddsCmpDbl(UnitSc, 1.)) {
         UnitSc = 1.;
      } else if(! ddsCmpDbl(UnitSc, 1.e-3)) {
         UnitSc = 1.e-3;
      } else if(! ddsCmpDbl(UnitSc, 1.e-6)) {
         UnitSc = 1.e-6;
      } else if(! ddsCmpDbl(UnitSc, 1.e-9)) {
         UnitSc = 1.e-9;
      } else {
         /* ddsWarn("bogus usp UnitSc= %f, assuming milliseconds\n\t%s\n", */
         ddsWarn("bogus usp UnitSc= %f, using it anyway!\n\t%s\n",
            UnitSc, media->title.ptr);
      }

      if(strstr(bin->cube.axis[axis_t].units, "?")) {
         cube_units = usp_units;
         bin->cube.axis[axis_t].units = ddsString(usp_units);
      } else {
         cube_units = bin->cube.axis[axis_t].units;
      }

      sec_to_cube = cdds_scale(cube_units, "sec");

      if(! sec_to_cube) {
         ddsWarn("delta.t %s unknown, given usp Sample %s\n\t%s\n",
            cube_units, usp_units,
            media->title.ptr);
         sec_to_cube = 1.;
      }

      if(! (bin->cube.axis[axis_t].flag & DDS_AXIS_origin)) {
         /* Derive cube parameter from binary header. */
         bin->cube.axis[axis_t].origin = UnitSc * bin_hdr.TmMsFS *
            sec_to_cube;
         bin->cube.axis[axis_t].flag |= DDS_AXIS_origin;
      }

      if(! (bin->cube.axis[axis_t].flag & DDS_AXIS_delta)) {
         /* Derive cube parameter from binary header. */
         bin->cube.axis[axis_t].delta = UnitSc * bin_hdr.SmpInt *
            sec_to_cube;
         bin->cube.axis[axis_t].flag |= DDS_AXIS_delta;
      }
   }

   if(bin_hdr.UnitFl == USP_UnitFl_FEET) {
      usp_units = "feet";
   } else if(bin_hdr.UnitFl == USP_UnitFl_METERS) {
      usp_units = "meters";
   } else {
      bin_hdr.UnitFl = USP_UnitFl_other;
      usp_units = "usp_xy_units?";
   }

   if(axis_x && bin_hdr.TmMsSl &&
      ! (bin->cube.axis[axis_x].flag & DDS_AXIS_delta)) {
      /* "x-space" axis DOES exist (Note: Dx1000 == TmMsSl). */
      /* Derive cube parameter from binary header. */
      cube_units = bin->cube.axis[axis_x].units;

      if(strstr(cube_units, "?")) {
         if(! strcmp(bin->cube.axis[axis_x].units, "?")) {
            bin->cube.axis[axis_x].units = ddsString(usp_units);
         }
         usp_to_cube = 1.;
      } else {
         usp_to_cube = cdds_scale(cube_units, usp_units);
      }

      if(! usp_to_cube) {
         ddsWarn("delta.x %s unknown, given usp Dx1000 %s\n\t%s\n",
            (cube_units ? cube_units : "<cube_units>"), usp_units,
            media->title.ptr);
         usp_to_cube = 1.;
      }

      bin->cube.axis[axis_x].delta = (bin_hdr.TmMsSl / 1000.0) *
         usp_to_cube;
      bin->cube.axis[axis_x].flag |= DDS_AXIS_delta;
   }

   if(axis_y && bin_hdr.DtInFl &&
      ! (bin->cube.axis[axis_y].flag & DDS_AXIS_delta)) {
      /* "y-space" axis DOES exist (Note: Dy1000 == DtInFl). */
      /* Derive cube parameter from binary header. */
      cube_units = bin->cube.axis[axis_y].units;

      if(strstr(cube_units, "?")) {
         if(! strcmp(bin->cube.axis[axis_y].units, "?")) {
            bin->cube.axis[axis_y].units = ddsString(usp_units);
         }
         usp_to_cube = 1.;
      } else {
         usp_to_cube = cdds_scale(cube_units, usp_units);
      }

      if(! usp_to_cube) {
         ddsWarn("delta.y %s unknown, given usp Dy1000 %s\n\t%s\n",
            (cube_units ? cube_units : "<cube_units>"), usp_units,
            media->title.ptr);
         usp_to_cube = 1.;
      }

      bin->cube.axis[axis_y].delta = (bin_hdr.DtInFl / 1000.) *
         usp_to_cube;
      bin->cube.axis[axis_y].flag |= DDS_AXIS_delta;
   }

   if(axis_z && bin_hdr.TmSlIn &&
      ! (bin->cube.axis[axis_z].flag & DDS_AXIS_delta)) {
      /* "z-space" axis DOES exist (Note: Dz1000 == TmSlIn). */
      /* Derive cube parameter from binary header. */
      cube_units = bin->cube.axis[axis_z].units;

      if(strstr(cube_units, "?")) {
         if(! strcmp(bin->cube.axis[axis_z].units, "?")) {
            bin->cube.axis[axis_z].units = ddsString(usp_units);
         }
         usp_to_cube = 1.;
      } else {
         usp_to_cube = cdds_scale(cube_units, usp_units);
      }

      if(! usp_to_cube) {
         ddsWarn("delta.z %s unknown, given usp Dz1000 %s\n\t%s\n",
            (cube_units ? cube_units : "<cube_units>"), usp_units,
            media->title.ptr);
         usp_to_cube = 1.;
      }

      bin->cube.axis[axis_z].delta = (bin_hdr.TmSlIn / 1000.) *
         usp_to_cube;
      bin->cube.axis[axis_z].flag |= DDS_AXIS_delta;
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

   HDR_MOD_CHAR(usp_, EqpCod, 1)
   HDR_MOD_CHAR(usp_, CrwNam, 1)
   HDR_MOD_CHAR(usp_, DatTyp, 1)
   HDR_MOD_CHAR(usp_, DgTrkS, 1)
   HDR_MOD_CHAR(usp_, PrcNam, 1)
   HDR_MOD_CHAR(usp_, PrcDat, 1)
   HDR_MOD_CHAR(usp_, OACLin, 1)
   HDR_MOD_CHAR(usp_, JobNum, 1)
   HDR_MOD_INT(usp_, NumTrc)
   HDR_MOD_INT(usp_, NumRec)
   HDR_MOD_INT(usp_, SmpInt)
   HDR_MOD_INT(usp_, NumSmp)
   HDR_MOD_INT(usp_, Format)
   HDR_MOD_INT(usp_, IndAdj)
   HDR_MOD_INT(usp_, OpGrFl)
   HDR_MOD_CHAR(usp_, SrtTyp, 1)
   HDR_MOD_INT(usp_, CDPFld)
   HDR_MOD_CHAR(usp_, GrpInt, 1)
   HDR_MOD_INT(usp_, DtInFl)

   if(bin_hdr.DtInFl != dict_hdr.DtInFl)
      cdds_printf("usp_Dy1000", " %i\n", bin_hdr.DtInFl);

   HDR_MOD_INT(usp_, MxSPEl)
   HDR_MOD_INT(usp_, MnSPEl)
   HDR_MOD_INT(usp_, MxRSEL)
   HDR_MOD_INT(usp_, MnRSEL)
   HDR_MOD_INT(usp_, MxGrEl)
   HDR_MOD_INT(usp_, MnGrEl)
   HDR_MOD_INT(usp_, MxTrSt)
   HDR_MOD_INT(usp_, MnTrSt)
   HDR_MOD_CHAR(usp_, LinDir, 1)
   HDR_MOD_INT(usp_, AERcPr)
   HDR_MOD_INT(usp_, OrNTRC)
   HDR_MOD_INT(usp_, OrNREC)
   HDR_MOD_FLOAT(usp_, MutVel)
   HDR_MOD_INT(usp_, ZDMuTm)
   HDR_MOD_INT(usp_, IntInc)
   HDR_MOD_INT(usp_, WatVel)

   if(bin_hdr.WatVel != dict_hdr.WatVel)
      cdds_printf("usp_RefVel", " %i\n", bin_hdr.WatVel);

   HDR_MOD_INT(usp_, MxShDp)
   HDR_MOD_INT(usp_, MnShDp)
   HDR_MOD_INT(usp_, MxUHTm)
   HDR_MOD_INT(usp_, MnUHTm)
   HDR_MOD_INT(usp_, MxTrOf)
   HDR_MOD_INT(usp_, MnTrOf)
   HDR_MOD_INT(usp_, RATTrc)

   if(bin_hdr.RATTrc != dict_hdr.RATTrc)
      cdds_printf("usp_AnglSt", " %i\n", bin_hdr.RATTrc);

   HDR_MOD_INT(usp_, RATFld)

   if(bin_hdr.RATFld != dict_hdr.RATFld)
      cdds_printf("usp_AngInc", " %i\n", bin_hdr.RATFld);

   HDR_MOD_INT(usp_, UnitFl)
   HDR_MOD_INT(usp_, FrstSP)
   HDR_MOD_INT(usp_, DpN1SP)
   HDR_MOD_INT(usp_, NmDpIn)
   HDR_MOD_INT(usp_, StWdFl)
   HDR_MOD_INT(usp_, DptInt)
   HDR_MOD_INT(usp_, TmMsSl)

   if(bin_hdr.TmMsSl != dict_hdr.TmMsSl)
      cdds_printf("usp_Dx1000", " %i\n", bin_hdr.TmMsSl);

   HDR_MOD_INT(usp_, TmSlIn)

   if(bin_hdr.TmSlIn != dict_hdr.TmSlIn)
      cdds_printf("usp_Dz1000", " %i\n", bin_hdr.TmSlIn);

   HDR_MOD_FLOAT(usp_, TmMsFS)
   HDR_MOD_INT(usp_, NTrLnS)
   HDR_MOD_INT(usp_, PltDir)
   HDR_MOD_INT(usp_, LgRfEl)
   HDR_MOD_INT(usp_, MnLnIn)
   HDR_MOD_INT(usp_, MxLnIn)
   HDR_MOD_INT(usp_, MnDpIn)
   HDR_MOD_INT(usp_, MxDpIn)
   HDR_MOD_FLOAT(usp_, ILClIn)
   HDR_MOD_FLOAT(usp_, CLClIn)
   HDR_MOD_INT(usp_, ReSpFm)
   HDR_MOD_INT(usp_, VlFnRF)
   HDR_MOD_CHAR(usp_, OACUsr, 1)
   HDR_MOD_CHAR(usp_, LinNam, 1)
   HDR_MOD_INT(usp_, MutFlg)
   HDR_MOD_CHAR(usp_, SPBias, 1)
   HDR_MOD_INT(usp_, SpBiFl)
   HDR_MOD_CHAR(usp_, APIWNo, 1)
   HDR_MOD_FLOAT(usp_, NmSpMi)

   /**********************************************************/
   /* The dictionary MUST be updated with these union floats */
   /* BEFORE updating the VelFun array, otherwise the floats */
   /* have precedence and may screw up fidelity if the field */
   /* is actually an integer (bogus float representation).   */
   /**********************************************************/

   if(bin_hdr.VelFun[41] != dict_hdr.VelFun[41] ||
      bin_hdr.VelFun[42] != dict_hdr.VelFun[42]) {
      ddsFake2Float(&FakeFloat, bin_hdr.VelFun+41, bin_hdr.VelFun+42);
      cdds_printf("usp_AziIln", " %g\n", FakeFloat);
   }
   if(bin_hdr.VelFun[43] != dict_hdr.VelFun[43] ||
      bin_hdr.VelFun[44] != dict_hdr.VelFun[44]) {
      ddsFake2Float(&FakeFloat, bin_hdr.VelFun+43, bin_hdr.VelFun+44);
      cdds_printf("usp_AziXln", " %g\n", FakeFloat);
   }
   if(bin_hdr.VelFun[45] != dict_hdr.VelFun[45] ||
      bin_hdr.VelFun[46] != dict_hdr.VelFun[46]) {
      ddsFake2Float(&FakeFloat, bin_hdr.VelFun+45, bin_hdr.VelFun+46);
      cdds_printf("usp_HrzNul", " %g\n", FakeFloat);
   }
   if(bin_hdr.VelFun[48] != dict_hdr.VelFun[48] ||
      bin_hdr.VelFun[49] != dict_hdr.VelFun[49]) {
      ddsFake2Float(&FakeFloat, bin_hdr.VelFun+48, bin_hdr.VelFun+49);
      cdds_printf("usp_UnitSc", " %g\n", FakeFloat);
   }
   if(bin_hdr.VelFun[50] != dict_hdr.VelFun[50] ||
      bin_hdr.VelFun[51] != dict_hdr.VelFun[51]) {
      ddsFake2Float(&FakeFloat, bin_hdr.VelFun+50, bin_hdr.VelFun+51);
      cdds_printf("usp_SmpFlt", " %g\n", FakeFloat);
   }
   if(bin_hdr.VelFun[52] != dict_hdr.VelFun[52] ||
      bin_hdr.VelFun[53] != dict_hdr.VelFun[53]) {
      ddsFake2Float(&FakeFloat, bin_hdr.VelFun+52, bin_hdr.VelFun+53);
      cdds_printf("usp_MaxAmp", " %g\n", FakeFloat);
   }
   if(bin_hdr.VelFun[54] != dict_hdr.VelFun[54] ||
      bin_hdr.VelFun[55] != dict_hdr.VelFun[55]) {
      ddsFake2Float(&FakeFloat, bin_hdr.VelFun+54, bin_hdr.VelFun+55);
      cdds_printf("usp_AvgAmp", " %g\n", FakeFloat);
   }
   if(bin_hdr.VelFun[56] != dict_hdr.VelFun[56] ||
      bin_hdr.VelFun[57] != dict_hdr.VelFun[57]) {
      ddsFake2Float(&FakeFloat, bin_hdr.VelFun+56, bin_hdr.VelFun+57);
      cdds_printf("usp_RmsAmp", " %g\n", FakeFloat);
   }
   if(bin_hdr.VelFun[68] != dict_hdr.VelFun[68] ||
      bin_hdr.VelFun[69] != dict_hdr.VelFun[69]) {
      ddsFake2Float(&FakeFloat, bin_hdr.VelFun+68, bin_hdr.VelFun+69);
      cdds_printf("usp_FreQst", " %g\n", FakeFloat);
   }
   if(bin_hdr.VelFun[70] != dict_hdr.VelFun[70] ||
      bin_hdr.VelFun[71] != dict_hdr.VelFun[71]) {
      ddsFake2Float(&FakeFloat, bin_hdr.VelFun+70, bin_hdr.VelFun+71);
      cdds_printf("usp_FreQnd", " %g\n", FakeFloat);
   }
   if(bin_hdr.VelFun[72] != dict_hdr.VelFun[72] ||
      bin_hdr.VelFun[73] != dict_hdr.VelFun[73]) {
      ddsFake2Float(&FakeFloat, bin_hdr.VelFun+72, bin_hdr.VelFun+73);
      cdds_printf("usp_RaMmin", " %g\n", FakeFloat);
   }
   if(bin_hdr.VelFun[74] != dict_hdr.VelFun[74] ||
      bin_hdr.VelFun[73] != dict_hdr.VelFun[73]) {
      ddsFake2Float(&FakeFloat, bin_hdr.VelFun+74, bin_hdr.VelFun+73);
      cdds_printf("usp_RaMmax", " %g\n", FakeFloat);
   }
   if(bin_hdr.VelFun[76] != dict_hdr.VelFun[76] ||
      bin_hdr.VelFun[77] != dict_hdr.VelFun[77]) {
      ddsFake2Float(&FakeFloat, bin_hdr.VelFun+76, bin_hdr.VelFun+77);
      cdds_printf("usp_RaXmax", " %g\n", FakeFloat);
   }
   if(bin_hdr.VelFun[78] != dict_hdr.VelFun[78] ||
      bin_hdr.VelFun[79] != dict_hdr.VelFun[79]) {
      ddsFake2Float(&FakeFloat, bin_hdr.VelFun+78, bin_hdr.VelFun+79);
      cdds_printf("usp_RaZref", " %g\n", FakeFloat);
   }

   HDR_MOD_INT(usp_, VelFun)

   if(bin_hdr.VelFun[0] != dict_hdr.VelFun[0])
      cdds_printf("usp_MinVel", " %i\n", bin_hdr.VelFun[0]);
   if(bin_hdr.VelFun[1] != dict_hdr.VelFun[1])
      cdds_printf("usp_MaxVel", " %i\n", bin_hdr.VelFun[1]);
   if(bin_hdr.VelFun[2] != dict_hdr.VelFun[2])
      cdds_printf("usp_Nx_Pad", " %i\n", bin_hdr.VelFun[2]);
   if(bin_hdr.VelFun[3] != dict_hdr.VelFun[3])
      cdds_printf("usp_Nx_Win", " %i\n", bin_hdr.VelFun[3]);
   if(bin_hdr.VelFun[4] != dict_hdr.VelFun[4])
      cdds_printf("usp_Nx_Rec", " %i\n", bin_hdr.VelFun[4]);
   if(bin_hdr.VelFun[5] != dict_hdr.VelFun[5])
      cdds_printf("usp_NxLine", " %i\n", bin_hdr.VelFun[5]);
   if(bin_hdr.VelFun[6] != dict_hdr.VelFun[6])
      cdds_printf("usp_Nx_Trc", " %i\n", bin_hdr.VelFun[6]);
   if(bin_hdr.VelFun[7] != dict_hdr.VelFun[7])
      cdds_printf("usp_Ny_Pad", " %i\n", bin_hdr.VelFun[7]);
   if(bin_hdr.VelFun[8] != dict_hdr.VelFun[8])
      cdds_printf("usp_Ny_Win", " %i\n", bin_hdr.VelFun[8]);
   if(bin_hdr.VelFun[9] != dict_hdr.VelFun[9])
      cdds_printf("usp_Ny_Rec", " %i\n", bin_hdr.VelFun[9]);
   if(bin_hdr.VelFun[10] != dict_hdr.VelFun[10])
      cdds_printf("usp_NyLine", " %i\n", bin_hdr.VelFun[10]);
   if(bin_hdr.VelFun[11] != dict_hdr.VelFun[11])
      cdds_printf("usp_Ny_Trc", " %i\n", bin_hdr.VelFun[11]);
   if(bin_hdr.VelFun[12] != dict_hdr.VelFun[12])
      cdds_printf("usp_MCTrSp", " %i\n", bin_hdr.VelFun[12]);
   if(bin_hdr.VelFun[13] != dict_hdr.VelFun[13])
      cdds_printf("usp_NumCmp", " %i\n", bin_hdr.VelFun[13]);
   if(bin_hdr.VelFun[14] != dict_hdr.VelFun[14])
      cdds_printf("usp_MCLE01", " %i\n", bin_hdr.VelFun[14]);
   if(bin_hdr.VelFun[15] != dict_hdr.VelFun[15])
      cdds_printf("usp_MCLE02", " %i\n", bin_hdr.VelFun[15]);
   if(bin_hdr.VelFun[16] != dict_hdr.VelFun[16])
      cdds_printf("usp_MCLE03", " %i\n", bin_hdr.VelFun[16]);
   if(bin_hdr.VelFun[17] != dict_hdr.VelFun[17])
      cdds_printf("usp_MCLE04", " %i\n", bin_hdr.VelFun[17]);
   if(bin_hdr.VelFun[18] != dict_hdr.VelFun[18])
      cdds_printf("usp_MCLE05", " %i\n", bin_hdr.VelFun[18]);
   if(bin_hdr.VelFun[19] != dict_hdr.VelFun[19])
      cdds_printf("usp_MCLE06", " %i\n", bin_hdr.VelFun[19]);
   if(bin_hdr.VelFun[20] != dict_hdr.VelFun[20])
      cdds_printf("usp_MCLE07", " %i\n", bin_hdr.VelFun[20]);
   if(bin_hdr.VelFun[21] != dict_hdr.VelFun[21])
      cdds_printf("usp_MCLE08", " %i\n", bin_hdr.VelFun[21]);
   if(bin_hdr.VelFun[22] != dict_hdr.VelFun[22])
      cdds_printf("usp_MCLE09", " %i\n", bin_hdr.VelFun[22]);
   if(bin_hdr.VelFun[23] != dict_hdr.VelFun[23])
      cdds_printf("usp_MCLE10", " %i\n", bin_hdr.VelFun[23]);
   if(bin_hdr.VelFun[24] != dict_hdr.VelFun[24])
      cdds_printf("usp_MCLE11", " %i\n", bin_hdr.VelFun[24]);
   if(bin_hdr.VelFun[25] != dict_hdr.VelFun[25])
      cdds_printf("usp_MCLE12", " %i\n", bin_hdr.VelFun[25]);
   if(bin_hdr.VelFun[26] != dict_hdr.VelFun[26])
      cdds_printf("usp_MCLE13", " %i\n", bin_hdr.VelFun[26]);
   if(bin_hdr.VelFun[27] != dict_hdr.VelFun[27])
      cdds_printf("usp_MCLE14", " %i\n", bin_hdr.VelFun[27]);
   if(bin_hdr.VelFun[28] != dict_hdr.VelFun[28])
      cdds_printf("usp_MCLE15", " %i\n", bin_hdr.VelFun[28]);
   if(bin_hdr.VelFun[29] != dict_hdr.VelFun[29])
      cdds_printf("usp_MCLE16", " %i\n", bin_hdr.VelFun[29]);
   if(bin_hdr.VelFun[40] != dict_hdr.VelFun[40])
      cdds_printf("usp_OrNSMP", " %i\n", bin_hdr.VelFun[40]);

   HDR_MOD_INT(usp_, HlhEnt)
   HDR_MOD_INT(usp_, HlhByt)

   /**********************************************/
   /* add HLH to dictionary if not already there */
   /**********************************************/

   cdds_dict(bin->dict_open, "scan");
   if (cdds_scanf("previous_history", "") < 0) {
      int truncate=0;
      cdds_dict(bin->dict_open, "print");
      hlh_ptr = green_prec + usp_line_min;

      while(HlhEnt && hlh_ptr < green_line + green_prec) {
         CON_ANSS(DDS_CTYPE_UINT, short_stype, &len, line_buf + hlh_ptr);
         hlh_ptr += HlhByt_prec;
         ddsStrCpyN(&tmp_str, line_buf + hlh_ptr, len);
         hlh_ptr += len;
         if((HlhByt_prec==8) && (len % HlhByt_prec))
            hlh_ptr += HlhByt_prec - len % HlhByt_prec;

         if(strncmp(tmp_str.ptr, "dict:", 5) && strncmp(tmp_str.ptr, " dict:", 6)) {
            /* remove 1st & last '"' and replace any others with "'" */
            hlh_str = tmp_str.ptr;
            if (*hlh_str == '"') hlh_str++;
            n = strlen(hlh_str);
            if (hlh_str[n-1] == '"') hlh_str[n-1] = '\0';
            n = strlen(hlh_str);
            /* limit size */
            if (n > 1024) {
               n = 1024;
               if(!truncate) {
                  truncate = 1;
                  ddsWarn("ddsOpenOldUsp: truncating huge HLH entries, \n\tbinary= %s\n",
                          bin->name);
               }
            }
            /* build string replacing unprintable characters, escaping internal quotes & "=" */
            hlh = (char*)malloc(2*n);
            for(i=j=0;i<n;i++,j++) {
               if (!isprint(0377 & hlh_str[i]) && 
                   !isspace(0377 & hlh_str[i])) {
                  hlh[j] = '~';
               } else if (hlh_str[i] == '"') {
                  hlh[j] = '\''; 
               } else if (hlh_str[i] == '\\') {
                  hlh[j] = '\\'; 
                  hlh[++j] = hlh_str[++i]; 
               } else if (hlh_str[i] == '=') {
                  hlh[j] = '\\'; 
                  hlh[++j] = '='; 
               } else {
                  hlh[j] = hlh_str[i];
               }
            }
            hlh[j] = '\0';
            cdds_printf("previous_history", "\"%s\"\n", hlh);
            free(hlh);
         }

         HlhEnt--;
      }
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
      ddsError("Bogus usp format trace declaration\n\tbinary= %s\n",
         bin->name);
      return;
   }

   /*************************/
   /* link cube description */
   /*************************/

   ddsCubeLinker(bin);

   type = bin->trace.type;

   ddsLinkMember(&bin->prefix, type->block, "green_prefix");
   ddsLinkMember(&bin->TrcType, type->block, "StaCor");
   ddsLinkMember(&bin->Samples, type->block, "Samples");

   /****************/
   /* sanity check */
   /****************/

   type = bin->std.usp.line.type;
   member = bin->std.usp.line_HlhBuf.member;
   if(member->offset != usp_line_min + green_prec) {
      ddsError("USP line HlhBuf offset= %lu, expected %i\n\tbinary= %s\n",
         (unsigned long)member->offset,
         (int)(usp_line_min + green_prec), bin->name);
   } else if(green_line != type->stroke - green_prec) {
      ddsError("USP line green word= %i, expected %i\n\tbinary= %s\n",
         (int)green_line, (int)(type->stroke - green_prec), bin->name);
   }

   /****************/
   /* sanity check */
   /****************/

   type = bin->trace.type;
   member = bin->Samples.member;
   if(member->offset != usp_trace_min + green_prec) {
      ddsError("USP trace Samples offset= %lu, expected %i\n\tbinary= %s\n",
         (unsigned long)member->offset,
         (int)(usp_trace_min + green_prec), bin->name);
   } else if(green_trace != type->stroke - green_prec) {
      ddsError("USP trace green word= %i, expected %i\n\tbinary= %s\n",
         (int)green_trace, (int)(type->stroke - green_prec), bin->name);
   }

   /*********************************************************/
   /* allocate buffer for one line header and initialize it */
   /*********************************************************/
 
   type = bin->std.usp.line.type;
   bin->std.usp.line_buf = ddsMalloc(type->stroke);
   memcpy(bin->std.usp.line_buf, line_buf, type->stroke);
   bin->std.usp.line_green_min = usp_line_min;
   bin->std.usp.line_green_max = usp_line_max;

   if((dds_state & DDS_DUMP_DATA) && (dds_state & DDS_DUMP_READ)) {
      ddsDumpDataBuf(bin, "read", (DDS_SYM*)type, line_buf);
   }

   /*********************************************************/
   /* allocate buffer for one dead trace, and initialize it */
   /*********************************************************/

   type = bin->trace.type;
   bin->trace_buf = (char*)ddsMalloc(type->stroke);

   if(dds_state & DDS_DUMP_DIFF) {
      bin->diff1_buf = ddsMalloc(type->stroke);
      memset(bin->diff1_buf, DDS_NON_CHAR, type->stroke);
      bin->diff2_buf = ddsMalloc(type->stroke);
      memset(bin->diff2_buf, DDS_NON_CHAR, type->stroke);
   }

   bin->dead_trace = ddsZalloc(type->stroke);

   type = bin->TrcType.type;
   member = bin->TrcType.member;
   i_value = USP_TRACE_DEAD;
   CON_NASS(type->ptype, DDS_CTYPE_INT,
      bin->dead_trace + member->offset, &i_value);

   cdds_dict(bin->dict_open, "scan");
   
   return;
}
