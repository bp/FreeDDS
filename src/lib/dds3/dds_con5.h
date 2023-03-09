
#ifndef __DDS_CON5_H
#define __DDS_CON5_H

/*
RCS-t=DDS, macros convert data between binary representations

$Id: dds_con5.h 164 2010-07-29 17:01:42Z user $
*/

/*******************************************************/
/* These functions convert data between types that are */
/* NOT implicitily supported by the host.              */
/*******************************************************/

/**********************************************************************/
/*  Summary of IBM functions for type conversion                      */
/*                                                                    */
/*  FUNCTION    LVALUE    RVALUE    comments                          */
/*  ddsIEEEIBM  F4        F4IBM                                       */
/*  ddsIBMIEEE  F4IBM     F4                                          */
/*  ddsIEEEIBM8 F8        F8IBM                                       */
/**********************************************************************/


#define CON5_FLOAT4_FLOAT4IBM(OUT, IN, COUNT) \
   {ddsIEEEIBM((DDS_IEEE4_OUT*)(OUT), (DDS_IBM4_IN*)(IN), \
      (int)(COUNT)); rc = DDS_CON_OK;}

#define CON5_FLOAT4X_FLOAT4IBM(OUT, IN, COUNT) \
   {ddsIEEEXIBM((DDS_IEEE4_OUT*)(OUT), (DDS_IBM4_IN*)(IN), \
      (int)(COUNT)); rc = DDS_CON_OK;}

#define CON5_FLOAT4IBM_FLOAT4(OUT, IN, COUNT) \
   {ddsIBMIEEE((DDS_IBM4_OUT*)(OUT), (DDS_IEEE4_IN*)(IN), \
      (int)(COUNT)); rc = DDS_CON_OK;}

#define CON5_FLOAT4IBM_FLOAT4X(OUT, IN, COUNT) \
   {ddsIBMIEEEX((DDS_IBM4_OUT*)(OUT), (DDS_IEEE4_IN*)(IN), \
      (int)(COUNT)); rc = DDS_CON_OK;}

#define CON5_COMPLEX8_COMPLEX8IBM(OUT, IN, COUNT) \
   {ddsIEEEIBM((DDS_IEEE4_OUT*)(OUT), (DDS_IBM4_IN*)(IN), \
      (int)(2 * (COUNT))); rc = DDS_CON_OK;}

#define CON5_COMPLEX8X_COMPLEX8IBM(OUT, IN, COUNT) \
   {ddsIEEEXIBM((DDS_IEEE4_OUT*)(OUT), (DDS_IBM4_IN*)(IN), \
      (int)(2 * (COUNT))); rc = DDS_CON_OK;}

#define CON5_COMPLEX8IBM_COMPLEX8(OUT, IN, COUNT) \
   {ddsIBMIEEE((DDS_IBM4_OUT*)(OUT), (DDS_IEEE4_IN*)(IN), \
      (int)(2 * (COUNT))); rc = DDS_CON_OK;}

#define CON5_COMPLEX8XIBM_COMPLEX8X(OUT, IN, COUNT) \
   {ddsIBMIEEEX((DDS_IBM4_OUT*)(OUT), (DDS_IEEE4_IN*)(IN), \
      (int)(2 * (COUNT))); rc = DDS_CON_OK;}

