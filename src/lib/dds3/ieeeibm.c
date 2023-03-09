static char rcsid[]="$Id: ieeeibm.c 150 2010-04-30 20:19:35Z user $";
/*
RCS-t=DDS, convert IEEE short floating point to ibm short floating point.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

/* Stolen 3/2/94 by R. Selzler from /net/gpsol/pro1/iqc/src/Libqc/sunibm.c.
   Changed function names to DDS convention.
   Changed "long" to typedef for IEEE and IBM values (portable).
   Stolen again 5/16/94, for overflow checks and double precision support
      enhancements made by Ray Early
   Kludge to work with swapped data by Jerry Ehlers 10/2000
   Added IBMIEEX & IEEEXIBM by Jerry Ehlers 7/2006
*/
#include "dds_sys.h"
#include "dds_swap.h"


int ddsIBMIEEE(
   DDS_IBM4_OUT *out,
   DDS_IEEE4_IN *inp,
   int nsm)
{
   int i, loop, savsig;
   unsigned iexp, iexpsv, mant;
   register DDS_IEEE4_IN *rinp;
   register DDS_IBM4_OUT *rout;

#ifdef HOST_SWAP 
   ddsSWAP4X(out, inp, nsm);
   rinp = out;
#else
   rinp = inp;
#endif
   rout = out;
   loop = nsm;

   for (i=0;i<loop;i++) {
      if (*rinp == 0) {
         *rout++ = 0;
         rinp++;
      } else {
         savsig = *rinp & 0x80000000;	/* save sign */
         iexp = *rinp & 0x7f800000;
         mant = (*rinp & 0x007fffff) | 0x00800000;
         iexp >>= 23;
         iexp = iexp - 126;
         iexpsv = iexp & 0x00000003;
         if (iexpsv != 0) {
            iexpsv = 4 - iexpsv;
            mant >>= iexpsv;
            iexp = iexp + 4;
         }
         iexp >>= 2;
         iexp = iexp + 64;
         iexp <<= 24;
         *rout++ = (savsig | mant) | iexp;
         rinp++;
      }
   }
#ifdef HOST_SWAP 
   ddsSWAP4X(out, out, nsm);
#endif
   return (0);
}


int ddsIEEEIBM(
   DDS_IEEE4_OUT *out,   /* may be same as inp */
   DDS_IBM4_IN *inp,
   int nsm)
{
   union {
      float fval;
      unsigned  man;
   } ibm;
   int bmove, loop, i;
   unsigned bias;
   register DDS_IBM4_IN *rinp;
   register DDS_IEEE4_OUT *rout;

#ifdef HOST_SWAP 
   ddsSWAP4X(out, inp, nsm);
   rinp = out;
#else
   rinp = inp;
#endif
   rout = out;
   loop = nsm;

   for (i=0;i<loop;i++) {
      /* check for 0 value and return 0 */
      if (*rinp == 0) {
         *rout++ = 0;
         rinp++;
      } else {

         /*
          * strip off the mantissa from the ibm floating point
          */
         ibm.man = *rinp & 077777777;

         /*
          * move the first non zero bit of the mantissa to the most
          * significant bit position of the mantissa. Then remove the
          * hiden bit (the most significant bit).
          */
         for (bmove = 1; bmove < 25; bmove++) {
            if ((ibm.man & ~037777777) > 0)
               break;
            ibm.man <<= 1;
         }
         ibm.man = ibm.man & ~040000000;

         /*
          * strip off the bias from the ibm floating point. Then mask the
          * last 8 bits as the the ibm bias.
          */
         bias = *rinp >> 24;
         bias = bias & 0177;

         /*
          * check for overflow condition... ibm can represent larger number
          * than ieee so if overflow, clip to largest ieee value
          */

         if (bias > 96) {
            *rout++ = 0x7f7fffff;	/* max norm flt pt val (infinity - 1) */
            rinp++;
            continue;
         }

         /*
          * check for underflow condition ... ibm can represent smaller
          * number than ieee so if underflow, set to 0
          */
         if (bias <= 32) {
            *rout++ = 0;
            rinp++;
            continue;
         }

         /*
          * compute the sun bias. convert base 16 to base 2 by multiple 
          * by 4. substract the number of moves to the left by mantissa,
          * i.e. one move to left reduce the power by 1 for base 2. Add 
          * the base sun based power of 127.
          */
         bias = (((bias - 64) * 4) - bmove) + 127;
         bias <<= 23;

         /*
          * map the mantissa and bias together. Then add the sign bit.
          */
         ibm.man = ibm.man | bias;
         *rout++ = (020000000000 & *rinp) | ibm.man;
         rinp++;
      }
   }
#ifdef HOST_SWAP 
   ddsSWAP4X(out, out, nsm);
#endif
   return (0);
}


