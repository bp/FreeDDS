static char rcsid[]="$Id: autofmt.c 156 2010-06-15 20:31:50Z user $";
/*
RCS-t=DDS, automatic format detection.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, xx May 1996            ***/
/**********************************************/

#include <sys/types.h>
#if defined(_WIN32) || defined(MINGW)
#include <conio.h>
#else
#include <sys/ioctl.h>
#endif


#include "dds_sys.h"

#include "ascii_ebcdic.h"

#define BUF_SIZE  100000

static int SegyFmt;


static int is_oslabel(
   int nbytes,
   char buf[])
{
   char lbuf[4];

   if (nbytes != 80) return(DDS_FALSE);

   /* Check for IBM Labels */

   if (CON_AAVV(DDS_CTYPE_CHAR, DDS_STYPE_EBCDIC1, lbuf, buf,4) 
       != DDS_CON_OK) return(DDS_FALSE);

   if (!memcmp(buf, "VOL1",4) || !memcmp(lbuf, "VOL1",4) ||
       !memcmp(buf, "HDR1",4) || !memcmp(lbuf, "HDR1",4) ||
       !memcmp(buf, "HDR2",4) || !memcmp(lbuf, "HDR2",4))
      return(DDS_TRUE);

   return(DDS_FALSE);
}


static int is_segylabel(
   int nbytes,
   char buf[])
{
   if (nbytes != 128) return(DDS_FALSE);

   /* Check for SEGY Label */

   if (!memcmp(&buf[4], "SY1.0RECORD",11)) return(DDS_TRUE);

   return(DDS_FALSE);
}


static int check_oslabel(
   int nbytes,
   char buf[])
{
   char lbuf[240];

   if (nbytes < 240) return(DDS_FALSE);

   /* Check for IBM Labels */

   if (CON_AAVV(DDS_CTYPE_CHAR, DDS_STYPE_EBCDIC1, lbuf, buf, 240)
       != DDS_CON_OK) return(DDS_FALSE);

   if (memcmp(&buf[0],  "VOL1",4) && memcmp(&lbuf[0],  "VOL1",4))
      return(DDS_FALSE);
   if (memcmp(&buf[80], "HDR1",4) && memcmp(&lbuf[80], "HDR1",4))
      return(DDS_FALSE);
   if (memcmp(&buf[160],"HDR2",4) && memcmp(&lbuf[160],"HDR2",4))
      return(DDS_FALSE);

   return(DDS_TRUE);
}


static int check_vds(
   int nbytes,
   char buf[])
{
   if (nbytes < 36) return(DDS_FALSE);

   /* Check for Disco (vds)   */

   if (memcmp(&buf[0],  "DIGICON SFL     ", 16)) return(DDS_FALSE);
   if (memcmp(&buf[20], "RSF_FILE        ", 16)) {
      ddsPrintStr("ddsAutoFmt: Disco file, but not 'RFS' type!\n");
      return(DDS_FALSE);
   }

   return(DDS_TRUE);
}


static int check_tapout(
   int nbytes,
   char buf[])
{
   if (nbytes < 280) return(DDS_FALSE);

   /* Check for Disco Internal Format (TAPOUT)  */

   if (!memcmp(&buf[  0], "ID      DISCO INTERNAL FORMAT   ", 32)) 
      return(DDS_TRUE);

   if (!memcmp(&buf[240], "ID      DISCO INTERNAL FORMAT   ", 32)) 
      return(DDS_TRUE);

   if (!memcmp(&buf[248], "ID      DISCO INTERNAL FORMAT   ", 32)) 
      return(DDS_TRUE);

   return(DDS_FALSE);
}