#ifdef HOST_FAKE8

   /* these macros assume "sun4" binary formats */

   #define CON5_INTEGER1_INTEGER8(OUT, IN, COUNT) \
      {size_t n; \
      DDS_INTEGER1_T *out_buf = (DDS_INTEGER1_T*)(OUT); \
      DDS_INTEGER1_T *in_buf = (DDS_INTEGER1_T*)(IN) + 7; \
      for(n = 0; n < (COUNT); n++) { \
         *(out_buf++) = *in_buf; in_buf += 8; \
      } rc = DDS_CON_OK;}

   #define CON5_INTEGER2_INTEGER8(OUT, IN, COUNT) \
      {size_t n; \
      DDS_INTEGER1_T *out_buf = (DDS_INTEGER1_T*)(OUT); \
      DDS_INTEGER2_T *in_buf = (DDS_INTEGER2_T*)(IN) + 6; \
      for(n = 0; n < (COUNT); n++) { \
         *(out_buf++) = *in_buf; in_buf += 4; \
      } rc = DDS_CON_OK;}

   #define CON5_INTEGER4_INTEGER8(OUT, IN, COUNT) \
      {size_t n; \
      DDS_INTEGER1_T *out_buf = (DDS_INTEGER1_T*)(OUT); \
      DDS_INTEGER4_T *in_buf = (DDS_INTEGER4_T*)(IN) + 4; \
      for(n = 0; n < (COUNT); n++) { \
         *(out_buf++) = *in_buf; in_buf += 2; \
      } rc = DDS_CON_OK;}

   #define CON5_INTEGER8_INTEGER1(OUT, IN, COUNT) \
      {size_t n; \
      DDS_INTEGER4_T *out_buf = (DDS_INTEGER4_T*)(OUT); \
      DDS_INTEGER1_T *in_buf = (DDS_INTEGER1_T*)(IN); \
      for(n = 0; n < (COUNT); n++) { \
         *(out_buf++) = (*in_buf < 0) ? 0xffffffff : 0; \
         *(out_buf++) = *(in_buf++); \
      } rc = DDS_CON_OK;}

   #define CON5_INTEGER8_INTEGER2(OUT, IN, COUNT) \
      {size_t n; \
      DDS_INTEGER4_T *out_buf = (DDS_INTEGER4_T*)(OUT); \
      DDS_INTEGER2_T *in_buf = (DDS_INTEGER2_T*)(IN); \
      for(n = 0; n < (COUNT); n++) { \
         *(out_buf++) = (*in_buf < 0) ? 0xffffffff : 0; \
         *(out_buf++) = *(in_buf++); \
      } rc = DDS_CON_OK;}

   #define CON5_INTEGER8_INTEGER4(OUT, IN, COUNT) \
      {size_t n; \
      DDS_INTEGER4_T *out_buf = (DDS_INTEGER4_T*)(OUT); \
      DDS_INTEGER4_T *in_buf = (DDS_INTEGER4_T*)(IN); \
      for(n = 0; n < (COUNT); n++) { \
         *(out_buf++) = (*in_buf < 0) ? 0xffffffff : 0; \
         *(out_buf++) = *(in_buf++); \
      } rc = DDS_CON_OK;}

   #define CON5_INTEGER8_UNSIGNED1(OUT, IN, COUNT) \
      {size_t n; \
      DDS_INTEGER4_T *out_buf = (DDS_INTEGER4_T*)(OUT); \
      DDS_UNSIGNED1_T *in_buf = (DDS_UNSIGNED1_T*)(IN); \
      for(n = 0; n < (COUNT); n++) { \
         *(out_buf++) = 0; *(out_buf++) = *(in_buf++); \
      } rc = DDS_CON_OK;}

   #define CON5_INTEGER8_UNSIGNED2(OUT, IN, COUNT) \
      {size_t n; \
      DDS_INTEGER4_T *out_buf = (DDS_INTEGER4_T*)(OUT); \
      DDS_UNSIGNED2_T *in_buf = (DDS_UNSIGNED2_T*)(IN); \
      for(n = 0; n < (COUNT); n++) { \
         *(out_buf++) = 0; *(out_buf++) = *(in_buf++); \
      } rc = DDS_CON_OK;}

   #define CON5_INTEGER8_UNSIGNED4(OUT, IN, COUNT) \
      {size_t n; \
      DDS_INTEGER4_T *out_buf = (DDS_INTEGER4_T*)(OUT); \
      DDS_UNSIGNED4_T *in_buf = (DDS_UNSIGNED4_T*)(IN); \
      for(n = 0; n < (COUNT); n++) { \
         *(out_buf++) = 0; *(out_buf++) = *(in_buf++); \
      } rc = DDS_CON_OK;}

#endif

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
   } rc = DDS_CON_OK; }

#define ddsSWAP4X(OUT, IN, COUNT) \
   {register int i, n = COUNT; \
   register DDS_INT4_OUT *out_buf = (DDS_INT4_OUT *) (OUT); \
   register DDS_INT4_IN  *in_buf  = (DDS_INT4_IN  *) (IN); \
   for(i=0;i<n;i++) { \
      *out_buf++ = CONVERT4(*in_buf); \
      in_buf++; \
   } rc = DDS_CON_OK; }

#define ddsSWAP8X(OUT, IN, COUNT) \
   {register int i, n = COUNT; \
   register DDS_INT4_OUT *out_buf = (DDS_INT4_OUT *) (OUT); \
   register DDS_INT4_IN  *in_buf  = (DDS_INT4_IN  *) (IN); \
   for(i=0;i<n;i++) { \
      out_buf[1] = CONVERT4(in_buf[0]); \
      out_buf[0] = CONVERT4(in_buf[1]); \
      out_buf += 2; in_buf += 2; \
   } rc = DDS_CON_OK; }


#define CON5_INTEGER2_INTEGER2X(OUT, IN, COUNT) \
   {ddsSWAP2X((DDS_INT2_OUT*)(OUT), (DDS_INT2_IN*)(IN), \
      (int)(COUNT)); rc = DDS_CON_OK;}