int ddsIBMIEEEX(
   DDS_IBM4_OUT *out,
   DDS_IEEE4_IN *inp,
   int nsm)
{
   int i, loop, savsig;
   unsigned iexp, iexpsv, mant;
   register DDS_IEEE4_IN *rinp;
   register DDS_IBM4_OUT *rout;

#ifndef HOST_SWAP 
   ddsSWAP4X(out, inp, nsm);
   rinp = out;
#else
   rinp = inp;
#endif
   rout = out;
   loop = nsm;

   for (i=0;i<loop;i++) {
      if (*rinp == 0) {
         *rout++ = 0;
         rinp++;
      } else {
         savsig = *rinp & 0x80000000;	/* save sign */
         iexp = *rinp & 0x7f800000;
         mant = (*rinp & 0x007fffff) | 0x00800000;
         iexp >>= 23;
         iexp = iexp - 126;
         iexpsv = iexp & 0x00000003;
         if (iexpsv != 0) {
            iexpsv = 4 - iexpsv;
            mant >>= iexpsv;
            iexp = iexp + 4;
         }
         iexp >>= 2;
         iexp = iexp + 64;
         iexp <<= 24;
         *rout++ = (savsig | mant) | iexp;
         rinp++;
      }
   }
#ifdef HOST_SWAP 
   ddsSWAP4X(out, out, nsm);
#endif
   return (0);
}


int ddsIEEEXIBM(
   DDS_IEEE4_OUT *out,   /* may be same as inp */
   DDS_IBM4_IN *inp,
   int nsm)
{
   union {
      float fval;
      unsigned  man;
   } ibm;
   int bmove, loop, i;
   unsigned bias;
   register DDS_IBM4_IN *rinp;
   register DDS_IEEE4_OUT *rout;

#ifdef HOST_SWAP 
   ddsSWAP4X(out, inp, nsm);
   rinp = out;
#else
   rinp = inp;
#endif
   rout = out;
   loop = nsm;

   for (i=0;i<loop;i++) {
      /* check for 0 value and return 0 */
      if (*rinp == 0) {
         *rout++ = 0;
         rinp++;
      } else {

         /*
          * strip off the mantissa from the ibm floating point
          */
         ibm.man = *rinp & 077777777;

         /*
          * move the first non zero bit of the mantissa to the most
          * significant bit position of the mantissa. Then remove the
          * hiden bit (the most significant bit).
          */
         for (bmove = 1; bmove < 25; bmove++) {
            if ((ibm.man & ~037777777) > 0)
               break;
            ibm.man <<= 1;
         }
         ibm.man = ibm.man & ~040000000;

         /*
          * strip off the bias from the ibm floating point. Then mask the
          * last 8 bits as the the ibm bias.
          */
         bias = *rinp >> 24;
         bias = bias & 0177;

         /*
          * check for overflow condition... ibm can represent larger number
          * than ieee so if overflow, clip to largest ieee value
          */

         if (bias > 96) {
            *rout++ = 0x7f7fffff;	/* max norm flt pt val (infinity - 1) */
            rinp++;
            continue;
         }

         /*
          * check for underflow condition ... ibm can represent smaller
          * number than ieee so if underflow, set to 0
          */
         if (bias <= 32) {
            *rout++ = 0;
            rinp++;
            continue;
         }

         /*
          * compute the sun bias. convert base 16 to base 2 by multiple 
          * by 4. substract the number of moves to the left by mantissa,
          * i.e. one move to left reduce the power by 1 for base 2. Add 
          * the base sun based power of 127.
          */
         bias = (((bias - 64) * 4) - bmove) + 127;
         bias <<= 23;

         /*
          * map the mantissa and bias together. Then add the sign bit.
          */
         ibm.man = ibm.man | bias;
         *rout++ = (020000000000 & *rinp) | ibm.man;
         rinp++;
      }
   }
#ifndef HOST_SWAP 
   ddsSWAP4X(out, out, nsm);
#endif
   return (0);
}