static int check_generic(
   int nbytes,
   char buf[])
{
   int i, c, quote;
   DDS_BOOLEAN name = DDS_FALSE;   /* true, iff 1st name started */
   DDS_BOOLEAN equal = DDS_FALSE;  /* true, iff 1st defn found */

   /*************************/
   /* Check Text dictionary */
   /*********************************************************/
   /* 21 July 1998 RLS, change dictionary test logic.       */
   /* Note: the old logic was flawed in two ways...         */
   /* It used "isprint" (which excludes newline, etc.).     */
   /* It assumed data and format were defined, which is     */
   /* wrong for many dictionaries (parameter files, etc.)   */
   /*                                                       */
   /* Dictionaries may be completely empty, in which case   */
   /* the read should return zero bytes and the ddsAutoFmt  */
   /* should return false with fmt set to DDS_FMT_UNKNOWN.  */
   /*                                                       */
   /* If it contains any characters (up to DDS_EOD_CHAR)    */
   /* they must all pass isgraph or isspace tests.          */
   /*                                                       */
   /* To avoid being fooled by a segy ascii line header and */
   /* perhaps other text file, the first non-white stuff    */
   /* must look like a definition, i.e. "name<opt-white>=". */
   /*********************************************************/
   quote = 0;
   for(i=0;i<nbytes;i++) {
      c = 0377 & buf[i];

      if(c == DDS_EOD_CHAR) {
         /* data attached to dict, iff defn was found. */
         return equal;
      }

      if (c == '"') quote = !quote;

      if(!isprint(c) && !isspace(c) && !quote) {
         return DDS_FALSE; /* gunk */
      }

      if(equal) continue;  /* 1st defn already found */

      /* continue search for first defn, i.e. "name= ..." */

      if(isgraph(c)) {
         /* non-white char found */
         if(! name) {
            /* first non-white char found (name ?) */
            name = DDS_TRUE; /* first char in name ? */
         } else if(c == '=') {
            equal = DDS_TRUE;  /* "name= ..." found ! */
         } else if(isspace(0377 & buf[i - 1])) {
            /* "<white-space>z" found, as in "xxx<white-space>z" */
            /* It wasn't really a definition, search for new one */
            /*  return DDS_FALSE; */
            name = DDS_FALSE;
         }
      }
   }

   if(name) {
      /* dictionary, iff defn was found. */
      return equal;
   } else if(nbytes < 100) {
      /* small dictionary only containing white space ? */
      return DDS_TRUE;
   }

   return DDS_FALSE;
}


static int check_usp(
   int  nbytes,
   char buf[],
   int  tape,
   int  swap,
   int  grn_prec)
{
   int green, loc;
   int LineMin, LineMax, LineLen, TrcMin, TrcLen;
   int NumTrc, NumTrcLoc;
   int NumSmp, NumSmpLoc;
   int FmtCod, FmtCodLoc;
   int HlhByt, HlhBytLoc;
   int int_prec, short_prec;
   DDS_PTYPE int_type, short_type;

   /* Check Line Header */

   if (!tape && grn_prec <= 0) return(DDS_FALSE);
   LineMin    = 1004;
   LineMax    = 32758;
   TrcMin     = 256;
   NumTrcLoc  = grn_prec + 48;
   NumSmpLoc  = grn_prec + 60;
   FmtCodLoc  = grn_prec + 64;
   HlhBytLoc  = grn_prec + 1002;
   int_prec   = 4;
   short_prec = 2;
   if (swap) {
      int_type = DDS_STYPE_UNSIGNED4X;
      short_type  = DDS_STYPE_UNSIGNED2X;
   } else {
      int_type = DDS_STYPE_UNSIGNED4;
      short_type  = DDS_STYPE_UNSIGNED2;
   }

   if (tape) {
      if (nbytes < LineMin+grn_prec) return(DDS_FALSE);
   } else {
      if (nbytes < LineMin+TrcMin+2*grn_prec) return(DDS_FALSE);
   }

   if (CON_ANSS(DDS_CTYPE_INT, int_type, &NumTrc, &buf[NumTrcLoc]) 
       != DDS_CON_OK) return(DDS_FALSE);
   if (CON_ANSS(DDS_CTYPE_INT, int_type, &NumSmp, &buf[NumSmpLoc])
       != DDS_CON_OK) return(DDS_FALSE);
   if (CON_ANSS(DDS_CTYPE_INT, short_type, &FmtCod, &buf[FmtCodLoc]) 
       != DDS_CON_OK) return(DDS_FALSE);
   if (CON_ANSS(DDS_CTYPE_INT, short_type, &HlhByt, &buf[HlhBytLoc]) 
       != DDS_CON_OK) return(DDS_FALSE);

   if (NumTrc < 1) return(DDS_FALSE);
   if (NumSmp < 1) return(DDS_FALSE);
   if (FmtCod < 0 || FmtCod >      9) return(DDS_FALSE);
   if (HlhByt < 4) return(DDS_FALSE);
   
   /* Check green words */

   LineLen = LineMin + HlhByt - 2*short_prec;
   if (tape) {
      if (LineLen + grn_prec != nbytes) return(DDS_FALSE);
   } else {
      if (LineLen > LineMax) return(DDS_FALSE);
      if (LineLen < LineMin) return(DDS_FALSE);
   }
   if (grn_prec) {
      if (CON_ANSS(DDS_CTYPE_INT, int_type, &green, buf) 
          != DDS_CON_OK) return(DDS_FALSE);
      if (green != LineLen) return(DDS_FALSE);

      if (!tape) {
         TrcLen = TrcMin + NumSmp*int_prec;
         loc = grn_prec + LineLen;
         if (loc+grn_prec >= nbytes) return(DDS_FALSE);
         if (CON_ANSS(DDS_CTYPE_INT, int_type, &green, &buf[loc]) 
             != DDS_CON_OK) return(DDS_FALSE);
         if (green != TrcLen) return(DDS_FALSE);
      }
   }

   return(DDS_TRUE);
}