#define CON5_INTEGER2_UNSIGNED2X(OUT, IN, COUNT) \
   {ddsSWAP2X((DDS_INT2_OUT*)(OUT), (DDS_INT2_IN*)(IN), \
      (int)(COUNT)); rc = DDS_CON_OK;}

#define CON5_INTEGER2X_INTEGER2(OUT, IN, COUNT) \
   {ddsSWAP2X((DDS_INT2_OUT*)(OUT), (DDS_INT2_IN*)(IN), \
      (int)(COUNT)); rc = DDS_CON_OK;}

#define CON5_INTEGER2X_UNSIGNED2(OUT, IN, COUNT) \
   {ddsSWAP2X((DDS_INT2_OUT*)(OUT), (DDS_INT2_IN*)(IN), \
      (int)(COUNT)); rc = DDS_CON_OK;}

#define CON5_INTEGER4_INTEGER4X(OUT, IN, COUNT) \
   {ddsSWAP4X((DDS_INT4_OUT*)(OUT), (DDS_INT4_IN*)(IN), \
      (int)(COUNT)); rc = DDS_CON_OK;}

#define CON5_INTEGER4_UNSIGNED4X(OUT, IN, COUNT) \
   {ddsSWAP4X((DDS_INT4_OUT*)(OUT), (DDS_INT4_IN*)(IN), \
      (int)(COUNT)); rc = DDS_CON_OK;}

#define CON5_INTEGER4X_INTEGER4(OUT, IN, COUNT) \
   {ddsSWAP4X((DDS_INT4_OUT*)(OUT), (DDS_INT4_IN*)(IN), \
      (int)(COUNT)); rc = DDS_CON_OK;}

#define CON5_INTEGER4X_UNSIGNED4(OUT, IN, COUNT) \
   {ddsSWAP4X((DDS_INT4_OUT*)(OUT), (DDS_INT4_IN*)(IN), \
      (int)(COUNT)); rc = DDS_CON_OK;}

#define CON5_INTEGER8_INTEGER8X(OUT, IN, COUNT) \
   {ddsSWAP8X((DDS_INT8_OUT*)(OUT), (DDS_INT8_IN*)(IN), \
      (int)(COUNT)); rc = DDS_CON_OK;}

#define CON5_INTEGER8_UNSIGNED8X(OUT, IN, COUNT) \
   {ddsSWAP8X((DDS_INT8_OUT*)(OUT), (DDS_INT8_IN*)(IN), \
      (int)(COUNT)); rc = DDS_CON_OK;}

#define CON5_INTEGER8X_INTEGER8(OUT, IN, COUNT) \
   {ddsSWAP8X((DDS_INT8_OUT*)(OUT), (DDS_INT8_IN*)(IN), \
      (int)(COUNT)); rc = DDS_CON_OK;}

#define CON5_INTEGER8X_UNSIGNED8(OUT, IN, COUNT) \
   {ddsSWAP8X((DDS_INT8_OUT*)(OUT), (DDS_INT8_IN*)(IN), \
      (int)(COUNT)); rc = DDS_CON_OK;}

#define CON5_UNSIGNED2_INTEGER2X(OUT, IN, COUNT) \
   {ddsSWAP2X((DDS_INT2_OUT*)(OUT), (DDS_INT2_IN*)(IN), \
      (int)(COUNT)); rc = DDS_CON_OK;}

#define CON5_UNSIGNED2_UNSIGNED2X(OUT, IN, COUNT) \
   {ddsSWAP2X((DDS_INT2_OUT*)(OUT), (DDS_INT2_IN*)(IN), \
      (int)(COUNT)); rc = DDS_CON_OK;}

#define CON5_UNSIGNED2X_INTEGER2(OUT, IN, COUNT) \
   {ddsSWAP2X((DDS_INT2_OUT*)(OUT), (DDS_INT2_IN*)(IN), \
      (int)(COUNT)); rc = DDS_CON_OK;}

#define CON5_UNSIGNED2X_UNSIGNED2(OUT, IN, COUNT) \
   {ddsSWAP2X((DDS_INT2_OUT*)(OUT), (DDS_INT2_IN*)(IN), \
      (int)(COUNT)); rc = DDS_CON_OK;}

#define CON5_UNSIGNED4_INTEGER4X(OUT, IN, COUNT) \
   {ddsSWAP4X((DDS_INT4_OUT*)(OUT), (DDS_INT4_IN*)(IN), \
      (int)(COUNT)); rc = DDS_CON_OK;}

#define CON5_UNSIGNED4_UNSIGNED4X(OUT, IN, COUNT) \
   {ddsSWAP4X((DDS_INT4_OUT*)(OUT), (DDS_INT4_IN*)(IN), \
      (int)(COUNT)); rc = DDS_CON_OK;}

