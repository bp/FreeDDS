static char rcsid[]="$Id: makecon2.c 164 2010-07-29 17:01:42Z user $";
/*
RCS-t=DDS main, create dds_con2.h include file

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cdds.h"
#include "dds_stack.h"
#include "dds_util.h"
#include "dds_ptype.h"

typedef struct HOST_TYPES
{  /* host type control info (DDS_PTYPE info) */
   DDS_BOOLEAN ctype;   /* TRUE iff type is supported by host */ 
   DDS_PTYPE ptype;     /* Best approximation of type for typedef. */
                        /*    Example, FLOAT8 and FLOAT4IB.. */
} HOST_TYPES;

HOST_TYPES host[DDS_STYPE_MAX];

DDS_PRIME *dds_prime = NULL;

typedef enum FORM
{  /* templates associated with CON2 macros */
   FORM_UNDEFINED,  /* not defined */
   FORM_BOGUS, /* illegal conversion, no CON2 macro */
   FORM_ERROR, /* CON3_ERROR(ltype, rtype) */
   FORM_COPY,  /* CON3_COPY(LP2, RP2, MAX2) */
               /*    COPY must be < ASSN, ASSR, ASSX, and FUNC */
   FORM_TRAN,  /* CON3_TRAN(ltype, rtype, LP2, RP2, MAX2) */
   FORM_ASSN,  /* CON3_##LA2##RA2##_##S2(ASSN, ltype, rtype, LP2, RP2, MAX2) */
   FORM_ASSR,  /* CON3_##LA2##RA2##_##S2(ASSR, ltype, rtype, LP2, RP2, MAX2) */
   FORM_ASSX,  /* CON3_##LA2##RA2##_##S2(ASSX, ltype, rtype, LP2, RP2, MAX2) */
   FORM_FUNC,  /* CON3_##LA2##RA2##_##S2(FUNC, ltype, rtype, LP2, RP2, MAX2) */
   FORM_MONO,  /* CON3_##S2##_MONO(LA2, RA2, ltype, rtype, LP2, RP2, MAX2, itype) */
   FORM_DUAL   /* CON3_##S2##_DUAL(LA2, RA2, ltype, rtype, LP2, RP2, MAX2, iltype, irtype) */
} FORM;

typedef struct CON2
{  /* dds_con2.h control info (ltype-rtype pairs) */
   FORM form;            /* macro form */
   DDS_BOOLEAN lalign;   /* TRUE iff left  is implicitly aligned */
   DDS_BOOLEAN ralign;   /* TRUE iff right is implicitly aligned */
   DDS_PTYPE lit;        /* left  intermediate type for DUAL or MONO */
   DDS_PTYPE rit;        /* right intermediate type for DUAL */
} CON2;

CON2 con2[DDS_STYPE_MAX][DDS_STYPE_MAX];

CON2 dir2[DDS_STYPE_MAX][DDS_STYPE_MAX];

/*******************/
/* main makecon2.c */
/*******************/

