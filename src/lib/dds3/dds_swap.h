#ifndef __DDS_SWAP_H
#define __DDS_SWAP_H

/*
RCS-t=DDS, macros to convert big-endian data for evaluate.c
 
$Id: dds_swap.h 16 2009-01-06 21:52:47Z user $
*/

/**********************************************************/
/* These functions are for handling byte swapping on PC's */
/**********************************************************/

#define CONVERT2(word) \
   (short)(((long)word<<8)&0x0000ff00)|(((long)word>>8)&0x000000ff)
#define CONVERT4(word) \
   (((word<<24)&0xff000000)|((word<<8)&0x00ff0000) \
   |((word>>8)&0x0000ff00)|((word>>24)&0x000000ff))

#define ddsSWAP2X(OUT, IN, COUNT) \
   {register int i, n = COUNT; \
   register DDS_INT2_OUT *out_buf = (DDS_INT2_OUT *) (OUT); \
   register DDS_INT2_IN  *in_buf  = (DDS_INT2_IN  *) (IN); \
   for(i=0;i<n;i++) { \
      *out_buf++ = CONVERT2(*in_buf); \
      in_buf++; \
   }}

#define ddsSWAP4X(OUT, IN, COUNT) \
   {register int i, n = COUNT; \
   register DDS_INT4_OUT *out_buf = (DDS_INT4_OUT *) (OUT); \
   register DDS_INT4_IN  *in_buf  = (DDS_INT4_IN  *) (IN); \
   for(i=0;i<n;i++) { \
      *out_buf++ = CONVERT4(*in_buf); \
      in_buf++; \
   }}

#define ddsSWAP8X(OUT, IN, COUNT) \
   {register int i, n = COUNT; \
   register DDS_INT4_OUT *out_buf = (DDS_INT4_OUT *) (OUT); \
   register DDS_INT4_IN  *in_buf  = (DDS_INT4_IN  *) (IN); \
   for(i=0;i<n;i++) { \
      out_buf[1] = CONVERT4(in_buf[0]); \
      out_buf[0] = CONVERT4(in_buf[1]); \
      out_buf += 2; in_buf += 2; \
   }}

/****************************************************************/
/* These functions are for handling byte John's STREAM swapping */
/****************************************************************/

#ifdef HOST_SWAP
#define ddsStreamReadSwap(SWAP, BUF, COUNT) \
   {register int n; \
   register DDS_INT4_IN *buf4; \
   register DDS_INT2_IN *buf2; \
   if ((SWAP) == 1) { \
      buf4 = (DDS_INT4_IN *) (BUF); \
      for(n=(COUNT)/4;n>0;n--) { \
         *buf4 = CONVERT4(*buf4); buf4++; \
   }} else if ((SWAP) == 2) { \
      buf2 = (DDS_INT2_IN *) (BUF); \
      for(n=(COUNT);n>0;n--) { \
         *buf2 = CONVERT2(*buf2); buf2++; \
   }} else if ((SWAP) == 4) { \
      buf4 = (DDS_INT4_IN *) (BUF); \
      for(n=(COUNT);n>0;n--) { \
         *buf4 = CONVERT4(*buf4); buf4++; \
   }}}
#define ddsStreamWriteSwap(SWAP, BUFFER, BUFLEN, OUTBUF, COUNT) \
   {register int n; \
   if ((COUNT)*(SWAP) > (BUFLEN)) { \
      (BUFLEN) = (COUNT)*(SWAP); \
      (OUTBUF) = (const void *)cdds_realloc((void*)(OUTBUF), (BUFLEN)); \
   } \
   if ((SWAP) == 1) { \
      register DDS_INT4_IN  *ibuf4 = (DDS_INT4_IN *)(BUFFER); \
      register DDS_INT4_OUT *obuf4 = (DDS_INT4_OUT *)(OUTBUF); \
      for(n=(COUNT)/4;n>0;n--) { \
         *obuf4 = CONVERT4(*ibuf4); obuf4++; ibuf4++; \
   }} else if ((SWAP) == 2) { \
      register DDS_INT2_IN  *ibuf2 = (DDS_INT2_IN *)(BUFFER); \
      register DDS_INT2_OUT *obuf2 = (DDS_INT2_OUT *)(OUTBUF); \
      for(n=(COUNT);n>0;n--) { \
         *obuf2 = CONVERT2(*ibuf2); obuf2++; ibuf2++; \
   }} else if ((SWAP) == 4) { \
      register DDS_INT4_IN  *ibuf4 = (DDS_INT4_IN *)(BUFFER); \
      register DDS_INT4_OUT *obuf4 = (DDS_INT4_OUT *)(OUTBUF); \
      for(n=(COUNT);n>0;n--) { \
         *obuf4 = CONVERT4(*ibuf4); obuf4++; ibuf4++; \
   }} else { \
      if ((BUFLEN)) cdds_free((void*)(OUTBUF)); \
      (BUFLEN) = 0; \
      (OUTBUF) = (const void *)(BUFFER); \
   }}