static int check_segy(
   int  nbytes,
   char buf[],
   int  tape,
   int  grn1_prec,
   int  grn2_prec)
{
   float percent_ascii, percent_ebcdic;
   int   i, n, loc, prec, green;
   int   LineSize, BinSize, TrcMin, TrcLen;
   int   NumTrc, NumTrcLoc;
   int   NumSmp, NumSmpLoc;
   int   FmtCod, FmtCodLoc;
   int   SegyFmtLoc, int_prec;
   DDS_PTYPE int_type, short_type;

   /* Check Line Header */

   LineSize  = 3200;
   BinSize   = 400;
   TrcMin    = 240;
   NumTrcLoc = 2 * grn1_prec + LineSize + grn2_prec + 12;
   NumSmpLoc = 2 * grn1_prec + LineSize + grn2_prec + 20;
   FmtCodLoc = 2 * grn1_prec + LineSize + grn2_prec + 24;
   SegyFmtLoc= 2 * grn1_prec + LineSize + grn2_prec + 300;
   int_prec  = 4;
   int_type  = DDS_STYPE_UNSIGNED4;
   short_type= DDS_STYPE_UNSIGNED2;

   if (tape) {
      if (nbytes != LineSize+grn1_prec+grn2_prec) return(DDS_FALSE);
   } else {
      n = LineSize+BinSize+TrcMin+3*(grn1_prec+grn2_prec);
      if (nbytes < n) return(DDS_FALSE);
   }

   /* Check Text Header */

   percent_ascii = 0.0;
   for (i=grn1_prec;i<grn1_prec+LineSize;i++) {
      if (isprint(0377 & buf[i]) || isspace(0377 & buf[i])) {
         percent_ascii += 1.0;
      }
   }

   percent_ebcdic = 0.0;
   for (i=grn1_prec;i<grn1_prec+LineSize;i++) {
      if (isprint(ebcdic_to_ascii(buf[i]))) percent_ebcdic += 1.0;
   }

   percent_ascii /= LineSize;
   percent_ebcdic /= LineSize;

   if(percent_ascii < 0.8 && percent_ebcdic < 0.8) return(DDS_FALSE);
   
   /* Check green word for tape  */

   if (tape) {
      if (grn1_prec) {
         if (CON_ANSS(DDS_CTYPE_INT, int_type, &green, buf) 
             != DDS_CON_OK) return(DDS_FALSE);
         if (green != LineSize) return(DDS_FALSE);

         if (grn2_prec) {
            loc = grn1_prec + LineSize;
            if (CON_ANSS(DDS_CTYPE_INT, int_type, &green, buf) 
                != DDS_CON_OK) return(DDS_FALSE);
            if (green != LineSize) return(DDS_FALSE);
         }
      }
      return(DDS_TRUE);
   }

   /* Check Binary Header  */

   if (CON_ANSS(DDS_CTYPE_INT, short_type, &NumTrc, &buf[NumTrcLoc])
       != DDS_CON_OK) return(DDS_FALSE);
   if (CON_ANSS(DDS_CTYPE_INT, short_type, &NumSmp, &buf[NumSmpLoc])
       != DDS_CON_OK) return(DDS_FALSE);
   if (CON_ANSS(DDS_CTYPE_INT, short_type, &FmtCod, &buf[FmtCodLoc])
       != DDS_CON_OK) return(DDS_FALSE);
   if (CON_ANSS(DDS_CTYPE_INT, short_type, &SegyFmt, &buf[SegyFmtLoc])
       != DDS_CON_OK) return(DDS_FALSE);
   
   if (NumTrc < 1) return(DDS_FALSE);
   if (NumSmp < 1) return(DDS_FALSE);
   if (FmtCod < 0 || FmtCod > 9) return(DDS_FALSE);
 
   switch(FmtCod) {
   case 1:
   case 2:
   case 4:
   case 5:
      prec = 4;
      break;
   case 3:
      prec = 2;
      break;
   case 7:
   case 8:
      prec = 1;
      break;
   default:
      prec = int_prec;
      break;
   }
   
   /* Check green words */

   TrcLen = TrcMin + NumSmp * prec;
   if (grn1_prec) {
      if (CON_ANSS(DDS_CTYPE_INT, int_type, &green, buf)
          != DDS_CON_OK) return(DDS_FALSE);
      if (green != LineSize) return(DDS_FALSE);
      loc = grn1_prec + LineSize + grn2_prec;
      if (loc+grn1_prec >= nbytes) return(DDS_FALSE);
      if (CON_ANSS(DDS_CTYPE_INT, int_type, &green, &buf[loc])
          != DDS_CON_OK) return(DDS_FALSE);
      if (green != BinSize) return(DDS_FALSE);
      loc = loc + grn1_prec + BinSize + grn2_prec;
      if (loc+grn1_prec >= nbytes) return(DDS_FALSE);
      if (CON_ANSS(DDS_CTYPE_INT, int_type, &green, &buf[loc])
          != DDS_CON_OK) return(DDS_FALSE);
      if (green != TrcLen) return(DDS_FALSE);

      if (grn2_prec) {
         loc = grn1_prec + LineSize;
         if (CON_ANSS(DDS_CTYPE_INT, int_type, &green, buf)
             != DDS_CON_OK) return(DDS_FALSE);
         if (green != LineSize) return(DDS_FALSE);
         loc = loc + grn2_prec + grn1_prec + BinSize;
         if (CON_ANSS(DDS_CTYPE_INT, int_type, &green, &buf[loc])
             != DDS_CON_OK) return(DDS_FALSE);
         if (green != BinSize) return(DDS_FALSE);
         loc = loc + grn2_prec + grn1_prec + BinSize;
         if (loc+grn2_prec >= nbytes) return(DDS_FALSE);
         if (CON_ANSS(DDS_CTYPE_INT, int_type, &green, &buf[loc])
             != DDS_CON_OK) return(DDS_FALSE);
         if (green != TrcLen) return(DDS_FALSE);
      }
   }

   return(DDS_TRUE);
}