#define CON5_UNSIGNED4X_INTEGER4(OUT, IN, COUNT) \
   {ddsSWAP4X((DDS_INT4_OUT*)(OUT), (DDS_INT4_IN*)(IN), \
      (int)(COUNT)); rc = DDS_CON_OK;}

#define CON5_UNSIGNED4X_UNSIGNED4(OUT, IN, COUNT) \
   {ddsSWAP4X((DDS_INT4_OUT*)(OUT), (DDS_INT4_IN*)(IN), \
      (int)(COUNT)); rc = DDS_CON_OK;}

#define CON5_UNSIGNED8_INTEGER8X(OUT, IN, COUNT) \
   {ddsSWAP8X((DDS_INT8_OUT*)(OUT), (DDS_INT8_IN*)(IN), \
      (int)(COUNT)); rc = DDS_CON_OK;}

#define CON5_UNSIGNED8_UNSIGNED8X(OUT, IN, COUNT) \
   {ddsSWAP8X((DDS_INT8_OUT*)(OUT), (DDS_INT8_IN*)(IN), \
      (int)(COUNT)); rc = DDS_CON_OK;}

#define CON5_UNSIGNED8X_INTEGER8(OUT, IN, COUNT) \
   {ddsSWAP8X((DDS_INT8_OUT*)(OUT), (DDS_INT8_IN*)(IN), \
      (int)(COUNT)); rc = DDS_CON_OK;}

#define CON5_UNSIGNED8X_UNSIGNED8(OUT, IN, COUNT) \
   {ddsSWAP8X((DDS_INT8_OUT*)(OUT), (DDS_INT8_IN*)(IN), \
      (int)(COUNT)); rc = DDS_CON_OK;}

#define CON5_FLOAT4_FLOAT4X(OUT, IN, COUNT) \
   {ddsSWAP4X((DDS_INT4_OUT*)(OUT), (DDS_INT4_IN*)(IN), \
      (int)(COUNT)); rc = DDS_CON_OK;}

#define CON5_FLOAT4X_FLOAT4(OUT, IN, COUNT) \
   {ddsSWAP4X((DDS_INT4_OUT*)(OUT), (DDS_INT4_IN*)(IN), \
      (int)(COUNT)); rc = DDS_CON_OK;}

#define CON5_FLOAT8_FLOAT8X(OUT, IN, COUNT) \
   {ddsSWAP8X((DDS_INT4_OUT*)(OUT), (DDS_INT4_IN*)(IN), \
      (int)(COUNT)); rc = DDS_CON_OK;}

#define CON5_FLOAT8X_FLOAT8(OUT, IN, COUNT) \
   {ddsSWAP8X((DDS_INT4_OUT*)(OUT), (DDS_INT4_IN*)(IN), \
      (int)(COUNT)); rc = DDS_CON_OK;}

#define CON5_FLOAT8_FLOAT8IBM(OUT, IN, COUNT) \
   {ddsIEEEIBM8((DDS_IEEE8_OUT*)(OUT), (DDS_IBM8_IN*)(IN), \
      (int)(COUNT)); rc = DDS_CON_OK;}

#define CON5_COMPLEX8_COMPLEX8X(OUT, IN, COUNT) \
   {ddsSWAP4X((DDS_INT4_OUT*)(OUT), (DDS_INT4_IN*)(IN), \
      (int)(2 * (COUNT))); rc = DDS_CON_OK;}

#define CON5_COMPLEX8X_COMPLEX8(OUT, IN, COUNT) \
   {ddsSWAP4X((DDS_INT4_OUT*)(OUT), (DDS_INT4_IN*)(IN), \
      (int)(2 * (COUNT))); rc = DDS_CON_OK;}

#define CON5_COMPLEX16_COMPLEX16X(OUT, IN, COUNT) \
   {ddsSWAP8X((DDS_INT4_OUT*)(OUT), (DDS_INT4_IN*)(IN), \
      (int)(2 * (COUNT))); rc = DDS_CON_OK;}

#define CON5_COMPLEX16X_COMPLEX16(OUT, IN, COUNT) \
   {ddsSWAP8X((DDS_INT4_OUT*)(OUT), (DDS_INT4_IN*)(IN), \
      (int)(2 * (COUNT))); rc = DDS_CON_OK;}

#define CON5_COMPLEX16_COMPLEX16IBM(OUT, IN, COUNT) \
   {ddsSWAP8X((DDS_IEEE8_OUT*)(OUT), (DDS_IBM8_IN*)(IN), \
      (int)(2 * (COUNT))); rc = DDS_CON_OK;}

#endif /* __DDS_CON5_H */