#else
#define ddsStreamReadSwap(SWAP, BUF, COUNT) \
   if ((SWAP)) (SWAP) = 0;
#define ddsStreamWriteSwap(SWAP, BUFFER, BUFLEN, OUTBUF, COUNT) \
   (OUTBUF) = (const void *)(BUFFER); \
   if ((BUFLEN)) (BUFLEN) = 0;
#endif

/***************************************************************/
/*  These macros are used in conjunction of each other to      */
/*  convert between big-endian data & "normal" little-endian   */
/*  for evaluation processing of DDS_ASSIGN_OP.                */
/*  Converts types:                                            */
/*        INTEGER2X                                            */
/*        INTEGER4X                                            */
/*        INTEGER8X                                            */
/*        UNSIGNED2X                                           */
/*        UNSIGNED4X                                           */
/*        UNSIGNED8X                                           */
/*        FLOAT4X                                              */
/*        FLOAT8X                                              */
/*        COMPLEX8X                                            */
/*        COMPLEX16X                                           */
/*                                                             */
/* The follow variables are expected to be defined:            */
/*    DDS_PTYPE stype1, stype2;                                */
/*    void *ptr1, *ptr2;                                       */
/*    size_t reset = dds_tmp.top;                              */
/*                                                             */
/***************************************************************/

