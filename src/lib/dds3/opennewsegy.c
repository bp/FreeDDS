static char rcsid[]="$Id: opennewsegy.c 219 2011-02-16 01:11:40Z user $";
/*
RCS-t=DDS, open new binary data, segy format

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 22 Oct 1996            ***/
/**********************************************/

#include "dds_sys.h"
#include "segy_hdr.h"

void ddsOpenNewSegy(
   DDS_BIN *bin,
   DDS_CUBE *old_cube)
{
   SEGY_LINE dict_hdr, bin_hdr;
   DDS_PTYPE char_stype, short_stype, int_stype, float_stype;
   DDS_PTYPE green_stype, samples_stype;
   DDS_TYPE *type;
   DDS_MEMBER *member;
   DDS_BLOCK *block, *hdr_block;
   DDS_MEDIA *media = bin->media;
   DDS_AXIS *axis;
   DDS_STRING tmp_str = NULL_STRING;
   char *hdr_buf, *card_buf, *binary_buf;
   int rank, axis_t;
   const char *cube_units;
   double cube_to_usec;
   int green_prefix_prec, green_suffix_prec;
   int green_card, green_binary, green_trace;
   int sample_prec, SU;
   int card_pad, binary_pad, trace_pad;
   int SegyFmt;

   /***********************************/
   /* fake su format as segy for now  */
   /***********************************/

   SU = (bin->fmt == DDS_FMT_SU);
   if (SU) {
      cdds_dict(bin->dict_open, "print");
      cdds_printf("$fmt:su","fmt:segy\n");
      bin->fmt == DDS_FMT_SEGY;
      bin->fmt_name = ddsString("segy");
   }

   /**************************/
   /* Check for segy Rev. 1  */
   /**************************/

   if (bin->fmt == DDS_FMT_SEGY1) {
      SegyFmt = 0x0100;
   } else {
      SegyFmt = 0;
   }

   /***************************************************/
   /* DEFault header values from dictionary.          */
   /*                                                 */
   /* If the dictionary defines a particular header,  */
   /* then its value is retrieved and saved in the    */
   /* corresponding dict_hdr and bin_hdr members.     */
   /* If not defined, the members are defaulted       */
   /* (zero if numeric, blank if character).          */
   /***************************************************/

   /* setup implicit arguments used by HDR_DEF */
   cdds_dict(bin->dict_open, "scan");

   HDR_DEF_CHAR(segy_, card3200, 40)

   HDR_DEF_INT(segy_, JobNum)
   HDR_DEF_INT(segy_, LineNum)
   HDR_DEF_INT(segy_, ReelNum)
   HDR_DEF_INT(segy_, NumTrc)
   HDR_DEF_INT(segy_, NumAux)
   HDR_DEF_INT(segy_, SmpInt)
   HDR_DEF_INT(segy_, OrigSmpInt)
   HDR_DEF_INT(segy_, NumSmp)
   HDR_DEF_INT(segy_, OrigNumSmp)
   HDR_DEF_INT(segy_, FmtCode)
   HDR_DEF_INT(segy_, CdpFold)
   HDR_DEF_INT(segy_, SortCode)
   HDR_DEF_INT(segy_, VertSumFlag)
   HDR_DEF_INT(segy_, SwpFreqStart)
   HDR_DEF_INT(segy_, SwpFreqEnd)
   HDR_DEF_INT(segy_, SwpLength)
   HDR_DEF_INT(segy_, SwpCode)
   HDR_DEF_INT(segy_, SwpChan)
   HDR_DEF_INT(segy_, SwpTprStart)
   HDR_DEF_INT(segy_, SwpTprEnd)
   HDR_DEF_INT(segy_, SwpTprType)
   HDR_DEF_INT(segy_, CorrFlag)
   HDR_DEF_INT(segy_, GainFlag)
   HDR_DEF_INT(segy_, AmpFlag)
   HDR_DEF_INT(segy_, UnitsFlag)
   HDR_DEF_INT(segy_, PolarityFlag)
   HDR_DEF_INT(segy_, VibLagFlag)
   HDR_DEF_INT(segy_, SpareBinI2)
   HDR_DEF_INT(segy_, SegyFmt)
   HDR_DEF_INT(segy_, FixedLength)
   HDR_DEF_INT(segy_, NumHdr)
   HDR_DEF_INT(segy_, Spare2BinI2)
 
   /*************************************/
   /* force rational line header values */
   /*************************************/

   bin->cube.axis[1].flag |= DDS_AXIS_size;
   if(bin->cube.axis[1].size < 1)
      bin->cube.axis[1].size = 1;
   bin_hdr.NumSmp = bin->cube.axis[1].size;

   if(bin->cube.rank >= 2) {
      bin->cube.axis[2].flag |= DDS_AXIS_size;
      if(bin->cube.axis[2].size < 1)
         bin->cube.axis[2].size = 1;
      bin_hdr.NumTrc = bin->cube.axis[2].size;
   } else {
      bin_hdr.NumTrc = 1;
   }

   if(bin->cube.rank >= 3) {
      bin->cube.axis[3].flag |= DDS_AXIS_size;
      if(bin->cube.axis[3].size < 1)
         bin->cube.axis[3].size = 1;
   }

   bin_hdr.SegyFmt = SegyFmt;
   bin_hdr.FixedLength = 1;
   bin_hdr.NumHdr = 0;

   card_pad = 0;
   binary_pad = 0;
   trace_pad = 0;

   /***************************************************/
   /* Merge line header and nominal cube description. */
   /***************************************************/

   /* Discover rank of standard axis names, if they exist. */
   axis_t = 0;
   bin_hdr.UnitsFlag = SEGY_UNITSFLAG_FEET;

   for(rank = 1; rank <= bin->cube.rank; rank++) {
      axis = bin->cube.axis + rank;

      if(!strcmp(axis->name,"t")) axis_t = rank;
      
      if(axis->units && !strcmp(axis->units,"meters")) {
         bin_hdr.UnitsFlag = SEGY_UNITSFLAG_METERS;
      }
   }

   if(axis_t == 1) {
      /* "time" axis 1 */
      if(strcmp(bin->cube.axis[axis_t].units, "?")) {
         cube_units = bin->cube.axis[axis_t].units;
         cube_to_usec = cdds_scale("usec", cube_units);

         if(! cube_to_usec) {
            ddsWarn("sec unknown, given delta.t %s\n\t%s\n",
               cube_units, media->title.ptr);
            cube_to_usec = 1.;
         }
      }

      if(bin->cube.axis[axis_t].flag & DDS_AXIS_delta) {
         bin_hdr.SmpInt = ROUND(bin->cube.axis[axis_t].delta * cube_to_usec);
      }
   } else if(bin->cube.axis[1].flag & DDS_AXIS_delta || ! bin_hdr.SmpInt) {
      /* default the line header sample interval to depth interval */
      if (bin->cube.axis[1].delta <= 32) {
         bin_hdr.SmpInt = 1000 * bin->cube.axis[1].delta;
      } else {
         bin_hdr.SmpInt = bin->cube.axis[1].delta;
      }
   }

   /*******************************************************/
   /* Given a "format=" and media, determine segy flavor. */
   /*******************************************************/

   short_stype = DDS_STYPE_INTEGER2;
   int_stype   = DDS_STYPE_INTEGER4;
   green_stype = DDS_STYPE_INTEGER4;
   float_stype = DDS_STYPE_FLOAT4;

#if HOST_SWAP
   if (SU) {
      short_stype = DDS_STYPE_INTEGER2X;
      int_stype   = DDS_STYPE_INTEGER4X;
      green_stype = DDS_STYPE_INTEGER4X;
      float_stype = DDS_STYPE_FLOAT4X;
   }
#endif

   if(cdds_scank("format", "integer")) {
      sample_prec = 4;
      cdds_scanf("", "%*s %d", &sample_prec);
      if(sample_prec == 1) {
         bin_hdr.FmtCode = SEGY_FMTCODE_INTEGER1;
         samples_stype = DDS_STYPE_INTEGER1;
      } else if(sample_prec == 2) {
         bin_hdr.FmtCode = SEGY_FMTCODE_INTEGER2;
         samples_stype = DDS_STYPE_INTEGER2;
      } else if(sample_prec == 4) {
         bin_hdr.FmtCode = SEGY_FMTCODE_INTEGER4;
         samples_stype = DDS_STYPE_INTEGER4;
      } else if(sample_prec == 8) {
         bin_hdr.FmtCode = 0;
         samples_stype = DDS_STYPE_INTEGER8;
      } else {
         ddsWarn("Bogus segy format 'integer' precision=%i\n\tbinary= %s\n",
            sample_prec, bin->name);
         bin_hdr.FmtCode = SEGY_FMTCODE_INTEGER4;
         samples_stype = DDS_STYPE_INTEGER4;
      }
   } else {
      /* assume float sample type */
      sample_prec = 4;
      if(cdds_scank("format", "float")) {
         cdds_scanf("", "%*s %d", &sample_prec);
      }
      if(cdds_scank("format", "ieee")) {
         if(sample_prec == 8) {
            bin_hdr.FmtCode = 0;
            samples_stype = DDS_STYPE_FLOAT8;
         } else if(sample_prec == 4) {
            if (cdds_scank("format", "complex")) {
               bin_hdr.FmtCode = 0;
               samples_stype = DDS_STYPE_COMPLEX8;
               bin_hdr.NumSmp = 2*bin_hdr.NumSmp;
            } else {
               bin_hdr.FmtCode = SEGY_FMTCODE_FLOAT4;
               samples_stype = DDS_STYPE_FLOAT4;
            }
         } else {
            ddsWarn("Bogus segy format 'ieee' precision=%i\n\tbinary= %s\n",
               sample_prec, bin->name);
            bin_hdr.FmtCode = SEGY_FMTCODE_FLOAT4;
            samples_stype = DDS_STYPE_FLOAT4;
         }
      } else { /* assume ibm (ieee for SU) */
         if(sample_prec == 8) {
            bin_hdr.FmtCode = 0;
            if (SU) {
#if HOST_SWAP
               samples_stype = DDS_STYPE_FLOAT8X;
#else
               samples_stype = DDS_STYPE_FLOAT8;
#endif
            } else {
               samples_stype = DDS_STYPE_FLOAT8IBM;
            }
         } else if(sample_prec == 4) {
            if (cdds_scank("format", "complex")) {
               bin_hdr.FmtCode = 0;
               if (SU) {
#if HOST_SWAP
               samples_stype = DDS_STYPE_COMPLEX8X;
#else
               samples_stype = DDS_STYPE_COMPLEX8;
#endif
               } else {
                  samples_stype = DDS_STYPE_COMPLEX8IBM;
               }
               bin_hdr.NumSmp = 2*bin_hdr.NumSmp;
            } else {
               if (SU) {
                  bin_hdr.FmtCode = SEGY_FMTCODE_FLOAT4;
#if HOST_SWAP
                  samples_stype = DDS_STYPE_FLOAT4X;
#else
                  samples_stype = DDS_STYPE_FLOAT4;
#endif
               } else {
                  bin_hdr.FmtCode = SEGY_FMTCODE_FLOAT4IBM;
                  samples_stype = DDS_STYPE_FLOAT4IBM;
               }
            }
         } else {
            ddsWarn("Bogus segy format 'ibm' precision=%i\n\tbinary= %s\n",
               sample_prec, bin->name);
            bin_hdr.FmtCode = SEGY_FMTCODE_FLOAT4IBM;
            samples_stype = DDS_STYPE_FLOAT4IBM;
         }
      }
   }
 
   if(cdds_scank("format", "ascii")) {
      char_stype = DDS_STYPE_ASCII1;
   } else {
      char_stype = DDS_STYPE_EBCDIC1;
   }

   if(cdds_scank("format", "green_prefix")) {
      green_prefix_prec = dds_prime[green_stype].prec;
   } else {
      green_prefix_prec = 0;
   }
   if(cdds_scank("format", "green_suffix")) {
      green_suffix_prec = green_prefix_prec = dds_prime[green_stype].prec;
   } else {
      green_suffix_prec = 0;
   }

   if (green_prefix_prec) {
      bin->state  |= DDS_green_prefix;
   }
   if (green_suffix_prec) {
      bin->state  |= DDS_green_suffix;
   }

   sample_prec = dds_prime[samples_stype].prec;

   green_card   = SEGY_CARD_MIN;
   green_binary = SEGY_BINARY_MIN;
   green_trace  = SEGY_TRACE_MIN + bin->cube.axis[1].size * sample_prec;

   /*********************************************/
   /* create the new segy card & binary headers */
   /*********************************************/

   card_buf = ddsZalloc(SEGY_CARD_MAX);
   binary_buf = ddsZalloc(SEGY_BINARY_MAX);

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

   if(ddsCompileFmt(bin, DDS_TRUE)) return;

   ddsUncompileFmt(bin);

   cdds_dict(bin->dict_open, "print");

   /*****************************************/
   /* link bin struct to card header fields */
   /*****************************************/

   if(ddsObjectTag(&bin->std.segy.card, bin->block_new, "PREAMBLE") || 
      bin->std.segy.card.type->ptype != DDS_STYPE_STRUCT ||
      ddsLinkObject(&bin->std.segy.card)) {
      ddsError("Bogus segy format card declaration\n\tbinary= %s\n",
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

   /******************************/
   /* update symbol table values */
   /******************************/

   type = bin->std.segy.card.type;
   type->stroke = green_prefix_prec + green_card + green_suffix_prec;
   type->prec = green_prefix_prec + green_card + green_suffix_prec;
   bin->std.segy.card_buf = ddsRealloc(card_buf, type->stroke);

   type = bin->std.segy.binary.type;
   type->stroke = green_prefix_prec + green_binary + green_suffix_prec;
   type->prec = green_prefix_prec + green_binary + green_suffix_prec;
   bin->std.segy.binary_buf = ddsRealloc(binary_buf, type->stroke);

   /****************/
   /* sanity check */
   /****************/

   type = bin->std.segy.card.type;
   if(green_card != type->stroke - green_prefix_prec - green_suffix_prec) {
      ddsError("SEGY card green word= %i, expected %i\n\tbinary= %s\n",
         (int)green_card, 
         (int)(type->stroke - green_prefix_prec - green_suffix_prec), 
         bin->name);
   }

   type = bin->std.segy.binary.type;
   if(green_binary != type->stroke - green_prefix_prec - green_suffix_prec) {
      ddsError("SEGY binary green word= %i, expected %i\n\tbinary= %s\n",
         (int)green_binary, 
         (int)(type->stroke - green_prefix_prec - green_suffix_prec), 
         bin->name);
   }

   type = bin->trace.type;
   member = bin->Samples.member;
   if(member->offset != SEGY_TRACE_MIN + green_prefix_prec) {
      ddsError("SEGY trace Samples offset= %lu, expected %i\n\tbinary= %s\n",
         (unsigned long)member->offset,
         (int)(SEGY_TRACE_MIN + green_prefix_prec), bin->name);
   } else if(green_trace != type->stroke - green_prefix_prec 
         - green_suffix_prec) {
      ddsError("SEGY trace green word= %i, expected %i\n\tbinary= %s\n",
         (int)green_trace, 
         (int)(type->stroke - green_prefix_prec - green_suffix_prec), 
         bin->name);
   }

   /*******************************************************/
   /* PUT header values into binary buffer.               */
   /*                                                     */
   /* The bin_hdr value is put into the binary header     */
   /* referenced by hdr_buf.                              */
   /*                                                     */
   /* API tags are created for each member and the offset */
   /* to each member in the buffer is returned in "bias". */
   /*******************************************************/

   bin->std.segy.card_green_min = SEGY_CARD_MIN;
   bin->std.segy.card_green_max = SEGY_CARD_MAX;
   bin->std.segy.binary_green_min = SEGY_BINARY_MIN;
   bin->std.segy.binary_green_max = SEGY_BINARY_MAX;

   /* setup implicit arguments used by HDR_PUT */
   hdr_buf = bin->std.segy.card_buf;
   type = bin->std.segy.card.type;
   hdr_block = type->block;
 
   HDR_PUT_CHAR("", card3200)

   /* setup implicit arguments used by HDR_PUT */
   hdr_buf = bin->std.segy.binary_buf;
   type = bin->std.segy.binary.type;
   hdr_block = type->block;
 
   HDR_PUT_INT("", JobNum)
   HDR_PUT_INT("", LineNum)
   HDR_PUT_INT("", ReelNum)
   HDR_PUT_INT("", NumTrc)
   HDR_PUT_INT("", NumAux)
   HDR_PUT_INT("", SmpInt)
   HDR_PUT_INT("", OrigSmpInt)
   HDR_PUT_INT("", NumSmp)
   HDR_PUT_INT("", OrigNumSmp)
   HDR_PUT_INT("", FmtCode)
   HDR_PUT_INT("", CdpFold)
   HDR_PUT_INT("", SortCode)
   HDR_PUT_INT("", VertSumFlag)
   HDR_PUT_INT("", SwpFreqStart)
   HDR_PUT_INT("", SwpFreqEnd)
   HDR_PUT_INT("", SwpLength)
   HDR_PUT_INT("", SwpCode)
   HDR_PUT_INT("", SwpChan)
   HDR_PUT_INT("", SwpTprStart)
   HDR_PUT_INT("", SwpTprEnd)
   HDR_PUT_INT("", SwpTprType)
   HDR_PUT_INT("", CorrFlag)
   HDR_PUT_INT("", GainFlag)
   HDR_PUT_INT("", AmpFlag)
   HDR_PUT_INT("", UnitsFlag)
   HDR_PUT_INT("", PolarityFlag)
   HDR_PUT_INT("", VibLagFlag)
   HDR_PUT_INT("SpareBin.", SpareBinI2)
   HDR_PUT_INT("", SegyFmt)
   HDR_PUT_INT("", FixedLength)
   HDR_PUT_INT("", NumHdr)
   HDR_PUT_INT("SpareBin.", Spare2BinI2)

   /*********************************************************/
   /* allocate buffer for one dead trace, and initialize it */
   /*********************************************************/

   type = bin->trace.type;
   bin->trace_buf = ddsZalloc(type->stroke);

   if(dds_state & DDS_DUMP_DIFF) {
      bin->diff1_buf = ddsMalloc(type->stroke);
      memset(bin->diff1_buf, DDS_NON_CHAR, type->stroke);
      bin->diff2_buf = ddsMalloc(type->stroke);
      memset(bin->diff2_buf, DDS_NON_CHAR, type->stroke);
   }

   bin->dead_trace = ddsZalloc(type->stroke);

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

   /********************************************************************/
   /* Save Sample interval in overloaded sample_prec for ddsWriteSegyT */
   /* This had only been used for reading existing segy files.         */
   /********************************************************************/

   bin->std.segy.sample_prec = bin_hdr.SmpInt;

   /**************************************/
   /* restore the su format from here on */
   /**************************************/

   if (SU) {
      bin->fmt == DDS_FMT_SU;
      bin->fmt_name = ddsString("su");
   }
}