static int check_su(
   int nbytes,
   char buf[])
{
   int ofst, test, prec;
   int TrcMin, TrcLen;
   int SmpInt, SmpIntL;
   int NumSmp, NumSmpL;
   int TrcIdCode, TrcIdCodeL;
   int TrcSeqNum, TrcSeqNumL;
   int FieldRecNum, FieldRecNumL;
   int FieldTrcNum, FieldTrcNumL;
   DDS_PTYPE short_type,long_type;

   prec = 4;
   TrcMin = 240;
   SmpIntL = 116;
   NumSmpL = 114;
   TrcIdCodeL = 28;
   TrcSeqNumL = 0;
   FieldRecNumL = 8;
   FieldTrcNumL = 12;
#if HOST_SWAP
   short_type = DDS_STYPE_INTEGER2X;
#else
   short_type = DDS_STYPE_INTEGER2;
#endif
#if HOST_SWAP
   long_type = DDS_STYPE_INTEGER4X;
#else
   long_type = DDS_STYPE_INTEGER4;
#endif

   /* Check Binary Header  */

   if (CON_ANSS(DDS_CTYPE_INT, short_type, &SmpInt, &buf[SmpIntL])
       != DDS_CON_OK) return(DDS_FALSE);
   if (CON_ANSS(DDS_CTYPE_INT, short_type, &NumSmp, &buf[NumSmpL])
       != DDS_CON_OK) return(DDS_FALSE);
   if (CON_ANSS(DDS_CTYPE_INT, short_type, &TrcIdCode, &buf[TrcIdCodeL])
       != DDS_CON_OK) return(DDS_FALSE);
   if (CON_ANSS(DDS_CTYPE_INT, long_type, &TrcSeqNum, &buf[TrcSeqNumL])
       != DDS_CON_OK) return(DDS_FALSE);
   if (CON_ANSS(DDS_CTYPE_INT, long_type, &FieldRecNum, &buf[FieldRecNumL])
       != DDS_CON_OK) return(DDS_FALSE);
   if (CON_ANSS(DDS_CTYPE_INT, long_type, &FieldTrcNum, &buf[FieldTrcNumL])
       != DDS_CON_OK) return(DDS_FALSE);
   
   /* Start with basic sanity checks */

   if (TrcIdCode > 32767) return(DDS_FALSE);
   if (TrcSeqNum < 0) return(DDS_FALSE);
   if (FieldRecNum < 1) return(DDS_FALSE);
   if (FieldTrcNum < 1) return(DDS_FALSE);
   if (SmpInt < 1) return(DDS_FALSE);
   if (NumSmp < 1) return(DDS_FALSE);
   
   /* Check green words */

   TrcLen = TrcMin + NumSmp * prec;

   /* This is to try and detect false-positives that dodge the second header tests */

   if (nbytes > TrcLen && nbytes < 2*TrcLen) return(DDS_FALSE);

   for(ofst=TrcLen;ofst+TrcLen<nbytes;ofst+=TrcLen) {
      if (CON_ANSS(DDS_CTYPE_INT, short_type, &test, &buf[ofst+SmpIntL])
          != DDS_CON_OK) return(DDS_FALSE);
      if (test != SmpInt) return(DDS_FALSE);
      if (CON_ANSS(DDS_CTYPE_INT, short_type, &test, &buf[ofst+NumSmpL])
          != DDS_CON_OK) return(DDS_FALSE);
      if (test != NumSmp) return(DDS_FALSE);
   }

   return(DDS_TRUE);
}