#define SWAPPRE() \
   reset = dds_tmp.top; \
   switch(expra->stype2) { \
   case DDS_STYPE_INTEGER2X: \
      stype2 = DDS_STYPE_INTEGER2; \
      ptr2 = ddsStackTmp(expra->count, DDS_INTEGER2_P);  \
      ddsSWAP2X(ptr2, expra->arg2.value.ptr, expra->count ) \
      break; \
   case DDS_STYPE_INTEGER4X: \
      stype2 = DDS_STYPE_INTEGER4; \
      ptr2 = ddsStackTmp(expra->count, DDS_INTEGER4_P);  \
      ddsSWAP4X(ptr2, expra->arg2.value.ptr, expra->count ) \
      break; \
   case DDS_STYPE_INTEGER8X: \
      stype2 = DDS_STYPE_INTEGER8; \
      ptr2 = ddsStackTmp(expra->count, DDS_INTEGER8_P);  \
      ddsSWAP8X(ptr2, expra->arg2.value.ptr, expra->count ) \
      break; \
   case DDS_STYPE_UNSIGNED2X: \
      stype2 = DDS_STYPE_UNSIGNED2; \
      ptr2 = ddsStackTmp(expra->count, DDS_UNSIGNED2_P);  \
      ddsSWAP2X(ptr2, expra->arg2.value.ptr, expra->count ) \
      break; \
   case DDS_STYPE_UNSIGNED4X: \
      stype2 = DDS_STYPE_UNSIGNED4; \
      ptr2 = ddsStackTmp(expra->count, DDS_UNSIGNED4_P);  \
      ddsSWAP4X(ptr2, expra->arg2.value.ptr, expra->count ) \
      break; \
   case DDS_STYPE_UNSIGNED8X: \
      stype2 = DDS_STYPE_UNSIGNED8; \
      ptr2 = ddsStackTmp(expra->count, DDS_UNSIGNED8_P);  \
      ddsSWAP8X(ptr2, expra->arg2.value.ptr, expra->count ) \
      break; \
   case DDS_STYPE_FLOAT4X: \
      stype2 = DDS_STYPE_FLOAT4; \
       ptr2 = ddsStackTmp(expra->count, DDS_FLOAT4_P);  \
      ddsSWAP4X(ptr2, expra->arg2.value.ptr, expra->count ) \
     break; \
   case DDS_STYPE_FLOAT8X: \
      stype2 = DDS_STYPE_FLOAT8; \
      ptr2 = ddsStackTmp(expra->count, DDS_FLOAT8_P);  \
      ddsSWAP8X(ptr2, expra->arg2.value.ptr, expra->count ) \
      break; \
   case DDS_STYPE_COMPLEX8X: \
      stype2 = DDS_STYPE_COMPLEX8; \
      ptr2 = ddsStackTmp(expra->count, DDS_COMPLEX8_P);  \
      ddsSWAP4X(ptr2, expra->arg2.value.ptr, 2 * expra->count ) \
      break; \
   case DDS_STYPE_COMPLEX16X: \
      stype2 = DDS_STYPE_COMPLEX16; \
      ptr2 = ddsStackTmp(expra->count, DDS_COMPLEX16_P);  \
      ddsSWAP8X(ptr2, expra->arg2.value.ptr, 2 * expra->count ) \
      break; \
   default: \
     stype2 = expra->stype2; \
     ptr2   = expra->arg2.value.ptr; \
   } \
   switch(expra->stype1) { \
   case DDS_STYPE_INTEGER2X: \
      stype1 = DDS_STYPE_INTEGER2; \
      ptr1 = ddsStackTmp(expra->count, DDS_INTEGER2_P);  \
      break; \
   case DDS_STYPE_INTEGER4X: \
      stype1 = DDS_STYPE_INTEGER4; \
      ptr1 = ddsStackTmp(expra->count, DDS_INTEGER4_P);  \
      break; \
   case DDS_STYPE_INTEGER8X: \
      stype1 = DDS_STYPE_INTEGER8; \
      ptr1 = ddsStackTmp(expra->count, DDS_INTEGER8_P);  \
      break; \
   case DDS_STYPE_UNSIGNED2X: \
      stype1 = DDS_STYPE_UNSIGNED2; \
      ptr1 = ddsStackTmp(expra->count, DDS_UNSIGNED2_P);  \
      break; \
   case DDS_STYPE_UNSIGNED4X: \
      stype1 = DDS_STYPE_UNSIGNED4; \
      ptr1 = ddsStackTmp(expra->count, DDS_UNSIGNED4_P);  \
      break; \
   case DDS_STYPE_UNSIGNED8X: \
      stype1 = DDS_STYPE_UNSIGNED8; \
      ptr1 = ddsStackTmp(expra->count, DDS_UNSIGNED8_P);  \
      break; \
   case DDS_STYPE_FLOAT4X: \
      stype1 = DDS_STYPE_FLOAT4; \
       ptr1 = ddsStackTmp(expra->count, DDS_FLOAT4_P);  \
     break; \
   case DDS_STYPE_FLOAT8X: \
      stype1 = DDS_STYPE_FLOAT8; \
      ptr1 = ddsStackTmp(expra->count, DDS_FLOAT8_P);  \
      break; \
   case DDS_STYPE_COMPLEX8X: \
      stype1 = DDS_STYPE_COMPLEX8; \
      ptr1 = ddsStackTmp(expra->count, DDS_COMPLEX8_P);  \
      break; \
   case DDS_STYPE_COMPLEX16X: \
      stype1 = DDS_STYPE_COMPLEX16; \
      ptr1 = ddsStackTmp(expra->count, DDS_COMPLEX16_P);  \
      break; \
   default: \
     stype1 = expra->stype1; \
     ptr1   = expra->ptr; \
   }

#define SWAPPOST() \
   switch(expra->stype1) { \
   case DDS_STYPE_INTEGER2X: \
   case DDS_STYPE_UNSIGNED2X: \
      ddsSWAP2X(expra->ptr, ptr1, expra->count) \
      break; \
   case DDS_STYPE_INTEGER4X: \
   case DDS_STYPE_UNSIGNED4X: \
   case DDS_STYPE_FLOAT4X: \
      ddsSWAP4X(expra->ptr, ptr1, expra->count) \
      break; \
   case DDS_STYPE_INTEGER8X: \
   case DDS_STYPE_UNSIGNED8X: \
   case DDS_STYPE_FLOAT8X: \
      ddsSWAP8X(expra->ptr, ptr1, expra->count) \
      break; \
   case DDS_STYPE_COMPLEX8X: \
      ddsSWAP4X(expra->ptr, ptr1, 2 * expra->count) \
      break; \
   case DDS_STYPE_COMPLEX16X: \
      ddsSWAP8X(expra->ptr, ptr1, 2 * expra->count) \
      break; \
   default: \
      break; \
   } \
   ddsStackSet(&dds_tmp, reset);

#endif /* __DDS_SWAP_H */