int main(
   int argc,
   char **argv)
{
   DDS_PTYPE ptype, ltype, rtype, lit, rit, it;
   CON2 *item;
   FILE *fp;
   const char *copy_op;

   /************************************************************/
   /* allocate and initialize array for prime type description */
   /************************************************************/

   dds_prime = (DDS_PRIME*)malloc(DDS_STYPE_MAX * sizeof(DDS_PRIME));
 
   ddsPrimeTypes();

   /*****************************************/
   /* initialize info describing host types */
   /*****************************************/

   /* default host types */
   for(ptype = DDS_STYPE_MIN; ptype < DDS_STYPE_MAX; ptype++) {
      host[ptype].ctype = DDS_FALSE;       /* not supported by host */
      host[ptype].ptype = DDS_CTYPE_VOID;  /* best approximation */
   }

   /* host character types */
   #if (DDS_ASCII1 == HOST_CHAR_DDS)
      host[DDS_STYPE_ASCII1].ctype = DDS_TRUE;
   #elif (DDS_EBCDIC1 == HOST_CHAR_DDS)
      host[DDS_STYPE_EBCDIC1].ctype = DDS_TRUE;
   #endif
   host[DDS_STYPE_ASCII1].ptype = DDS_CTYPE_CHAR;
   host[DDS_STYPE_EBCDIC1].ptype = DDS_CTYPE_CHAR;

   /* 1 byte integral types */
   #undef I1
   #undef U1
   #if (DDS_INTEGER1 == HOST_SCHAR_DDS)
      host[DDS_STYPE_INTEGER1].ctype = DDS_TRUE;
      host[DDS_STYPE_UNSIGNED1].ctype = DDS_TRUE;
      #define I1 DDS_CTYPE_SCHAR
      #define U1 DDS_CTYPE_UCHAR
   #endif

   /* 2 byte integral types */
   #undef I2
   #undef U2
   #if (DDS_INTEGER2 == HOST_SHORT_DDS)
      host[DDS_STYPE_INTEGER2].ctype = DDS_TRUE;
      host[DDS_STYPE_UNSIGNED2].ctype = DDS_TRUE;
      #define I2 DDS_CTYPE_SHORT
      #define U2 DDS_CTYPE_USHORT
   #elif (DDS_INTEGER2X == HOST_SHORT_DDS)
      host[DDS_STYPE_INTEGER2X].ctype = DDS_TRUE;
      host[DDS_STYPE_UNSIGNED2X].ctype = DDS_TRUE;
      #define I2 DDS_CTYPE_SHORT
      #define U2 DDS_CTYPE_USHORT
   #elif (DDS_INTEGER2 == HOST_INT_DDS)
      host[DDS_STYPE_INTEGER2].ctype = DDS_TRUE;
      host[DDS_STYPE_UNSIGNED2].ctype = DDS_TRUE;
      #define I2 DDS_CTYPE_INT
      #define U2 DDS_CTYPE_UINT
   #elif (DDS_INTEGER2X == HOST_INT_DDS)
      host[DDS_STYPE_INTEGER2X].ctype = DDS_TRUE;
      host[DDS_STYPE_UNSIGNED2X].ctype = DDS_TRUE;
      #define I2 DDS_CTYPE_INT
      #define U2 DDS_CTYPE_UINT
   #endif

   /* 4 byte integral types */
   #undef I4
   #undef U4
   #if (DDS_INTEGER4 == HOST_SHORT_DDS)
      host[DDS_STYPE_INTEGER4].ctype = DDS_TRUE;
      host[DDS_STYPE_UNSIGNED4].ctype = DDS_TRUE;
      #define I4 DDS_CTYPE_SHORT
      #define U4 DDS_CTYPE_USHORT
   #elif (DDS_INTEGER4X == HOST_SHORT_DDS)
      host[DDS_STYPE_INTEGER4X].ctype = DDS_TRUE;
      host[DDS_STYPE_UNSIGNED4X].ctype = DDS_TRUE;
      #define I4 DDS_CTYPE_SHORT
      #define U4 DDS_CTYPE_USHORT
   #elif (DDS_INTEGER4 == HOST_INT_DDS)
      host[DDS_STYPE_INTEGER4].ctype = DDS_TRUE;
      host[DDS_STYPE_UNSIGNED4].ctype = DDS_TRUE;
      #define I4 DDS_CTYPE_INT
      #define U4 DDS_CTYPE_UINT
   #elif (DDS_INTEGER4X == HOST_INT_DDS)
      host[DDS_STYPE_INTEGER4X].ctype = DDS_TRUE;
      host[DDS_STYPE_UNSIGNED4X].ctype = DDS_TRUE;
      #define I4 DDS_CTYPE_INT
      #define U4 DDS_CTYPE_UINT
   #elif (DDS_INTEGER4 == HOST_LONG_DDS)
      host[DDS_STYPE_INTEGER4].ctype = DDS_TRUE;
      host[DDS_STYPE_UNSIGNED4].ctype = DDS_TRUE;
      #define I4 DDS_CTYPE_LONG
      #define U4 DDS_CTYPE_ULONG
   #elif (DDS_INTEGER4X == HOST_LONG_DDS)
      host[DDS_STYPE_INTEGER4X].ctype = DDS_TRUE;
      host[DDS_STYPE_UNSIGNED4X].ctype = DDS_TRUE;
      #define I4 DDS_CTYPE_LONG
      #define U4 DDS_CTYPE_ULONG
   #elif (DDS_INTEGER4 == HOST_LLONG_DDS)
      host[DDS_STYPE_INTEGER4].ctype = DDS_TRUE;
      host[DDS_STYPE_UNSIGNED4].ctype = DDS_TRUE;
      #define I4 DDS_CTYPE_LLONG
      #define U4 DDS_CTYPE_ULLONG
   #elif (DDS_INTEGER4X == HOST_LLONG_DDS)
      host[DDS_STYPE_INTEGER4X].ctype = DDS_TRUE;
      host[DDS_STYPE_UNSIGNED4X].ctype = DDS_TRUE;
      #define I4 DDS_CTYPE_LLONG
      #define U4 DDS_CTYPE_ULLONG
   #endif

   /* 8 byte integral types */
   #undef I8
   #undef U8
   #if (DDS_INTEGER8 == HOST_INT_DDS)
      host[DDS_STYPE_INTEGER8].ctype = DDS_TRUE;
      host[DDS_STYPE_UNSIGNED8].ctype = DDS_TRUE;
      #define I8 DDS_CTYPE_INT
      #define U8 DDS_CTYPE_UINT
   #elif (DDS_INTEGER8X == HOST_INT_DDS)
      host[DDS_STYPE_INTEGER8X].ctype = DDS_TRUE;
      host[DDS_STYPE_UNSIGNED8X].ctype = DDS_TRUE;
      #define I8 DDS_CTYPE_INT
      #define U8 DDS_CTYPE_UINT
   #elif (DDS_INTEGER8 == HOST_LONG_DDS)
      host[DDS_STYPE_INTEGER8].ctype = DDS_TRUE;
      host[DDS_STYPE_UNSIGNED8].ctype = DDS_TRUE;
      #define I8 DDS_CTYPE_LONG
      #define U8 DDS_CTYPE_ULONG
   #elif (DDS_INTEGER8X == HOST_LONG_DDS)
      host[DDS_STYPE_INTEGER8X].ctype = DDS_TRUE;
      host[DDS_STYPE_UNSIGNED8X].ctype = DDS_TRUE;
      #define I8 DDS_CTYPE_LONG
      #define U8 DDS_CTYPE_ULONG
   #elif (DDS_INTEGER8 == HOST_LLONG_DDS)
      host[DDS_STYPE_INTEGER8].ctype = DDS_TRUE;
      host[DDS_STYPE_UNSIGNED8].ctype = DDS_TRUE;
      #define I8 DDS_CTYPE_LLONG
      #define U8 DDS_CTYPE_ULLONG
   #elif (DDS_INTEGER8X == HOST_LLONG_DDS)
      host[DDS_STYPE_INTEGER8X].ctype = DDS_TRUE;
      host[DDS_STYPE_UNSIGNED8X].ctype = DDS_TRUE;
      #define I8 DDS_CTYPE_LLONG
      #define U8 DDS_CTYPE_ULLONG
   #endif

   /* propagate from high to low precision */
   #ifndef I2
      #ifndef I4
         #define I4 I8
         #define U4 U8
      #endif
      #define I2 I4
      #define U2 U4
   #endif

   /* propagate from low to high precision */
   #ifndef I8
      #ifndef I4
         #ifndef I2
            #define I2 I1
            #define U2 U1
         #endif
         #define I4 I2
         #define U4 U2
      #endif
      #define I8 I4
      #define U8 U4
   #endif

   /* save the "best" approximation */
   host[DDS_STYPE_INTEGER1].ptype = I1;
   host[DDS_STYPE_INTEGER2].ptype = I2;
   host[DDS_STYPE_INTEGER2X].ptype = I2;
   host[DDS_STYPE_INTEGER4].ptype = I4;
   host[DDS_STYPE_INTEGER4X].ptype = I4;
   host[DDS_STYPE_INTEGER8].ptype = I8;
   host[DDS_STYPE_INTEGER8X].ptype = I8;

   host[DDS_STYPE_UNSIGNED1].ptype = U1;
   host[DDS_STYPE_UNSIGNED2].ptype = U2;
   host[DDS_STYPE_UNSIGNED2X].ptype = U2;
   host[DDS_STYPE_UNSIGNED4].ptype = U4;
   host[DDS_STYPE_UNSIGNED4X].ptype = U4;
   host[DDS_STYPE_UNSIGNED8].ptype = U8;
   host[DDS_STYPE_UNSIGNED8X].ptype = U8;

   /* 4 byte float types */
   #undef F4
   #undef C8
   #if (DDS_FLOAT4 == HOST_FLOAT_DDS)
      host[DDS_STYPE_FLOAT4].ctype = DDS_TRUE;
      host[DDS_STYPE_COMPLEX8].ctype = DDS_TRUE;
      #define F4 DDS_CTYPE_FLOAT
      #define C8 DDS_CTYPE_CPX
   #elif (DDS_FLOAT4 == HOST_DOUBLE_DDS)
      host[DDS_STYPE_FLOAT4].ctype = DDS_TRUE;
      host[DDS_STYPE_COMPLEX8].ctype = DDS_TRUE;
      #define F4 DDS_CTYPE_DOUBLE
      #define C8 DDS_CTYPE_DBLCPX
   #elif (DDS_FLOAT4X == HOST_FLOAT_DDS)
      host[DDS_STYPE_FLOAT4X].ctype = DDS_TRUE;
      host[DDS_STYPE_COMPLEX8X].ctype = DDS_TRUE;
      #define F4 DDS_CTYPE_FLOAT
      #define C8 DDS_CTYPE_CPX
   #elif (DDS_FLOAT4X == HOST_DOUBLE_DDS)
      host[DDS_STYPE_FLOAT4X].ctype = DDS_TRUE;
      host[DDS_STYPE_COMPLEX8X].ctype = DDS_TRUE;
      #define F4 DDS_CTYPE_DOUBLE
      #define C8 DDS_CTYPE_DBLCPX
   #elif (DDS_FLOAT4IBM == HOST_FLOAT_DDS)
      host[DDS_STYPE_FLOAT4IBM].ctype = DDS_TRUE;
      host[DDS_STYPE_COMPLEX8IBM].ctype = DDS_TRUE;
      #define F4 DDS_CTYPE_FLOAT
      #define C8 DDS_CTYPE_CPX
   #elif (DDS_FLOAT4IBM == HOST_DOUBLE_DDS)
      host[DDS_STYPE_FLOAT4IBM].ctype = DDS_TRUE;
      host[DDS_STYPE_COMPLEX8IBM].ctype = DDS_TRUE;
      #define F4 DDS_CTYPE_DOUBLE
      #define C8 DDS_CTYPE_DBLCPX
   #endif

   /* 8 byte float types */
   #undef F8
   #undef C16
   #if (DDS_FLOAT8 == HOST_FLOAT_DDS)
      host[DDS_STYPE_FLOAT8].ctype = DDS_TRUE;
      host[DDS_STYPE_COMPLEX16].ctype = DDS_TRUE;
      #define F8 DDS_CTYPE_FLOAT
      #define C16 DDS_CTYPE_CPX
   #elif (DDS_FLOAT8 == HOST_DOUBLE_DDS)
      host[DDS_STYPE_FLOAT8].ctype = DDS_TRUE;
      host[DDS_STYPE_COMPLEX16].ctype = DDS_TRUE;
      #define F8 DDS_CTYPE_DOUBLE
      #define C16 DDS_CTYPE_DBLCPX
   #elif (DDS_FLOAT8 == HOST_LDOUBLE_DDS)
      host[DDS_STYPE_FLOAT8].ctype = DDS_TRUE;
      host[DDS_STYPE_COMPLEX16].ctype = DDS_TRUE;
      #define F8 DDS_CTYPE_LDOUBLE
      #define C16 DDS_CTYPE_LDBLCPX
   #elif (DDS_FLOAT8X == HOST_FLOAT_DDS)
      host[DDS_STYPE_FLOAT8X].ctype = DDS_TRUE;
      host[DDS_STYPE_COMPLEX16X].ctype = DDS_TRUE;
      #define F8 DDS_CTYPE_FLOAT
      #define C16 DDS_CTYPE_CPX
   #elif (DDS_FLOAT8X == HOST_DOUBLE_DDS)
      host[DDS_STYPE_FLOAT8X].ctype = DDS_TRUE;
      host[DDS_STYPE_COMPLEX16X].ctype = DDS_TRUE;
      #define F8 DDS_CTYPE_DOUBLE
      #define C16 DDS_CTYPE_DBLCPX
   #elif (DDS_FLOAT8X == HOST_LDOUBLE_DDS)
      host[DDS_STYPE_FLOAT8X].ctype = DDS_TRUE;
      host[DDS_STYPE_COMPLEX16X].ctype = DDS_TRUE;
      #define F8 DDS_CTYPE_LDOUBLE
      #define C16 DDS_CTYPE_LDBLCPX
   #elif (DDS_FLOAT8IBM == HOST_FLOAT_DDS)
      host[DDS_STYPE_FLOAT8IBM].ctype = DDS_TRUE;
      host[DDS_STYPE_COMPLEX16IBM].ctype = DDS_TRUE;
      #define F8 DDS_CTYPE_FLOAT
      #define C16 DDS_CTYPE_CPX
   #elif (DDS_FLOAT8IBM == HOST_DOUBLE_DDS)
      host[DDS_STYPE_FLOAT8IBM].ctype = DDS_TRUE;
      host[DDS_STYPE_COMPLEX16IBM].ctype = DDS_TRUE;
      #define F8 DDS_CTYPE_DOUBLE
      #define C16 DDS_CTYPE_DBLCPX
   #endif

   /* 16 byte float types */
   #undef F16
   #undef C32
   #if (DDS_FLOAT16 == HOST_FLOAT_DDS)
      host[DDS_STYPE_FLOAT16].ctype = DDS_TRUE;
      host[DDS_STYPE_COMPLEX32].ctype = DDS_TRUE;
      #define F16 DDS_CTYPE_FLOAT
      #define C32 DDS_CTYPE_CPX
   #elif (DDS_FLOAT16 == HOST_DOUBLE_DDS)
      host[DDS_STYPE_FLOAT16].ctype = DDS_TRUE;
      host[DDS_STYPE_COMPLEX32].ctype = DDS_TRUE;
      #define F16 DDS_CTYPE_DOUBLE
      #define C32 DDS_CTYPE_DBLCPX
   #elif (DDS_FLOAT16 == HOST_LDOUBLE_DDS)
      host[DDS_STYPE_FLOAT16].ctype = DDS_TRUE;
      host[DDS_STYPE_COMPLEX32].ctype = DDS_TRUE;
      #define F16 DDS_CTYPE_LDOUBLE
      #define C32 DDS_CTYPE_LDBLCPX
   #elif (DDS_FLOAT16X == HOST_FLOAT_DDS)
      host[DDS_STYPE_FLOAT16X].ctype = DDS_TRUE;
      host[DDS_STYPE_COMPLEX32X].ctype = DDS_TRUE;
      #define F16 DDS_CTYPE_FLOAT
      #define C32 DDS_CTYPE_CPX
   #elif (DDS_FLOAT16X == HOST_DOUBLE_DDS)
      host[DDS_STYPE_FLOAT16X].ctype = DDS_TRUE;
      host[DDS_STYPE_COMPLEX32X].ctype = DDS_TRUE;
      #define F16 DDS_CTYPE_DOUBLE
      #define C32 DDS_CTYPE_DBLCPX
   #elif (DDS_FLOAT16X == HOST_LDOUBLE_DDS)
      host[DDS_STYPE_FLOAT16X].ctype = DDS_TRUE;
      host[DDS_STYPE_COMPLEX32X].ctype = DDS_TRUE;
      #define F16 DDS_CTYPE_LDOUBLE
      #define C32 DDS_CTYPE_LDBLCPX
   #endif

   host[DDS_STYPE_STRUCT].ctype = DDS_FALSE;
   host[DDS_STYPE_UNION].ctype = DDS_FALSE;

   /* propagate from high to low precision */
   #ifndef F4
      #ifndef F8
         #ifndef F16
            void abort compilation;
         #endif
         #define F8 F16
         #define C16 C32
      #endif
      #define F4 F8
      #define C8 C16
   #endif

   /* propagate from low to high precision */
   #ifndef F16
      #ifndef F8
         #define F8 F4
         #define C16 C8
      #endif
      #define F16 F8
      #define C32 C16
   #endif

   /* save the "best" approximation */
   host[DDS_STYPE_FLOAT4].ptype = F4;
   host[DDS_STYPE_FLOAT4X].ptype = F4;
   host[DDS_STYPE_FLOAT4IBM].ptype = F4;
   host[DDS_STYPE_FLOAT8].ptype = F8;
   host[DDS_STYPE_FLOAT8X].ptype = F8;
   host[DDS_STYPE_FLOAT8IBM].ptype = F8;
   host[DDS_STYPE_FLOAT16].ptype = F16;
   host[DDS_STYPE_FLOAT16X].ptype = F16;

   host[DDS_STYPE_COMPLEX8].ptype = C8;
   host[DDS_STYPE_COMPLEX8X].ptype = C8;
   host[DDS_STYPE_COMPLEX8IBM].ptype = C8;
   host[DDS_STYPE_COMPLEX16].ptype = C16;
   host[DDS_STYPE_COMPLEX16X].ptype = C16;
   host[DDS_STYPE_COMPLEX16IBM].ptype = C16;
   host[DDS_STYPE_COMPLEX32].ptype = C32;
   host[DDS_STYPE_COMPLEX32X].ptype = C32;

   /*****************************************/
   /* initialize info describing dds_con2.h */
   /*****************************************/
   for(ltype = DDS_STYPE_MIN; ltype < DDS_STYPE_MAX; ltype++) {
      for(rtype = DDS_STYPE_MIN; rtype < DDS_STYPE_MAX; rtype++) {
         con2[ltype][rtype].form = FORM_UNDEFINED;
      }
   }

   /* byte copy between identical stream types */
   for(ptype = DDS_STYPE_MIN; ptype < DDS_STYPE_MAX; ptype++) {
      con2[ptype][ptype].form = FORM_COPY;
   }

   /* byte copy between "almost" identical types */
   con2[DDS_STYPE_INTEGER1][DDS_STYPE_UNSIGNED1].form = FORM_COPY;
   con2[DDS_STYPE_INTEGER2][DDS_STYPE_UNSIGNED2].form = FORM_COPY;
   con2[DDS_STYPE_INTEGER2X][DDS_STYPE_UNSIGNED2X].form = FORM_COPY;
   con2[DDS_STYPE_INTEGER4][DDS_STYPE_UNSIGNED4].form = FORM_COPY;
   con2[DDS_STYPE_INTEGER4X][DDS_STYPE_UNSIGNED4X].form = FORM_COPY;
   con2[DDS_STYPE_INTEGER8][DDS_STYPE_UNSIGNED8].form = FORM_COPY;
   con2[DDS_STYPE_INTEGER8X][DDS_STYPE_UNSIGNED8X].form = FORM_COPY;

   /* byte copy between "almost" identical types */
   con2[DDS_STYPE_UNSIGNED1][DDS_STYPE_INTEGER1].form = FORM_COPY;
   con2[DDS_STYPE_UNSIGNED2][DDS_STYPE_INTEGER2].form = FORM_COPY;
   con2[DDS_STYPE_UNSIGNED2X][DDS_STYPE_INTEGER2X].form = FORM_COPY;
   con2[DDS_STYPE_UNSIGNED4][DDS_STYPE_INTEGER4].form = FORM_COPY;
   con2[DDS_STYPE_UNSIGNED4X][DDS_STYPE_INTEGER4X].form = FORM_COPY;
   con2[DDS_STYPE_UNSIGNED8][DDS_STYPE_INTEGER8].form = FORM_COPY;
   con2[DDS_STYPE_UNSIGNED8X][DDS_STYPE_INTEGER8X].form = FORM_COPY;

   /* character ascii/ebcdic translations */
   con2[DDS_STYPE_ASCII1][DDS_STYPE_EBCDIC1].form = FORM_TRAN;
   con2[DDS_STYPE_EBCDIC1][DDS_STYPE_ASCII1].form = FORM_TRAN;

   for(ltype = DDS_STYPE_MIN; ltype < DDS_STYPE_STRUCT; ltype++) {
      for(rtype = DDS_STYPE_MIN; rtype < DDS_STYPE_STRUCT; rtype++) {

         if(con2[ltype][rtype].form != FORM_UNDEFINED) continue;

         if(dds_prime[ltype].genus == DDS_TYPE_CHARS ||
            dds_prime[rtype].genus == DDS_TYPE_CHARS) {
            /* Bogus, can't convert between char and non-char types. */
            /* Character conversions have already been initialized. */
            con2[ltype][rtype].form = FORM_BOGUS;
         } else if(dds_prime[ltype].genus != DDS_TYPE_COMPLEXS &&
                   dds_prime[rtype].genus == DDS_TYPE_COMPLEXS) {
            /* Bogus, can't convert to non-complex from complex */
            con2[ltype][rtype].form = FORM_BOGUS;
         } else if(host[ltype].ctype && host[rtype].ctype) {
            /* Both left and right types supported by "C". */
            /* Convert via "C" assignment (ASSN, ASSR, ASSX). */
            if(dds_prime[ltype].genus != DDS_TYPE_COMPLEXS) {
               /* Simple assignment, neither type is complex. */
               con2[ltype][rtype].form = FORM_ASSN;
            } else if(dds_prime[rtype].genus != DDS_TYPE_COMPLEXS) {
               /* Assign to complex from non-complex. */
               con2[ltype][rtype].form = FORM_ASSR;
            } else {
               /* Assign to complex from complex. */
               con2[ltype][rtype].form = FORM_ASSX;
            }
         } else {
            /* Architecture specific conversion functions. */
            if(       ltype == DDS_STYPE_INTEGER2 &&
                      rtype == DDS_STYPE_INTEGER2X) {
               con2[ltype][rtype].form = FORM_FUNC;
            } else if(ltype == DDS_STYPE_INTEGER2 &&
                      rtype == DDS_STYPE_UNSIGNED2X) {
               con2[ltype][rtype].form = FORM_FUNC;

            } else if(ltype == DDS_STYPE_INTEGER2X &&
                      rtype == DDS_STYPE_INTEGER2) {
               con2[ltype][rtype].form = FORM_FUNC;
            } else if(ltype == DDS_STYPE_INTEGER2X &&
                      rtype == DDS_STYPE_UNSIGNED2X) {
               con2[ltype][rtype].form = FORM_FUNC;

            } else if(ltype == DDS_STYPE_UNSIGNED2 &&
                      rtype == DDS_STYPE_INTEGER2X) {
               con2[ltype][rtype].form = FORM_FUNC;
            } else if(ltype == DDS_STYPE_UNSIGNED2 &&
                      rtype == DDS_STYPE_UNSIGNED2X) {
               con2[ltype][rtype].form = FORM_FUNC;

            } else if(ltype == DDS_STYPE_UNSIGNED2X &&
                      rtype == DDS_STYPE_INTEGER2) {
               con2[ltype][rtype].form = FORM_FUNC;
            } else if(ltype == DDS_STYPE_UNSIGNED2X &&
                      rtype == DDS_STYPE_UNSIGNED2) {
               con2[ltype][rtype].form = FORM_FUNC;

            } else if(ltype == DDS_STYPE_INTEGER4 &&
                      rtype == DDS_STYPE_INTEGER4X) {
               con2[ltype][rtype].form = FORM_FUNC;
            } else if(ltype == DDS_STYPE_INTEGER4 &&
                      rtype == DDS_STYPE_UNSIGNED4X) {
               con2[ltype][rtype].form = FORM_FUNC;

            } else if(ltype == DDS_STYPE_INTEGER4X &&
                      rtype == DDS_STYPE_INTEGER4) {
               con2[ltype][rtype].form = FORM_FUNC;
            } else if(ltype == DDS_STYPE_INTEGER4X &&
                      rtype == DDS_STYPE_UNSIGNED4) {
               con2[ltype][rtype].form = FORM_FUNC;

            } else if(ltype == DDS_STYPE_UNSIGNED4 &&
                      rtype == DDS_STYPE_INTEGER4X) {
               con2[ltype][rtype].form = FORM_FUNC;
            } else if(ltype == DDS_STYPE_UNSIGNED4 &&
                      rtype == DDS_STYPE_UNSIGNED4X) {
               con2[ltype][rtype].form = FORM_FUNC;

            } else if(ltype == DDS_STYPE_UNSIGNED4X &&
                      rtype == DDS_STYPE_INTEGER4) {
               con2[ltype][rtype].form = FORM_FUNC;
            } else if(ltype == DDS_STYPE_UNSIGNED4X &&
                      rtype == DDS_STYPE_UNSIGNED4) {
               con2[ltype][rtype].form = FORM_FUNC;

            } else if(ltype == DDS_STYPE_INTEGER8 &&
                      rtype == DDS_STYPE_INTEGER8X) {
               con2[ltype][rtype].form = FORM_FUNC;
            } else if(ltype == DDS_STYPE_INTEGER8 &&
                      rtype == DDS_STYPE_UNSIGNED8X) {
               con2[ltype][rtype].form = FORM_FUNC;

            } else if(ltype == DDS_STYPE_INTEGER8X &&
                      rtype == DDS_STYPE_INTEGER8) {
               con2[ltype][rtype].form = FORM_FUNC;
            } else if(ltype == DDS_STYPE_INTEGER8X &&
                      rtype == DDS_STYPE_UNSIGNED8) {
               con2[ltype][rtype].form = FORM_FUNC;

            } else if(ltype == DDS_STYPE_UNSIGNED8 &&
                      rtype == DDS_STYPE_INTEGER8X) {
               con2[ltype][rtype].form = FORM_FUNC;
            } else if(ltype == DDS_STYPE_UNSIGNED8 &&
                      rtype == DDS_STYPE_UNSIGNED8X) {
               con2[ltype][rtype].form = FORM_FUNC;

            } else if(ltype == DDS_STYPE_UNSIGNED8X &&
                      rtype == DDS_STYPE_INTEGER8) {
               con2[ltype][rtype].form = FORM_FUNC;
            } else if(ltype == DDS_STYPE_UNSIGNED8X &&
                      rtype == DDS_STYPE_UNSIGNED8) {
               con2[ltype][rtype].form = FORM_FUNC;

            } else if(ltype == DDS_STYPE_FLOAT4 &&
                      rtype == DDS_STYPE_FLOAT4X) {
               con2[ltype][rtype].form = FORM_FUNC;
            } else if(ltype == DDS_STYPE_FLOAT4 &&
                      rtype == DDS_STYPE_FLOAT4IBM) {
               con2[ltype][rtype].form = FORM_FUNC;

            } else if(ltype == DDS_STYPE_FLOAT4X &&
                      rtype == DDS_STYPE_FLOAT4) {
               con2[ltype][rtype].form = FORM_FUNC;
            } else if(ltype == DDS_STYPE_FLOAT4X &&
                      rtype == DDS_STYPE_FLOAT4IBM) {
               con2[ltype][rtype].form = FORM_FUNC;

            } else if(ltype == DDS_STYPE_FLOAT4IBM &&
                      rtype == DDS_STYPE_FLOAT4) {
               con2[ltype][rtype].form = FORM_FUNC;
            } else if(ltype == DDS_STYPE_FLOAT4IBM &&
                      rtype == DDS_STYPE_FLOAT4X) {
               con2[ltype][rtype].form = FORM_FUNC;

            } else if(ltype == DDS_STYPE_FLOAT8 &&
                      rtype == DDS_STYPE_FLOAT8X) {
               con2[ltype][rtype].form = FORM_FUNC;
            } else if(ltype == DDS_STYPE_FLOAT8X &&
                      rtype == DDS_STYPE_FLOAT8) {
               con2[ltype][rtype].form = FORM_FUNC;
            } else if(ltype == DDS_STYPE_FLOAT8 &&
                      rtype == DDS_STYPE_FLOAT8IBM) {
               con2[ltype][rtype].form = FORM_FUNC;


            } else if(ltype == DDS_STYPE_COMPLEX8 &&
                      rtype == DDS_STYPE_COMPLEX8X) {
               con2[ltype][rtype].form = FORM_FUNC;
            } else if(ltype == DDS_STYPE_COMPLEX8 &&
                      rtype == DDS_STYPE_COMPLEX8IBM) {
               con2[ltype][rtype].form = FORM_FUNC;

            } else if(ltype == DDS_STYPE_COMPLEX8X &&
                      rtype == DDS_STYPE_COMPLEX8) {
               con2[ltype][rtype].form = FORM_FUNC;

            } else if(ltype == DDS_STYPE_COMPLEX8IBM &&
                      rtype == DDS_STYPE_COMPLEX8) {
               con2[ltype][rtype].form = FORM_FUNC;

            } else if(ltype == DDS_STYPE_COMPLEX16 &&
                      rtype == DDS_STYPE_COMPLEX16X) {
               con2[ltype][rtype].form = FORM_FUNC;
            } else if(ltype == DDS_STYPE_COMPLEX16X &&
                      rtype == DDS_STYPE_COMPLEX16) {
               con2[ltype][rtype].form = FORM_FUNC;
            }
            #ifdef HOST_FAKE8
               if(       ltype == DDS_STYPE_INTEGER1 &&
                         rtype == DDS_STYPE_INTEGER8) {
                     con2[ltype][rtype].form = FORM_FUNC;

               } else if(ltype == DDS_STYPE_INTEGER2 &&
                         rtype == DDS_STYPE_INTEGER8) {
                  con2[ltype][rtype].form = FORM_FUNC;

               } else if(ltype == DDS_STYPE_INTEGER4 &&
                         rtype == DDS_STYPE_INTEGER8) {
                  con2[ltype][rtype].form = FORM_FUNC;

               } else if(ltype == DDS_STYPE_INTEGER8 &&
                         rtype == DDS_STYPE_INTEGER1) {
                  con2[ltype][rtype].form = FORM_FUNC;
               } else if(ltype == DDS_STYPE_INTEGER8 &&
                         rtype == DDS_STYPE_INTEGER2) {
                  con2[ltype][rtype].form = FORM_FUNC;
               } else if(ltype == DDS_STYPE_INTEGER8 &&
                         rtype == DDS_STYPE_INTEGER4) {
                  con2[ltype][rtype].form = FORM_FUNC;

               } else if(ltype == DDS_STYPE_INTEGER8 &&
                         rtype == DDS_STYPE_UNSIGNED1) {
                  con2[ltype][rtype].form = FORM_FUNC;
               } else if(ltype == DDS_STYPE_INTEGER8 &&
                         rtype == DDS_STYPE_UNSIGNED2) {
                  con2[ltype][rtype].form = FORM_FUNC;
               } else if(ltype == DDS_STYPE_INTEGER8 &&
                         rtype == DDS_STYPE_UNSIGNED4) {
                  con2[ltype][rtype].form = FORM_FUNC;
               }
            #endif /* HOST_FAKE8 */
         }
      }
   }

   /* Create table of direct conversion (no intermediate types). */
   /* This is just a duplicate of the table accumulated so far. */
   memcpy(dir2, con2,
      DDS_STYPE_MAX * DDS_STYPE_MAX * sizeof(CON2));

   /* MONO conversions via ONE intermediate type. */
   for(ltype = DDS_STYPE_MIN; ltype < DDS_STYPE_STRUCT; ltype++) {
      for(rtype = DDS_STYPE_MIN; rtype < DDS_STYPE_STRUCT; rtype++) {

         if(con2[ltype][rtype].form != FORM_UNDEFINED) continue;

         for(it = DDS_STYPE_MIN; it < DDS_STYPE_STRUCT; it++) {
            if(dds_prime[it].prec < dds_prime[ltype].prec &&
               dds_prime[it].prec < dds_prime[rtype].prec) continue;

            if(dds_prime[it].genus == DDS_TYPE_INTEGERS &&
               dds_prime[ltype].genus != DDS_TYPE_INTEGERS &&
               dds_prime[rtype].genus != DDS_TYPE_INTEGERS) continue;

            if(dds_prime[it].genus == DDS_TYPE_UNSIGNEDS &&
               dds_prime[ltype].genus != DDS_TYPE_UNSIGNEDS &&
               dds_prime[rtype].genus != DDS_TYPE_UNSIGNEDS) continue;

            if(dir2[it][rtype].form >= FORM_COPY &&
               dir2[ltype][it].form >= FORM_COPY) {
               /* MONO conversion is supported via "it" */
               con2[ltype][rtype].form = FORM_MONO;
               con2[ltype][rtype].lit = it;
               break;
            }
         }
      }
   }

   /* DUAL conversions via TWO intermediate types */
   for(ltype = DDS_STYPE_MIN; ltype < DDS_STYPE_STRUCT; ltype++) {
      for(rtype = DDS_STYPE_MIN; rtype < DDS_STYPE_STRUCT; rtype++) {

         if(con2[ltype][rtype].form != FORM_UNDEFINED) continue;

         for(lit = DDS_STYPE_MIN; lit < DDS_STYPE_STRUCT; lit++) {
            for(rit = DDS_STYPE_MIN; rit < DDS_STYPE_STRUCT; rit++) {
               if((dds_prime[lit].prec < dds_prime[ltype].prec &&
                   dds_prime[lit].prec < dds_prime[rtype].prec) ||
                  (dds_prime[rit].prec < dds_prime[ltype].prec &&
                   dds_prime[rit].prec < dds_prime[rtype].prec)) continue;

               if((dds_prime[lit].genus == DDS_TYPE_INTEGERS ||
                   dds_prime[rit].genus == DDS_TYPE_INTEGERS) &&
                   dds_prime[ltype].genus != DDS_TYPE_INTEGERS &&
                   dds_prime[rtype].genus != DDS_TYPE_INTEGERS) continue;

               if((dds_prime[lit].genus == DDS_TYPE_UNSIGNEDS ||
                   dds_prime[rit].genus == DDS_TYPE_UNSIGNEDS) &&
                   dds_prime[ltype].genus != DDS_TYPE_UNSIGNEDS &&
                   dds_prime[rtype].genus != DDS_TYPE_UNSIGNEDS) continue;

               if(dir2[rit][rtype].form >= FORM_COPY &&
                  dir2[ltype][lit].form >= FORM_COPY &&
                  dir2[lit][rit].form >= FORM_COPY) {
                  /* direct conversion is supported via rit and lit */
                  con2[ltype][rtype].form = FORM_DUAL;
                  con2[ltype][rtype].lit = lit;
                  con2[ltype][rtype].rit = rit;
               }
            }
         }
      }
   }

   /********************************/
   /* print dds_con2.h description */
   /********************************/

   fp = fopen("dds_con2.h", "w");

   fprintf(fp,
      "/*\n"
      "   DO NOT EDIT THIS FILE !\n"
      "      It is made by executing main makecon2.c\n"
      "      The contents are dependent on the host architecture\n"
      "      $Id: makecon2.c 164 2010-07-29 17:01:42Z user $\n"
      "*/\n\n"
      "/**********************************************/\n"
      "/***   Copyright (C) 1997                   ***/\n"
      "/***   AMOCO Corporation, Tulsa, OK 74102   ***/\n"
      "/***   Randy Selzler, xx May 1997           ***/\n"
      "/**********************************************/\n"
      "\n"
      );

   fprintf(fp, "#ifdef DDS_CON2_EVALUATE\n\n");

   fprintf(fp,
      "/* Define precision of each DDS stream type. */\n"
      "\n"
      );

   for(ptype = DDS_STYPE_MIN; ptype < DDS_STYPE_STRUCT; ptype++) {
      fprintf(fp, "#define DDS_%s_P \t%i\n",
         dds_prime[ptype].macro, (int)dds_prime[ptype].prec);
   }

   fprintf(fp, "\n\n");

   fprintf(fp,
      "/* Define alignment of each DDS stream type. */\n"
      "\n"
      );

   for(ptype = DDS_STYPE_MIN; ptype < DDS_STYPE_STRUCT; ptype++) {
      fprintf(fp, "#define DDS_%s_A \t%i\n",
         dds_prime[ptype].macro, (int)dds_prime[ptype].align);
   }

   fprintf(fp, "\n\n");

   fprintf(fp,
      "/* Define host approximation of each DDS stream type. */\n"
      "\n"
      );

   for(ptype = DDS_STYPE_MIN; ptype < DDS_STYPE_STRUCT; ptype++) {
      fprintf(fp, "typedef HOST_%s_T \tDDS_%s_T;\n",
         dds_prime[host[ptype].ptype].macro, dds_prime[ptype].macro);
   }

   fprintf(fp, "\n\n");

   fprintf(fp,
      "/* Define conversion between pairs of DDS stream types. */\n"
      "\n"
      );

   for(ltype = DDS_STYPE_MIN; ltype < DDS_STYPE_STRUCT; ltype++) {
      for(rtype = DDS_STYPE_MIN; rtype < DDS_STYPE_STRUCT; rtype++) {

         item = &con2[ltype][rtype];

         if(item->form == FORM_BOGUS) continue;

         fprintf(fp, "#define CON2_%s_%s(S2,LA2,RA2,LP2,RP2,MAX2) ",
            dds_prime[ltype].macro, dds_prime[rtype].macro);

         switch(con2[ltype][rtype].form) {
            case FORM_COPY:
               if(dds_prime[ltype].prec == 1)
                  fprintf(fp, "CON3_COPY(LP2,RP2,MAX2)\n");
               else
                  fprintf(fp, "CON3_COPY(LP2,RP2,MAX2*%i)\n",
                     (int)dds_prime[ltype].prec);
               break;
            case FORM_TRAN:
               fprintf(fp, "CON3_TRAN(%s,%s,LP2,RP2,MAX2)\n",
                  dds_prime[ltype].macro, dds_prime[rtype].macro);
               break;
            case FORM_ASSN:
               fprintf(fp, "CON3_%s%s##_##S2(ASSN,%s,%s,LP2,RP2,MAX2)\n",
                  ((dds_prime[ltype].prec == 1) ? "##A" : "##LA2"),
                  ((dds_prime[rtype].prec == 1) ? "##A" : "##RA2"),
                  dds_prime[ltype].macro, dds_prime[rtype].macro);
               break;
            case FORM_ASSR:
               fprintf(fp, "CON3_%s%s##_##S2(ASSR,%s,%s,LP2,RP2,MAX2)\n",
                  ((dds_prime[ltype].prec == 1) ? "##A" : "##LA2"),
                  ((dds_prime[rtype].prec == 1) ? "##A" : "##RA2"),
                  dds_prime[ltype].macro, dds_prime[rtype].macro);
               break;
            case FORM_ASSX:
               fprintf(fp, "CON3_%s%s##_##S2(ASSX,%s,%s,LP2,RP2,MAX2)\n",
                  ((dds_prime[ltype].prec == 1) ? "##A" : "##LA2"),
                  ((dds_prime[rtype].prec == 1) ? "##A" : "##RA2"),
                  dds_prime[ltype].macro, dds_prime[rtype].macro);
               break;
            case FORM_FUNC:
               fprintf(fp, "CON3_%s%s##_##S2(FUNC,%s,%s,LP2,RP2,MAX2)\n",
                  ((dds_prime[ltype].prec == 1 ||
                     con2[ltype][rtype].lalign) ? "##A" : "##LA2"),
                  ((dds_prime[rtype].prec == 1 ||
                     con2[ltype][rtype].ralign) ? "##A" : "##RA2"),
                  dds_prime[ltype].macro, dds_prime[rtype].macro);
               break;
            case FORM_MONO:
               fprintf(fp, "CON3_##S2##_MONO(LA2,RA2,%s,%s,LP2,RP2,MAX2,%s)\n",
                  dds_prime[ltype].macro, dds_prime[rtype].macro,
                  dds_prime[con2[ltype][rtype].lit].macro);
               break;
            case FORM_DUAL:
               fprintf(fp, "CON3_##S2##_DUAL(LA2,RA2,%s,%s,LP2,RP2,MAX2,%s,%s)\n",
                  dds_prime[ltype].macro, dds_prime[rtype].macro,
                  dds_prime[con2[ltype][rtype].lit].macro,
                  dds_prime[con2[ltype][rtype].rit].macro);
               break;
            case FORM_ERROR:
            case FORM_UNDEFINED:
               fprintf(fp, "CON3_ERROR(%s,%s)\n",
                  dds_prime[ltype].macro, dds_prime[rtype].macro);
               break;
            default:
               fprintf(stderr, "makecon2: logic bug\n");
               exit(1);
         }
      }
   }

   fprintf(fp, "\n#endif /* DDS_CON2_EVALUATE */\n");

   fprintf(fp, "\n#ifdef DDS_CON2_COPY_OP\n\n");

   fprintf(fp,
      "/* DDS_TRUE iff DDS_ASSIGN_OP is effectively DDS_COPY_OP. */\n\n"
      "DDS_BOOLEAN dds_copy_op\n"
      "   [DDS_STYPE_MAX - DDS_STYPE_MIN]\n"
      "   [DDS_STYPE_MAX - DDS_STYPE_MIN] = {\n");

   for(ltype = DDS_STYPE_MIN; ltype < DDS_STYPE_MAX; ltype++) {
      fprintf(fp, "   {\n");
      for(rtype = DDS_STYPE_MIN; rtype < DDS_STYPE_MAX; rtype++) {

         item = &con2[ltype][rtype];

         if(item->form == FORM_COPY) {
            if(rtype + 1 < DDS_STYPE_MAX) {
               copy_op = "DDS_TRUE, ";
            } else {
               copy_op = "DDS_TRUE  ";
            }
         } else {
            if(rtype + 1 < DDS_STYPE_MAX) {
               copy_op = "DDS_FALSE,";
            } else {
               copy_op = "DDS_FALSE ";
            }
         }

         fprintf(fp, "      %s   /* %s \t= %s */\n",
            copy_op, dds_prime[ltype].macro, dds_prime[rtype].macro);
      }

      if(ltype + 1 < DDS_STYPE_MAX) {
         fprintf(fp, "   },\n");
      } else {
         fprintf(fp, "   }\n");
      }
   }

   fprintf(fp, "   };\n\n#endif /* DDS_CON2_COPY_OP */\n");

   fclose(fp);

   exit(0);
}