/*
 * Convert double precision ibm floating point to double precision ieee
 * floating point
 */
int ddsIEEEIBM8(
   DDS_IEEE8_OUT *out, /* may be same as inp */
   DDS_IBM8_IN *inp,
   int nsm)
{
   union {
      double fval;
      unsigned man[2];
   } ibm;
   int bmove, loop, i, j;
   unsigned bias;
   register DDS_IBM8_IN *rinp;
   register DDS_IEEE8_OUT *rout;

#ifdef HOST_SWAP 
   ddsSWAP8X(out, inp, nsm);
   rinp = out;
#else
   rinp = inp;
#endif
   rout = out;
   loop = nsm;

   for (i=0;i<loop;i++) {
      /* check for 0 value and return 0 */
      if ((*rinp == 0) && (*(rinp + 1) == 0)) {
         *rout++ = 0;
         *rout++ = 0;
         rinp += 2;
      } else {

         /*
          * strip off the mantissa from the ibm floating point
          */
         ibm.man[0] = *rinp & 077777777;
         ibm.man[1] = *(rinp + 1);

         /*
          * move the first non zero bit of the mantissa to the most 
          * significant bit position of the mantissa. Then remove the
          * hiden bit (the most significant bit).
          */
         for (bmove = 1; bmove < 57; bmove++) {
            if ((ibm.man[0] & ~037777777) > 0)
               break;
            else {
               ibm.man[0] <<= 1;
               if (ibm.man[1] & 0x80000000)
                  ibm.man[0] |= 1;
               ibm.man[1] <<= 1;
            }
         }
         ibm.man[0] = ibm.man[0] & ~040000000;

         /*
          * strip off the bias from the ibm floating point. Then mask the
          * last 8 bits as the the ibm bias.
          */
         bias = *rinp >> 24;
         bias = bias & 0177;

         /*
          * compute the sun bias. convert base 16 to base 2 by multiple 
          * by 4. substract the number of moves to the left by mantissa,
          * i.e. one move to left reduce the power by 1 for base 2. Add the
          * base sun based power of 127.
          */
         bias = (((bias - 64) * 4) - bmove) + 1023;

         /*
          * 3 extra exponent bits in ieee so only shift by 20 not 23 then 
          * shift the mantissa back to the right 3 places
          */
         bias <<= 20;
         for (j=0;j<3;j++) {
            ibm.man[1] >>= 1;
            if (ibm.man[0] & 1) ibm.man[1] |= 0x80000000;
            ibm.man[0] >>= 1;
         }

         /*
          * map the mantissa and bias together. Then add the sign bit.
          */
         ibm.man[0] = ibm.man[0] | bias;
         *rout++ = (020000000000 & *rinp) | ibm.man[0];
         *rout++ = ibm.man[1];
         rinp++;
      }
   }
#ifdef HOST_SWAP 
   ddsSWAP8X(out, out, nsm);
#endif
   return (0);
}