int ddsAutoFmt(
   DDS_AUTO_FMT *AutoFmt,
   GFILE *fp)
{
   int      nbytes, tape, swap, c;
   char     buf[BUF_SIZE+1];

   /* Initialize defaults  */

   AutoFmt->fmt   = DDS_FMT_UNKNOWN;
   AutoFmt->flag  = (DDS_AUTO_FLAG)0;
   AutoFmt->prec  = sizeof(float);
   AutoFmt->genus = DDS_TYPE_FLOATS;

   /* Read a large buffer  */

        /* non-buffered gunread only supports *one* physical   */
        /* record of push back.  For example, if N reads are   */
        /* performed resulting in M total bytes being un-read, */
        /* then the next read will have one opportunity to     */
        /* consume a maximum of M bytes (N - 1 reads lost).    */

   nbytes = greadc(fp, buf, BUF_SIZE);
   if (nbytes <= 0) return(DDS_FALSE);

   tape = (fp->genus == GIO_MEDIA_GENUS_TAPE);
   if (tape) {
      while (is_oslabel(nbytes, buf) || is_segylabel(nbytes, buf)) {
         nbytes = greadc(fp, buf, BUF_SIZE);
         if (fp->error) break;
      }
      if (!nbytes) nbytes = greadc(fp, buf, BUF_SIZE);
   } 
   gunread(fp, buf, nbytes);

   /* Check O/S Label      */
   if (check_oslabel(nbytes, buf)) {
      nbytes -= 240;
      memmove(buf, &buf[240], nbytes);
   }

   /* Check SEGY Label     */
   if (is_oslabel(nbytes, buf)) {
      nbytes -= 128;
      memmove(buf, &buf[128], nbytes);
   }

   /* Check Generic  */
   if (check_generic(nbytes, buf)) {
      AutoFmt->fmt = DDS_FMT_GENERIC;
      return(DDS_TRUE);
   }

   /* Check Disco */
   if (check_vds(nbytes, buf) || check_tapout(nbytes, buf)) {
      /* AutoFmt->fmt = DDS_FMT_DISCO; */
      /* return(DDS_TRUE); */
      ddsWarn("ddsAutoFmt: ignoring obsolete Disco format for file %s\n",
         fp->cfd.name); 
   }

   /* Check Usp   */
#ifdef HOST_SWAP
   swap = (fp->genus == GIO_MEDIA_GENUS_FIFO);
#else
   swap = 0;
#endif
   if (check_usp(nbytes, buf, tape, swap, 4) ||
       check_usp(nbytes, buf, tape, swap, 0)) {
      AutoFmt->fmt = DDS_FMT_USP;
      return(DDS_TRUE);
   }

   /* Check Segy & Segy Rev. 1 */
   if (check_segy(nbytes, buf, tape, 4, 4) ||
       check_segy(nbytes, buf, tape, 4, 0) ||
       check_segy(nbytes, buf, tape, 0, 0)) {
      if (SegyFmt == 0x0100) {
         AutoFmt->fmt  = DDS_FMT_SEGY1;
      } else {
         AutoFmt->fmt  = DDS_FMT_SEGY;
      }
      return(DDS_TRUE);
   }

   /* Check SU (Segy w/o card & binary headers) */
   if (check_su(nbytes, buf)) {
      AutoFmt->fmt  = DDS_FMT_SU;
      return(DDS_TRUE);
   }
      

   /* UNKNOWN!!   */
   AutoFmt->fmt  = DDS_FMT_UNKNOWN;
   AutoFmt->flag = DDS_AUTO_UNKNOWN;

   return(DDS_FALSE);
}

#ifdef TEST

MAIN_() {}

main(
   int argc,
   char **argv)
{
   GFILE    *fp;
   DDS_AUTO_FMT   AutoFmt;

   if (argc<2) exit(1);

   printf("Checking file: %s\n", argv[1]);
   if (!strcmp(argv[1], "stdin:")) fp = gstdin;
   else fp = gopen(argv[1], "r");
   if (!fp) exit(9);

   cdds_init();

   if (!ddsAutoFmt(&AutoFmt, fp)) 
   {
      printf("Unable to detect format!\n");
   }
   
   switch(AutoFmt.fmt)
   {
   case DDS_FMT_GENERIC:
      printf("DDS Dictionary\n");
      break;
   case DDS_FMT_DISCO:
      printf("Disco\n");
      break;
   case DDS_FMT_USP:
      printf("Usp\n");
      break;
   case DDS_FMT_SEGY:
      printf("Segy\n");
      break;
   case DDS_FMT_SU:
      printf("su\n");
      break;
   default:
      printf("Unknown format\n");
      exit(9);
      break;
   }
   
}
#endif
