static char rcsid[]="$Id: evaluate.c 186 2010-09-27 19:29:14Z user $";
 
/*
RCS-t=DDS, evaluate DDS_EXPR expression sequence

*/

/**********************************************/
/***   Copyright (C) 1996  ,2009            ***/
/***   BP America, Houston, TX              ***/
/***   Randy Selzler, xx May 1996           ***/
/***   Jerry Ehlers, Jan 2009               ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_opcode.h"

#define DDS_CON2_EVALUATE

#include "dds_con2.h"
#include "dds_con3.h"
#include "dds_con4.h"
#include "dds_con5.h"
#include "ascii_ebcdic.h"

#define etype1 expra->stype1 - DDS_STYPE_MIN
#define etype2 expra->stype2 - DDS_STYPE_MIN
#define ptr1   expra->ptr
#define ptr2   expra->arg2.value.ptr

/***************************************************************************/
/* intrinsic types referenced from MIN to reduce switch compile addressing */
/***************************************************************************/
#define DDS_ETYPE_ASCII1        DDS_STYPE_ASCII1 - DDS_STYPE_MIN
#define DDS_ETYPE_EBCDIC1       DDS_STYPE_EBCDIC1 - DDS_STYPE_MIN
#define DDS_ETYPE_INTEGER1      DDS_STYPE_INTEGER1 - DDS_STYPE_MIN
#define DDS_ETYPE_INTEGER2      DDS_STYPE_INTEGER2 - DDS_STYPE_MIN
#define DDS_ETYPE_INTEGER2X     DDS_STYPE_INTEGER2X - DDS_STYPE_MIN
#define DDS_ETYPE_INTEGER4      DDS_STYPE_INTEGER4 - DDS_STYPE_MIN
#define DDS_ETYPE_INTEGER4X     DDS_STYPE_INTEGER4X - DDS_STYPE_MIN
#define DDS_ETYPE_INTEGER8      DDS_STYPE_INTEGER8 - DDS_STYPE_MIN
#define DDS_ETYPE_INTEGER8X     DDS_STYPE_INTEGER8X - DDS_STYPE_MIN
#define DDS_ETYPE_UNSIGNED1     DDS_STYPE_UNSIGNED1 - DDS_STYPE_MIN
#define DDS_ETYPE_UNSIGNED2     DDS_STYPE_UNSIGNED2 - DDS_STYPE_MIN
#define DDS_ETYPE_UNSIGNED2X    DDS_STYPE_UNSIGNED2X - DDS_STYPE_MIN
#define DDS_ETYPE_UNSIGNED4     DDS_STYPE_UNSIGNED4 - DDS_STYPE_MIN
#define DDS_ETYPE_UNSIGNED4X    DDS_STYPE_UNSIGNED4X - DDS_STYPE_MIN
#define DDS_ETYPE_UNSIGNED8     DDS_STYPE_UNSIGNED8 - DDS_STYPE_MIN
#define DDS_ETYPE_UNSIGNED8X    DDS_STYPE_UNSIGNED8X - DDS_STYPE_MIN
#define DDS_ETYPE_FLOAT4        DDS_STYPE_FLOAT4 - DDS_STYPE_MIN
#define DDS_ETYPE_FLOAT4X       DDS_STYPE_FLOAT4X - DDS_STYPE_MIN
#define DDS_ETYPE_FLOAT4IBM     DDS_STYPE_FLOAT4IBM - DDS_STYPE_MIN
#define DDS_ETYPE_FLOAT8        DDS_STYPE_FLOAT8 - DDS_STYPE_MIN
#define DDS_ETYPE_FLOAT8X       DDS_STYPE_FLOAT8X - DDS_STYPE_MIN
#define DDS_ETYPE_FLOAT8IBM     DDS_STYPE_FLOAT8IBM - DDS_STYPE_MIN
#define DDS_ETYPE_FLOAT16       DDS_STYPE_FLOAT16 - DDS_STYPE_MIN
#define DDS_ETYPE_FLOAT16X      DDS_STYPE_FLOAT16X - DDS_STYPE_MIN
#define DDS_ETYPE_COMPLEX8      DDS_STYPE_COMPLEX8 - DDS_STYPE_MIN
#define DDS_ETYPE_COMPLEX8X     DDS_STYPE_COMPLEX8X - DDS_STYPE_MIN
#define DDS_ETYPE_COMPLEX8IBM   DDS_STYPE_COMPLEX8IBM - DDS_STYPE_MIN
#define DDS_ETYPE_COMPLEX16     DDS_STYPE_COMPLEX16 - DDS_STYPE_MIN
#define DDS_ETYPE_COMPLEX16X    DDS_STYPE_COMPLEX16X - DDS_STYPE_MIN
#define DDS_ETYPE_COMPLEX16IBM  DDS_STYPE_COMPLEX16IBM - DDS_STYPE_MIN
#define DDS_ETYPE_COMPLEX32     DDS_STYPE_COMPLEX32 - DDS_STYPE_MIN
#define DDS_ETYPE_COMPLEX32X    DDS_STYPE_COMPLEX32X - DDS_STYPE_MIN

void ddsEvaluate(
   DDS_EXPR *first_expr)
{
   DDS_CON_STATUS rc;
   DDS_EXPRA *expra;
   DDS_EXPR *branch;
   DDS_EXPR4 *expr = (DDS_EXPR4*)first_expr;
   DDS_EXPR4 *save;

   HOST_FLOAT_T     FLOAT_TS;
   HOST_DOUBLE_T    DOUBLE_TS;
   HOST_LDOUBLE_T   LDOUBLE_TS;

   /*****************************************************/
   /*****************************************************/
   /***                                               ***/
   /***  OPCODE SWITCH TO DECODE EXPRESSION SEQUENCE  ***/
   /***                                               ***/
   /*****************************************************/
   /*****************************************************/

   evaluate:
   switch(expr->opcode) {
      case DDS_ASSIGN_OP:
         /* assign, based upon operand types */
         expra = (DDS_EXPRA*)expr;
         switch(etype1) {
         case DDS_ETYPE_ASCII1:
            if(etype2 == DDS_ETYPE_EBCDIC1) {
               CON2_ASCII1_EBCDIC1(VV, A, A, ptr1, ptr2, expra->count)
            } else {
               memcpy(ptr1, ptr2, expra->count);
            }
            break;
         case DDS_ETYPE_EBCDIC1:
            if(etype2 == DDS_ETYPE_ASCII1) {
               CON2_EBCDIC1_ASCII1(VV, A, A, ptr1, ptr2, expra->count)
            } else {
               memcpy(ptr1, ptr2, expra->count);
            }
            break;
         case DDS_ETYPE_INTEGER1:
         case DDS_ETYPE_UNSIGNED1:
            switch(etype2) {
            case DDS_ETYPE_INTEGER1:
            case DDS_ETYPE_UNSIGNED1:
               memcpy(ptr1, ptr2, expra->count);
               break;
            case DDS_ETYPE_INTEGER2:
            case DDS_ETYPE_UNSIGNED2:
               CON2_INTEGER1_INTEGER2(VV, A, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2X:
            case DDS_ETYPE_UNSIGNED2X:
               CON2_INTEGER1_INTEGER2X(VV, A, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4:
            case DDS_ETYPE_UNSIGNED4:
               CON2_INTEGER1_INTEGER4(VV, A, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4X:
            case DDS_ETYPE_UNSIGNED4X:
               CON2_INTEGER1_INTEGER4X(VV, A, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8:
            case DDS_ETYPE_UNSIGNED8:
               CON2_INTEGER1_INTEGER8(VV, A, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8X:
            case DDS_ETYPE_UNSIGNED8X:
               CON2_INTEGER1_INTEGER8X(VV, A, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4:
               CON2_INTEGER1_FLOAT4(VV, A, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4X:
               CON2_INTEGER1_FLOAT4X(VV, A, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4IBM:
               CON2_INTEGER1_FLOAT4IBM(VV, A, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8:
               CON2_INTEGER1_FLOAT8(VV, A, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8X:
               CON2_INTEGER1_FLOAT8X(VV, A, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8IBM:
               CON2_INTEGER1_FLOAT8IBM(VV, A, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16:
               CON2_INTEGER1_FLOAT16(VV, A, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16X:
               CON2_INTEGER1_FLOAT16X(VV, A, N, ptr1, ptr2, expra->count)
               break;
            default:
               ddsPanic("ddsEvaluate: invalid assign, INTEGER1= (PTYPE)%i\n",
                  (int)expra->stype2);
            }
            break;
         case DDS_ETYPE_INTEGER2:
         case DDS_ETYPE_UNSIGNED2:
            switch(etype2) {
            case DDS_ETYPE_INTEGER1:
               CON2_INTEGER2_INTEGER1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED1:
               CON2_INTEGER2_UNSIGNED1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2:
            case DDS_ETYPE_UNSIGNED2:
               memcpy(ptr1, ptr2, 2 * expra->count);
               break;
            case DDS_ETYPE_INTEGER2X:
            case DDS_ETYPE_UNSIGNED2X:
               CON2_INTEGER2_INTEGER2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4:
            case DDS_ETYPE_UNSIGNED4:
               CON2_INTEGER2_INTEGER4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4X:
            case DDS_ETYPE_UNSIGNED4X:
               CON2_INTEGER2_INTEGER4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8:
            case DDS_ETYPE_UNSIGNED8:
               CON2_INTEGER2_INTEGER8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8X:
            case DDS_ETYPE_UNSIGNED8X:
               CON2_INTEGER2_INTEGER8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4:
               CON2_INTEGER2_FLOAT4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4X:
               CON2_INTEGER2_FLOAT4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4IBM:
               CON2_INTEGER2_FLOAT4IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8:
               CON2_INTEGER2_FLOAT8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8X:
               CON2_INTEGER2_FLOAT8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8IBM:
               CON2_INTEGER2_FLOAT8IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16:
               CON2_INTEGER2_FLOAT16(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16X:
               CON2_INTEGER2_FLOAT16X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            default:
               ddsPanic("ddsEvaluate: invalid assign, INTEGER2= (PTYPE)%i\n",
                  (int)expra->stype2);
            }
            break;
         case DDS_ETYPE_INTEGER2X:
         case DDS_ETYPE_UNSIGNED2X:
            switch(etype2) {
            case DDS_ETYPE_INTEGER1:
               CON2_INTEGER2X_INTEGER1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED1:
               CON2_INTEGER2X_UNSIGNED1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2:
            case DDS_ETYPE_UNSIGNED2:
               CON2_INTEGER2X_INTEGER2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2X:
            case DDS_ETYPE_UNSIGNED2X:
               memcpy(ptr1, ptr2, 2 * expra->count);
              break;
            case DDS_ETYPE_INTEGER4:
            case DDS_ETYPE_UNSIGNED4:
               CON2_INTEGER2X_INTEGER4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4X:
            case DDS_ETYPE_UNSIGNED4X:
               CON2_INTEGER2X_INTEGER4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8:
            case DDS_ETYPE_UNSIGNED8:
               CON2_INTEGER2X_INTEGER8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8X:
            case DDS_ETYPE_UNSIGNED8X:
               CON2_INTEGER2X_INTEGER8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4:
               CON2_INTEGER2X_FLOAT4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4X:
               CON2_INTEGER2X_FLOAT4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4IBM:
               CON2_INTEGER2X_FLOAT4IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8:
               CON2_INTEGER2X_FLOAT8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8X:
               CON2_INTEGER2X_FLOAT8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8IBM:
               CON2_INTEGER2X_FLOAT8IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16:
               CON2_INTEGER2X_FLOAT16(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16X:
               CON2_INTEGER2X_FLOAT16X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            default:
               ddsPanic("ddsEvaluate: invalid assign, INTEGER2X= (PTYPE)%i\n",
                  (int)expra->stype2);
            }
            break;
         case DDS_ETYPE_INTEGER4:
         case DDS_ETYPE_UNSIGNED4:
            switch(etype2) {
            case DDS_ETYPE_INTEGER1:
               CON2_INTEGER4_INTEGER1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED1:
               CON2_INTEGER4_UNSIGNED1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2:
               CON2_INTEGER4_INTEGER2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2:
               CON2_INTEGER4_UNSIGNED2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2X:
               CON2_INTEGER4_INTEGER2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2X:
               CON2_INTEGER4_UNSIGNED2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4:
            case DDS_ETYPE_UNSIGNED4:
               memcpy(ptr1, ptr2, 4 * expra->count);
               break;
            case DDS_ETYPE_INTEGER4X:
            case DDS_ETYPE_UNSIGNED4X:
               CON2_INTEGER4_INTEGER4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8:
            case DDS_ETYPE_UNSIGNED8:
               CON2_INTEGER4_INTEGER8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8X:
            case DDS_ETYPE_UNSIGNED8X:
               CON2_INTEGER4_INTEGER8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4:
               CON2_INTEGER4_FLOAT4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4X:
               CON2_INTEGER4_FLOAT4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4IBM:
               CON2_INTEGER4_FLOAT4IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8:
               CON2_INTEGER4_FLOAT8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8X:
               CON2_INTEGER4_FLOAT8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8IBM:
               CON2_INTEGER4_FLOAT8IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16:
               CON2_INTEGER4_FLOAT16(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16X:
               CON2_INTEGER4_FLOAT16X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            default:
               ddsPanic("ddsEvaluate: invalid assign, INTEGER4= (PTYPE)%i\n",
                  (int)expra->stype2);
            }
            break;
         case DDS_ETYPE_INTEGER4X:
         case DDS_ETYPE_UNSIGNED4X:
            switch(etype2) {
            case DDS_ETYPE_INTEGER1:
               CON2_INTEGER4X_INTEGER1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED1:
               CON2_INTEGER4X_UNSIGNED1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2:
               CON2_INTEGER4X_INTEGER2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2:
               CON2_INTEGER4X_UNSIGNED2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2X:
               CON2_INTEGER4X_INTEGER2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2X:
               CON2_INTEGER4X_UNSIGNED2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4:
            case DDS_ETYPE_UNSIGNED4:
               CON2_INTEGER4X_INTEGER4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4X:
            case DDS_ETYPE_UNSIGNED4X:
	       memcpy(ptr1, ptr2, 4 * expra->count);
	       break;
            case DDS_ETYPE_INTEGER8:
            case DDS_ETYPE_UNSIGNED8:
               CON2_INTEGER4X_INTEGER8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8X:
            case DDS_ETYPE_UNSIGNED8X:
               CON2_INTEGER4X_INTEGER8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4:
               CON2_INTEGER4X_FLOAT4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4X:
               CON2_INTEGER4X_FLOAT4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4IBM:
               CON2_INTEGER4X_FLOAT4IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8:
               CON2_INTEGER4X_FLOAT8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8X:
               CON2_INTEGER4X_FLOAT8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8IBM:
               CON2_INTEGER4X_FLOAT8IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16:
               CON2_INTEGER4X_FLOAT16(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16X:
               CON2_INTEGER4X_FLOAT16X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            default:
               ddsPanic("ddsEvaluate: invalid assign, INTEGER4X= (PTYPE)%i\n",
                  (int)expra->stype2);
            }
            break;
         case DDS_ETYPE_INTEGER8:
         case DDS_ETYPE_UNSIGNED8:
            switch(etype2) {
            case DDS_ETYPE_INTEGER1:
               CON2_INTEGER8_INTEGER1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED1:
               CON2_INTEGER8_UNSIGNED1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2:
               CON2_INTEGER8_INTEGER2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2X:
               CON2_INTEGER8_INTEGER2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2:
               CON2_INTEGER8_UNSIGNED2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2X:
               CON2_INTEGER8_UNSIGNED2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4:
               CON2_INTEGER8_INTEGER4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4X:
               CON2_INTEGER8_INTEGER4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4:
               CON2_INTEGER8_UNSIGNED4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4X:
               CON2_INTEGER8_UNSIGNED4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8:
            case DDS_ETYPE_UNSIGNED8:
               memcpy(ptr1, ptr2, 8 * expra->count);
               break;
            case DDS_ETYPE_INTEGER8X:
            case DDS_ETYPE_UNSIGNED8X:
               CON2_INTEGER8_INTEGER8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4:
               CON2_INTEGER8_FLOAT4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4X:
               CON2_INTEGER8_FLOAT4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4IBM:
               CON2_INTEGER8_FLOAT4IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8:
               CON2_INTEGER8_FLOAT8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8X:
               CON2_INTEGER8_FLOAT8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8IBM:
               CON2_INTEGER8_FLOAT8IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16:
               CON2_INTEGER8_FLOAT16(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16X:
               CON2_INTEGER8_FLOAT16X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            default:
               ddsPanic("ddsEvaluate: invalid assign, INTEGER8= (PTYPE)%i\n",
                  (int)expra->stype2);
            }
            break;
         case DDS_ETYPE_INTEGER8X:
         case DDS_ETYPE_UNSIGNED8X:
            switch(etype2) {
            case DDS_ETYPE_INTEGER1:
               CON2_INTEGER8X_INTEGER1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED1:
               CON2_INTEGER8X_UNSIGNED1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2:
               CON2_INTEGER8X_INTEGER2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2X:
               CON2_INTEGER8X_INTEGER2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2:
               CON2_INTEGER8X_UNSIGNED2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2X:
               CON2_INTEGER8X_UNSIGNED2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4:
               CON2_INTEGER8X_INTEGER4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4X:
               CON2_INTEGER8X_INTEGER4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4:
               CON2_INTEGER8X_UNSIGNED4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4X:
               CON2_INTEGER8X_UNSIGNED4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8:
            case DDS_ETYPE_UNSIGNED8:
               CON2_INTEGER8X_INTEGER8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8X:
            case DDS_ETYPE_UNSIGNED8X:
               memcpy(ptr1, ptr2, 8 * expra->count);
               break;
            case DDS_ETYPE_FLOAT4:
               CON2_INTEGER8X_FLOAT4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4X:
               CON2_INTEGER8X_FLOAT4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4IBM:
               CON2_INTEGER8X_FLOAT4IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8:
               CON2_INTEGER8X_FLOAT8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8X:
               CON2_INTEGER8X_FLOAT8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8IBM:
               CON2_INTEGER8X_FLOAT8IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16:
               CON2_INTEGER8X_FLOAT16(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16X:
               CON2_INTEGER8X_FLOAT16X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            default:
               ddsPanic("ddsEvaluate: invalid assign, INTEGER8X= (PTYPE)%i\n",
                  (int)expra->stype2);
            }
            break;
         case DDS_ETYPE_FLOAT4:
            switch(etype2) {
            case DDS_ETYPE_INTEGER1:
               CON2_FLOAT4_INTEGER1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED1:
               CON2_FLOAT4_UNSIGNED1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2:
               CON2_FLOAT4_INTEGER2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2X:
               CON2_FLOAT4_INTEGER2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2:
               CON2_FLOAT4_UNSIGNED2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2X:
               CON2_FLOAT4_UNSIGNED2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4:
               CON2_FLOAT4_INTEGER4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4X:
               CON2_FLOAT4_INTEGER4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4:
               CON2_FLOAT4_UNSIGNED4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4X:
               CON2_FLOAT4_UNSIGNED4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8:
               CON2_FLOAT4_INTEGER8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8X:
               CON2_FLOAT4_INTEGER8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED8:
               CON2_FLOAT4_UNSIGNED8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED8X:
               CON2_FLOAT4_UNSIGNED8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4:
               memcpy(ptr1, ptr2, 4 * expra->count);
               break;
            case DDS_ETYPE_FLOAT4X:
               CON2_FLOAT4_FLOAT4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4IBM:
               CON2_FLOAT4_FLOAT4IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8:
               CON2_FLOAT4_FLOAT8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8X:
               CON2_FLOAT4_FLOAT8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8IBM:
               CON2_FLOAT4_FLOAT8IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16:
               CON2_FLOAT4_FLOAT16(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16X:
               CON2_FLOAT4_FLOAT16X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            default:
               ddsPanic("ddsEvaluate: invalid assign, FLOAT4= (PTYPE)%i\n",
                  (int)expra->stype2);
            }
            break;
         case DDS_ETYPE_FLOAT4X:
            switch(etype2) {
            case DDS_ETYPE_INTEGER1:
               CON2_FLOAT4X_INTEGER1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED1:
               CON2_FLOAT4X_UNSIGNED1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2:
               CON2_FLOAT4X_INTEGER2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2X:
               CON2_FLOAT4X_INTEGER2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2:
               CON2_FLOAT4X_UNSIGNED2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2X:
               CON2_FLOAT4X_UNSIGNED2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4:
               CON2_FLOAT4X_INTEGER4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4X:
               CON2_FLOAT4X_INTEGER4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4:
               CON2_FLOAT4X_UNSIGNED4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4X:
               CON2_FLOAT4X_UNSIGNED4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8:
               CON2_FLOAT4X_INTEGER8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8X:
               CON2_FLOAT4X_INTEGER8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED8:
               CON2_FLOAT4X_UNSIGNED8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED8X:
               CON2_FLOAT4X_UNSIGNED8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4:
               CON2_FLOAT4X_FLOAT4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4X:
               memcpy(ptr1, ptr2, 4 * expra->count);
               break;
            case DDS_ETYPE_FLOAT4IBM:
               CON2_FLOAT4X_FLOAT4IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8:
               CON2_FLOAT4X_FLOAT8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8X:
               CON2_FLOAT4X_FLOAT8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8IBM:
               CON2_FLOAT4X_FLOAT8IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16:
               CON2_FLOAT4X_FLOAT16(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16X:
               CON2_FLOAT4X_FLOAT16X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            default:
               ddsPanic("ddsEvaluate: invalid assign, FLOAT4X= (PTYPE)%i\n",
                  (int)expra->stype2);
            }
            break;
         case DDS_ETYPE_FLOAT4IBM:
            switch(etype2) {
            case DDS_ETYPE_INTEGER1:
               CON2_FLOAT4IBM_INTEGER1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED1:
               CON2_FLOAT4IBM_UNSIGNED1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2:
               CON2_FLOAT4IBM_INTEGER2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2X:
               CON2_FLOAT4IBM_INTEGER2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2:
               CON2_FLOAT4IBM_UNSIGNED2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2X:
               CON2_FLOAT4IBM_UNSIGNED2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4:
               CON2_FLOAT4IBM_INTEGER4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4X:
               CON2_FLOAT4IBM_INTEGER4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4:
               CON2_FLOAT4IBM_UNSIGNED4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4X:
               CON2_FLOAT4IBM_UNSIGNED4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8:
               CON2_FLOAT4IBM_INTEGER8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8X:
               CON2_FLOAT4IBM_INTEGER8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED8:
               CON2_FLOAT4IBM_UNSIGNED8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED8X:
               CON2_FLOAT4IBM_UNSIGNED8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4:
               CON2_FLOAT4IBM_FLOAT4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4X:
               CON2_FLOAT4IBM_FLOAT4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4IBM:
               memcpy(ptr1, ptr2, 4 * expra->count);
               break;
            case DDS_ETYPE_FLOAT8:
               CON2_FLOAT4IBM_FLOAT8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8X:
               CON2_FLOAT4IBM_FLOAT8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8IBM:
               CON2_FLOAT4IBM_FLOAT8IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16:
               CON2_FLOAT4IBM_FLOAT16(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16X:
               CON2_FLOAT4IBM_FLOAT16X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            default:
               ddsPanic("ddsEvaluate: invalid assign, FLOAT4IBM= (PTYPE)%i\n",
                  (int)expra->stype2);
            }
            break;
         case DDS_ETYPE_FLOAT8:
            switch(etype2) {
            case DDS_ETYPE_INTEGER1:
               CON2_FLOAT8_INTEGER1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED1:
               CON2_FLOAT8_UNSIGNED1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2:
               CON2_FLOAT8_INTEGER2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2X:
               CON2_FLOAT8_INTEGER2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2:
               CON2_FLOAT8_UNSIGNED2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2X:
               CON2_FLOAT8_UNSIGNED2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4:
               CON2_FLOAT8_INTEGER4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4X:
               CON2_FLOAT8_INTEGER4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4:
               CON2_FLOAT8_UNSIGNED4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4X:
               CON2_FLOAT8_UNSIGNED4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8:
               CON2_FLOAT8_INTEGER8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8X:
               CON2_FLOAT8_INTEGER8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED8:
               CON2_FLOAT8_UNSIGNED8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED8X:
               CON2_FLOAT8_UNSIGNED8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4:
               CON2_FLOAT8_FLOAT4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4X:
               CON2_FLOAT8_FLOAT4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4IBM:
               CON2_FLOAT8_FLOAT4IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8:
               memcpy(ptr1, ptr2, 8 * expra->count);
               break;
            case DDS_ETYPE_FLOAT8X:
               CON2_FLOAT8_FLOAT8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8IBM:
               CON2_FLOAT8_FLOAT8IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16:
               CON2_FLOAT8_FLOAT16(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16X:
               CON2_FLOAT8_FLOAT16X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            default:
               ddsPanic("ddsEvaluate: invalid assign, FLOAT8= (PTYPE)%i\n",
                  (int)expra->stype2);
            }
            break;
         case DDS_ETYPE_FLOAT8X:
            switch(etype2) {
            case DDS_ETYPE_INTEGER1:
               CON2_FLOAT8X_INTEGER1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED1:
               CON2_FLOAT8X_UNSIGNED1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2:
               CON2_FLOAT8X_INTEGER2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2X:
               CON2_FLOAT8X_INTEGER2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2:
               CON2_FLOAT8X_UNSIGNED2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2X:
               CON2_FLOAT8X_UNSIGNED2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4:
               CON2_FLOAT8X_INTEGER4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4X:
               CON2_FLOAT8X_INTEGER4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4:
               CON2_FLOAT8X_UNSIGNED4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4X:
               CON2_FLOAT8X_UNSIGNED4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8:
               CON2_FLOAT8X_INTEGER8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8X:
               CON2_FLOAT8X_INTEGER8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED8:
               CON2_FLOAT8X_UNSIGNED8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED8X:
               CON2_FLOAT8X_UNSIGNED8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4:
               CON2_FLOAT8X_FLOAT4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4X:
               CON2_FLOAT8X_FLOAT4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4IBM:
               CON2_FLOAT8X_FLOAT4IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8:
               CON2_FLOAT8X_FLOAT8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8X:
               memcpy(ptr1, ptr2, 8 * expra->count);
               break;
            case DDS_ETYPE_FLOAT8IBM:
               CON2_FLOAT8X_FLOAT8IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16:
               CON2_FLOAT8X_FLOAT16(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16X:
               CON2_FLOAT8X_FLOAT16X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            default:
               ddsPanic("ddsEvaluate: invalid assign, FLOAT8X= (PTYPE)%i\n",
                  (int)expra->stype2);
            }
            break;
         case DDS_ETYPE_FLOAT8IBM:
            switch(etype2) {
            case DDS_ETYPE_INTEGER1:
               CON2_FLOAT8IBM_INTEGER1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED1:
               CON2_FLOAT8IBM_UNSIGNED1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2:
               CON2_FLOAT8IBM_INTEGER2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2X:
               CON2_FLOAT8IBM_INTEGER2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2:
               CON2_FLOAT8IBM_UNSIGNED2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2X:
               CON2_FLOAT8IBM_UNSIGNED2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4:
               CON2_FLOAT8IBM_INTEGER4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4X:
               CON2_FLOAT8IBM_INTEGER4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4:
               CON2_FLOAT8IBM_UNSIGNED4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4X:
               CON2_FLOAT8IBM_UNSIGNED4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8:
               CON2_FLOAT8IBM_INTEGER8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8X:
               CON2_FLOAT8IBM_INTEGER8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED8:
               CON2_FLOAT8IBM_UNSIGNED8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED8X:
               CON2_FLOAT8IBM_UNSIGNED8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4:
               CON2_FLOAT8IBM_FLOAT4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4X:
               CON2_FLOAT8IBM_FLOAT4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4IBM:
               CON2_FLOAT8IBM_FLOAT4IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8:
               CON2_FLOAT8IBM_FLOAT8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8X:
               CON2_FLOAT8IBM_FLOAT8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8IBM:
               memcpy(ptr1, ptr2, 8 * expra->count);
               break;
            case DDS_ETYPE_FLOAT16:
               CON2_FLOAT8IBM_FLOAT16(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16X:
               CON2_FLOAT8IBM_FLOAT16X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            default:
               ddsPanic("ddsEvaluate: invalid assign, FLOAT8IBM= (PTYPE)%i\n",
                  (int)expra->stype2);
            }
            break;
         case DDS_ETYPE_FLOAT16:
            switch(etype2) {
            case DDS_ETYPE_INTEGER1:
               CON2_FLOAT16_INTEGER1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED1:
               CON2_FLOAT16_UNSIGNED1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2:
               CON2_FLOAT16_INTEGER2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2X:
               CON2_FLOAT16_INTEGER2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2:
               CON2_FLOAT16_UNSIGNED2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2X:
               CON2_FLOAT16_UNSIGNED2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4:
               CON2_FLOAT16_INTEGER4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4X:
               CON2_FLOAT16_INTEGER4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4:
               CON2_FLOAT16_UNSIGNED4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4X:
               CON2_FLOAT16_UNSIGNED4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8:
               CON2_FLOAT16_INTEGER8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8X:
               CON2_FLOAT16_INTEGER8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED8:
               CON2_FLOAT16_UNSIGNED8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED8X:
               CON2_FLOAT16_UNSIGNED8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4:
               CON2_FLOAT16_FLOAT4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4X:
               CON2_FLOAT16_FLOAT4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4IBM:
               CON2_FLOAT16_FLOAT4IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8:
               CON2_FLOAT16_FLOAT8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8X:
               CON2_FLOAT16_FLOAT8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8IBM:
               CON2_FLOAT16_FLOAT8IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16:
               memcpy(ptr1, ptr2, 16 * expra->count);
               break;
            case DDS_ETYPE_FLOAT16X:
               CON2_FLOAT16_FLOAT16X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            default:
               ddsPanic("ddsEvaluate: invalid assign, FLOAT16= (PTYPE)%i\n",
                  (int)expra->stype2);
            }
            break;
         case DDS_ETYPE_FLOAT16X:
            switch(etype2) {
            case DDS_ETYPE_INTEGER1:
               CON2_FLOAT16X_INTEGER1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED1:
               CON2_FLOAT16X_UNSIGNED1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2:
               CON2_FLOAT16X_INTEGER2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2X:
               CON2_FLOAT16X_INTEGER2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2:
               CON2_FLOAT16X_UNSIGNED2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2X:
               CON2_FLOAT16X_UNSIGNED2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4:
               CON2_FLOAT16X_INTEGER4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4X:
               CON2_FLOAT16X_INTEGER4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4:
               CON2_FLOAT16X_UNSIGNED4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4X:
               CON2_FLOAT16X_UNSIGNED4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8:
               CON2_FLOAT16X_INTEGER8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8X:
               CON2_FLOAT16X_INTEGER8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED8:
               CON2_FLOAT16X_UNSIGNED8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED8X:
               CON2_FLOAT16X_UNSIGNED8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4:
               CON2_FLOAT16X_FLOAT4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4X:
               CON2_FLOAT16X_FLOAT4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4IBM:
               CON2_FLOAT16X_FLOAT4IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8:
               CON2_FLOAT16X_FLOAT8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8X:
               CON2_FLOAT16X_FLOAT8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8IBM:
               CON2_FLOAT16X_FLOAT8IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16:
               CON2_FLOAT16X_FLOAT16(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16X:
               memcpy(ptr1, ptr2, 16 * expra->count);
               break;
            default:
               ddsPanic("ddsEvaluate: invalid assign, FLOAT16X= (PTYPE)%i\n",
                  (int)expra->stype2);
            }
            break;
         case DDS_ETYPE_COMPLEX8:
            switch(etype2) {
            case DDS_ETYPE_INTEGER1:
               CON2_COMPLEX8_INTEGER1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED1:
               CON2_COMPLEX8_UNSIGNED1(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2:
               CON2_COMPLEX8_INTEGER2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2X:
               CON2_COMPLEX8_INTEGER2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2:
               CON2_COMPLEX8_UNSIGNED2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2X:
               CON2_COMPLEX8_UNSIGNED2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4:
               CON2_COMPLEX8_INTEGER4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4X:
               CON2_COMPLEX8_INTEGER4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4:
               CON2_COMPLEX8_UNSIGNED4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4X:
               CON2_COMPLEX8_UNSIGNED4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8:
               CON2_COMPLEX8_INTEGER8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8X:
               CON2_COMPLEX8_INTEGER8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED8:
               CON2_COMPLEX8_UNSIGNED8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED8X:
               CON2_COMPLEX8_UNSIGNED8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4:
               CON2_COMPLEX8_FLOAT4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4X:
               CON2_COMPLEX8_FLOAT4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4IBM:
               CON2_COMPLEX8_FLOAT4IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8:
               CON2_COMPLEX8_FLOAT8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8X:
               CON2_COMPLEX8_FLOAT8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8IBM:
               CON2_COMPLEX8_FLOAT8IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16:
               CON2_COMPLEX8_FLOAT16(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16X:
               CON2_COMPLEX8_FLOAT16X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX8:
               memcpy(ptr1, ptr2, 8 * expra->count);
               break;
            case DDS_ETYPE_COMPLEX8X:
               CON2_COMPLEX8_COMPLEX8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX8IBM:
               CON2_COMPLEX8_COMPLEX8IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX16:
               CON2_COMPLEX8_COMPLEX16(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX16X:
               CON2_COMPLEX8_COMPLEX16X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX16IBM:
               CON2_COMPLEX8_COMPLEX16IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX32:
               CON2_COMPLEX8_COMPLEX32(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX32X:
               CON2_COMPLEX8_COMPLEX32X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            default:
               ddsPanic("ddsEvaluate: invalid assign, COMPLEX8= (PTYPE)%i\n",
                  (int)expra->stype2);
            }
            break;
         case DDS_ETYPE_COMPLEX8X:
            switch(etype2) {
            case DDS_ETYPE_INTEGER1:
               CON2_COMPLEX8X_INTEGER1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED1:
               CON2_COMPLEX8X_UNSIGNED1(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2:
               CON2_COMPLEX8X_INTEGER2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2X:
               CON2_COMPLEX8X_INTEGER2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2:
               CON2_COMPLEX8X_UNSIGNED2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2X:
               CON2_COMPLEX8X_UNSIGNED2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4:
               CON2_COMPLEX8X_INTEGER4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4X:
               CON2_COMPLEX8X_INTEGER4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4:
               CON2_COMPLEX8X_UNSIGNED4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4X:
               CON2_COMPLEX8X_UNSIGNED4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8:
               CON2_COMPLEX8X_INTEGER8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8X:
               CON2_COMPLEX8X_INTEGER8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED8:
               CON2_COMPLEX8X_UNSIGNED8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED8X:
               CON2_COMPLEX8X_UNSIGNED8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4:
               CON2_COMPLEX8X_FLOAT4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4X:
               CON2_COMPLEX8X_FLOAT4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4IBM:
               CON2_COMPLEX8X_FLOAT4IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8:
               CON2_COMPLEX8X_FLOAT8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8X:
               CON2_COMPLEX8X_FLOAT8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8IBM:
               CON2_COMPLEX8X_FLOAT8IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16:
               CON2_COMPLEX8X_FLOAT16(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16X:
               CON2_COMPLEX8X_FLOAT16X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX8:
               CON2_COMPLEX8X_COMPLEX8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX8X:
               memcpy(ptr1, ptr2, 8 * expra->count);
               break;
            case DDS_ETYPE_COMPLEX8IBM:
               CON2_COMPLEX8X_COMPLEX8IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX16:
               CON2_COMPLEX8X_COMPLEX16(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX16X:
               CON2_COMPLEX8X_COMPLEX16X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX16IBM:
               CON2_COMPLEX8X_COMPLEX16IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX32:
               CON2_COMPLEX8X_COMPLEX32(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX32X:
               CON2_COMPLEX8X_COMPLEX32X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            default:
               ddsPanic("ddsEvaluate: invalid assign, COMPLEX8X= (PTYPE)%i\n",
                  (int)expra->stype2);
            }
            break;
         case DDS_ETYPE_COMPLEX8IBM:
            switch(etype2) {
            case DDS_ETYPE_INTEGER1:
               CON2_COMPLEX8IBM_INTEGER1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED1:
               CON2_COMPLEX8IBM_UNSIGNED1(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2:
               CON2_COMPLEX8IBM_INTEGER2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2X:
               CON2_COMPLEX8IBM_INTEGER2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2:
               CON2_COMPLEX8IBM_UNSIGNED2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2X:
               CON2_COMPLEX8IBM_UNSIGNED2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4:
               CON2_COMPLEX8IBM_INTEGER4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4X:
               CON2_COMPLEX8IBM_INTEGER4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4:
               CON2_COMPLEX8IBM_UNSIGNED4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4X:
               CON2_COMPLEX8IBM_UNSIGNED4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8:
               CON2_COMPLEX8IBM_INTEGER8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8X:
               CON2_COMPLEX8IBM_INTEGER8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED8:
               CON2_COMPLEX8IBM_UNSIGNED8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED8X:
               CON2_COMPLEX8IBM_UNSIGNED8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4:
               CON2_COMPLEX8IBM_FLOAT4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4X:
               CON2_COMPLEX8IBM_FLOAT4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4IBM:
               CON2_COMPLEX8IBM_FLOAT4IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8:
               CON2_COMPLEX8IBM_FLOAT8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8X:
               CON2_COMPLEX8IBM_FLOAT8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8IBM:
               CON2_COMPLEX8IBM_FLOAT8IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16:
               CON2_COMPLEX8IBM_FLOAT16(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16X:
               CON2_COMPLEX8IBM_FLOAT16X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX8:
               CON2_COMPLEX8IBM_COMPLEX8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX8X:
               CON2_COMPLEX8IBM_COMPLEX8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX8IBM:
               memcpy(ptr1, ptr2, 8 * expra->count);
               break;
            case DDS_ETYPE_COMPLEX16:
               CON2_COMPLEX8IBM_COMPLEX16(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX16X:
               CON2_COMPLEX8IBM_COMPLEX16X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX16IBM:
               CON2_COMPLEX8IBM_COMPLEX16IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX32:
               CON2_COMPLEX8IBM_COMPLEX32(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX32X:
               CON2_COMPLEX8IBM_COMPLEX32X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            default:
               ddsPanic("ddsEvaluate: invalid assign, COMPLEX8IBM= (PTYPE)%i\n",
                  (int)expra->stype2);
            }
            break;
         case DDS_ETYPE_COMPLEX16:
            switch(etype2) {
            case DDS_ETYPE_INTEGER1:
               CON2_COMPLEX16_INTEGER1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED1:
               CON2_COMPLEX16_UNSIGNED1(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2:
               CON2_COMPLEX16_INTEGER2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2X:
               CON2_COMPLEX16_INTEGER2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2:
               CON2_COMPLEX16_UNSIGNED2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2X:
               CON2_COMPLEX16_UNSIGNED2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4:
               CON2_COMPLEX16_INTEGER4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4X:
               CON2_COMPLEX16_INTEGER4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4:
               CON2_COMPLEX16_UNSIGNED4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4X:
               CON2_COMPLEX16_UNSIGNED4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8:
               CON2_COMPLEX16_INTEGER8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8X:
               CON2_COMPLEX16_INTEGER8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED8:
               CON2_COMPLEX16_UNSIGNED8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED8X:
               CON2_COMPLEX16_UNSIGNED8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4:
               CON2_COMPLEX16_FLOAT4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4X:
               CON2_COMPLEX16_FLOAT4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4IBM:
               CON2_COMPLEX16_FLOAT4IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8:
               CON2_COMPLEX16_FLOAT8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8X:
               CON2_COMPLEX16_FLOAT8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8IBM:
               CON2_COMPLEX16_FLOAT8IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16:
               CON2_COMPLEX16_FLOAT16(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16X:
               CON2_COMPLEX16_FLOAT16X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX8:
               CON2_COMPLEX16_COMPLEX8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX8X:
               CON2_COMPLEX16_COMPLEX8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX8IBM:
               CON2_COMPLEX16_COMPLEX8IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX16:
               memcpy(ptr1, ptr2, 16 * expra->count);
               break;
            case DDS_ETYPE_COMPLEX16X:
               CON2_COMPLEX16_COMPLEX16X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX16IBM:
               CON2_COMPLEX16_COMPLEX16IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX32:
               CON2_COMPLEX16_COMPLEX32(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX32X:
               CON2_COMPLEX16_COMPLEX32X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            default:
               ddsPanic("ddsEvaluate: invalid assign, COMPLEX16= (PTYPE)%i\n",
                  (int)expra->stype2);
            }
            break;
         case DDS_ETYPE_COMPLEX16X:
            switch(etype2) {
            case DDS_ETYPE_INTEGER1:
               CON2_COMPLEX16X_INTEGER1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED1:
               CON2_COMPLEX16X_UNSIGNED1(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2:
               CON2_COMPLEX16X_INTEGER2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2X:
               CON2_COMPLEX16X_INTEGER2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2:
               CON2_COMPLEX16X_UNSIGNED2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2X:
               CON2_COMPLEX16X_UNSIGNED2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4:
               CON2_COMPLEX16X_INTEGER4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4X:
               CON2_COMPLEX16X_INTEGER4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4:
               CON2_COMPLEX16X_UNSIGNED4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4X:
               CON2_COMPLEX16X_UNSIGNED4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8:
               CON2_COMPLEX16X_INTEGER8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8X:
               CON2_COMPLEX16X_INTEGER8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED8:
               CON2_COMPLEX16X_UNSIGNED8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED8X:
               CON2_COMPLEX16X_UNSIGNED8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4:
               CON2_COMPLEX16X_FLOAT4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4X:
               CON2_COMPLEX16X_FLOAT4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4IBM:
               CON2_COMPLEX16X_FLOAT4IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8:
               CON2_COMPLEX16X_FLOAT8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8X:
               CON2_COMPLEX16X_FLOAT8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8IBM:
               CON2_COMPLEX16X_FLOAT8IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16:
               CON2_COMPLEX16X_FLOAT16(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16X:
               CON2_COMPLEX16X_FLOAT16X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX8:
               CON2_COMPLEX16X_COMPLEX8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX8X:
               CON2_COMPLEX16X_COMPLEX8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX8IBM:
               CON2_COMPLEX16X_COMPLEX8IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX16:
               CON2_COMPLEX16X_COMPLEX16(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX16X:
               memcpy(ptr1, ptr2, 16 * expra->count);
               break;
            case DDS_ETYPE_COMPLEX16IBM:
               CON2_COMPLEX16X_COMPLEX16IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX32:
               CON2_COMPLEX16X_COMPLEX32(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX32X:
               CON2_COMPLEX16X_COMPLEX32X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            default:
               ddsPanic("ddsEvaluate: invalid assign, COMPLEX16X= (PTYPE)%i\n",
                  (int)expra->stype2);
            }
            break;
         case DDS_ETYPE_COMPLEX16IBM:
            switch(etype2) {
            case DDS_ETYPE_INTEGER1:
               CON2_COMPLEX16IBM_INTEGER1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED1:
               CON2_COMPLEX16IBM_UNSIGNED1(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2:
               CON2_COMPLEX16IBM_INTEGER2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2X:
               CON2_COMPLEX16IBM_INTEGER2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2:
               CON2_COMPLEX16IBM_UNSIGNED2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2X:
               CON2_COMPLEX16IBM_UNSIGNED2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4:
               CON2_COMPLEX16IBM_INTEGER4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4X:
               CON2_COMPLEX16IBM_INTEGER4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4:
               CON2_COMPLEX16IBM_UNSIGNED4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4X:
               CON2_COMPLEX16IBM_UNSIGNED4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8:
               CON2_COMPLEX16IBM_INTEGER8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8X:
               CON2_COMPLEX16IBM_INTEGER8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED8:
               CON2_COMPLEX16IBM_UNSIGNED8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED8X:
               CON2_COMPLEX16IBM_UNSIGNED8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4:
               CON2_COMPLEX16IBM_FLOAT4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4X:
               CON2_COMPLEX16IBM_FLOAT4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4IBM:
               CON2_COMPLEX16IBM_FLOAT4IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8:
               CON2_COMPLEX16IBM_FLOAT8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8X:
               CON2_COMPLEX16IBM_FLOAT8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8IBM:
               CON2_COMPLEX16IBM_FLOAT8IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16:
               CON2_COMPLEX16IBM_FLOAT16(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16X:
               CON2_COMPLEX16IBM_FLOAT16X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX8:
               CON2_COMPLEX16IBM_COMPLEX8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX8X:
               CON2_COMPLEX16IBM_COMPLEX8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX8IBM:
               CON2_COMPLEX16IBM_COMPLEX8IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX16:
               CON2_COMPLEX16IBM_COMPLEX16(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX16X:
               CON2_COMPLEX16IBM_COMPLEX16X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX16IBM:
               memcpy(ptr1, ptr2, 16 * expra->count);
               break;
            case DDS_ETYPE_COMPLEX32:
               CON2_COMPLEX16IBM_COMPLEX32(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX32X:
               CON2_COMPLEX16IBM_COMPLEX32X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            default:
               ddsPanic("ddsEvaluate: invalid assign, COMPLEX16IBM= (PTYPE)%i\n",
                  (int)expra->stype2);
            }
            break;
         case DDS_ETYPE_COMPLEX32:
            switch(etype2) {
            case DDS_ETYPE_INTEGER1:
               CON2_COMPLEX32_INTEGER1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED1:
               CON2_COMPLEX32_UNSIGNED1(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2:
               CON2_COMPLEX32_INTEGER2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2X:
               CON2_COMPLEX32_INTEGER2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2:
               CON2_COMPLEX32_UNSIGNED2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2X:
               CON2_COMPLEX32_UNSIGNED2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4:
               CON2_COMPLEX32_INTEGER4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4X:
               CON2_COMPLEX32_INTEGER4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4:
               CON2_COMPLEX32_UNSIGNED4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4X:
               CON2_COMPLEX32_UNSIGNED4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8:
               CON2_COMPLEX32_INTEGER8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8X:
               CON2_COMPLEX32_INTEGER8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED8:
               CON2_COMPLEX32_UNSIGNED8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED8X:
               CON2_COMPLEX32_UNSIGNED8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4:
               CON2_COMPLEX32_FLOAT4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4X:
               CON2_COMPLEX32_FLOAT4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4IBM:
               CON2_COMPLEX32_FLOAT4IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8:
               CON2_COMPLEX32_FLOAT8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8X:
               CON2_COMPLEX32_FLOAT8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8IBM:
               CON2_COMPLEX32_FLOAT8IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16:
               CON2_COMPLEX32_FLOAT16(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16X:
               CON2_COMPLEX32_FLOAT16X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX8:
               CON2_COMPLEX32_COMPLEX8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX8X:
               CON2_COMPLEX32_COMPLEX8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX8IBM:
               CON2_COMPLEX32_COMPLEX8IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX16:
               CON2_COMPLEX32_COMPLEX16(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX16X:
               CON2_COMPLEX32_COMPLEX16X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX16IBM:
               CON2_COMPLEX32_COMPLEX16IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX32:
               memcpy(ptr1, ptr2, 32 * expra->count);
               break;
            case DDS_ETYPE_COMPLEX32X:
               CON2_COMPLEX32_COMPLEX32X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            default:
               ddsPanic("ddsEvaluate: invalid assign, COMPLEX32= (PTYPE)%i\n",
                  (int)expra->stype2);
            }
            break;
         case DDS_ETYPE_COMPLEX32X:
            switch(etype2) {
            case DDS_ETYPE_INTEGER1:
               CON2_COMPLEX32X_INTEGER1(VV, N, A, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED1:
               CON2_COMPLEX32X_UNSIGNED1(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2:
               CON2_COMPLEX32X_INTEGER2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER2X:
               CON2_COMPLEX32X_INTEGER2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2:
               CON2_COMPLEX32X_UNSIGNED2(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED2X:
               CON2_COMPLEX32X_UNSIGNED2X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4:
               CON2_COMPLEX32X_INTEGER4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER4X:
               CON2_COMPLEX32X_INTEGER4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4:
               CON2_COMPLEX32X_UNSIGNED4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED4X:
               CON2_COMPLEX32X_UNSIGNED4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8:
               CON2_COMPLEX32X_INTEGER8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_INTEGER8X:
               CON2_COMPLEX32X_INTEGER8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED8:
               CON2_COMPLEX32X_UNSIGNED8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_UNSIGNED8X:
               CON2_COMPLEX32X_UNSIGNED8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4:
               CON2_COMPLEX32X_FLOAT4(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4X:
               CON2_COMPLEX32X_FLOAT4X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT4IBM:
               CON2_COMPLEX32X_FLOAT4IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8:
               CON2_COMPLEX32X_FLOAT8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8X:
               CON2_COMPLEX32X_FLOAT8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT8IBM:
               CON2_COMPLEX32X_FLOAT8IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16:
               CON2_COMPLEX32X_FLOAT16(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_FLOAT16X:
               CON2_COMPLEX32X_FLOAT16X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX8:
               CON2_COMPLEX32X_COMPLEX8(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX8X:
               CON2_COMPLEX32X_COMPLEX8X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX8IBM:
               CON2_COMPLEX32X_COMPLEX8IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX16:
               CON2_COMPLEX32X_COMPLEX16(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX16X:
               CON2_COMPLEX32X_COMPLEX16X(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX16IBM:
               CON2_COMPLEX32X_COMPLEX16IBM(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX32:
               CON2_COMPLEX32X_COMPLEX32(VV, N, N, ptr1, ptr2, expra->count)
               break;
            case DDS_ETYPE_COMPLEX32X:
               memcpy(ptr1, ptr2, 32 * expra->count);
               break;
            default:
               ddsPanic("ddsEvaluate: invalid assign, COMPLEX32X= (PTYPE)%i\n",
                  (int)expra->stype2);
            }
            break;
         default:
            ddsPanic("ddsEvaluate: invalid assign, ptype1 = %i\n",
               (int)expra->stype1);
         }
         break;
      case DDS_COPY_OP:
         memcpy(expr->arg1.value.ptr, expr->arg2.value.ptr,
            expr->arg3.value.idx);
         break;
      case DDS_ARRAY_REF_OP_PTR_SUB:
         expr->ptr =
            (char*)expr->arg1.value.ptr +
            *(size_t*)expr->arg2.value.ptr * expr->arg4.value.idx;
         break;
      case DDS_ARRAY_REF_OP_SUB_PTR:
         expr->ptr =
            (char*)expr->arg2.value.ptr +
            *(size_t*)expr->arg1.value.ptr * expr->arg4.value.idx;
         break;

      #define UNARY_MINUS(TYPE) \
         case DDS_UNARY_MINUS_OP_##TYPE : \
            *(HOST_##TYPE##_T*)expr->ptr = \
               - *(HOST_##TYPE##_T*)expr->arg1.value.ptr; \
            break;

      #define UNARY_MINUS_CPX(TYPE) \
         case DDS_UNARY_MINUS_OP_##TYPE : \
            (*(HOST_##TYPE##_T*)expr->ptr).real = \
               - (*(HOST_##TYPE##_T*)expr->arg1.value.ptr).real; \
            (*(HOST_##TYPE##_T*)expr->ptr).imag = \
               - (*(HOST_##TYPE##_T*)expr->arg1.value.ptr).imag; \
            break;
      UNARY_MINUS(INT)
      UNARY_MINUS(LONG)
      UNARY_MINUS(LLONG)
      UNARY_MINUS(UINT)
      UNARY_MINUS(ULONG)
      UNARY_MINUS(ULLONG)
      UNARY_MINUS(FLOAT)
      UNARY_MINUS(DOUBLE)
      UNARY_MINUS(LDOUBLE)
      UNARY_MINUS_CPX(CPX)
      UNARY_MINUS_CPX(DBLCPX)
      UNARY_MINUS_CPX(LDBLCPX)

      #define COMPLEMENT(TYPE) \
         case DDS_COMPLEMENT_OP_##TYPE : \
            *(HOST_##TYPE##_T*)expr->ptr = \
               ~ *(HOST_##TYPE##_T*)expr->arg1.value.ptr; \
            break;
      COMPLEMENT(INT)
      COMPLEMENT(LONG)
      COMPLEMENT(LLONG)
      COMPLEMENT(UINT)
      COMPLEMENT(ULONG)
      COMPLEMENT(ULLONG)

      #define NEGATION(TYPE) \
         case DDS_NEGATION_OP_##TYPE : \
            *(HOST_INT_T*)expr->ptr = \
               ! *(HOST_##TYPE##_T*)expr->arg1.value.ptr; \
            break;

      #define NEGATION_CPX(TYPE) \
         case DDS_NEGATION_OP_##TYPE : \
            *(HOST_INT_T*)expr->ptr = \
               ! ((*(HOST_##TYPE##_T*)expr->arg1.value.ptr).real || \
                  (*(HOST_##TYPE##_T*)expr->arg1.value.ptr).imag) ; \
         break;
      NEGATION(CHAR)
      NEGATION(SHORT)
      NEGATION(INT)
      NEGATION(LONG)
      NEGATION(LLONG)
      NEGATION(FLOAT)
      NEGATION(DOUBLE)
      NEGATION(LDOUBLE)
      NEGATION_CPX(CPX)
      NEGATION_CPX(DBLCPX)
      NEGATION_CPX(LDBLCPX)

      case DDS_SQRT_OP:
         *(HOST_DOUBLE_T*)expr->ptr = sqrt(*(double*)expr->arg1.value.ptr);
         break;

      case DDS_SIN_OP:
         *(HOST_DOUBLE_T*)expr->ptr = sin(*(double*)expr->arg1.value.ptr);
         break;

      case DDS_COS_OP:
         *(HOST_DOUBLE_T*)expr->ptr = cos(*(double*)expr->arg1.value.ptr);
         break;

      case DDS_TAN_OP:
         *(HOST_DOUBLE_T*)expr->ptr = tan(*(double*)expr->arg1.value.ptr);
         break;

      case DDS_ASIN_OP:
         *(HOST_DOUBLE_T*)expr->ptr = asin(*(double*)expr->arg1.value.ptr);
         break;

      case DDS_ACOS_OP:
         *(HOST_DOUBLE_T*)expr->ptr = acos(*(double*)expr->arg1.value.ptr);
         break;

      case DDS_ATAN_OP:
         *(HOST_DOUBLE_T*)expr->ptr = atan(*(double*)expr->arg1.value.ptr);
         break;

      case DDS_ATAN2_OP:
         *(HOST_DOUBLE_T*)expr->ptr = atan2(*(double*)expr->arg1.value.ptr,
                                            *(double*)expr->arg2.value.ptr);
         break;

      case DDS_EXP_OP:
         *(HOST_DOUBLE_T*)expr->ptr = exp(*(double*)expr->arg1.value.ptr);
         break;

      case DDS_LOG_OP:
         *(HOST_DOUBLE_T*)expr->ptr = log(*(double*)expr->arg1.value.ptr);
         break;

      case DDS_LOG10_OP:
         *(HOST_DOUBLE_T*)expr->ptr = log10(*(double*)expr->arg1.value.ptr);
         break;

      case DDS_POW_OP:
         *(HOST_DOUBLE_T*)expr->ptr = pow(*(double*)expr->arg1.value.ptr,
                                          *(double*)expr->arg2.value.ptr);
         break;

      case DDS_ABS_OP:
         *(HOST_DOUBLE_T*)expr->ptr = fabs(*(double*)expr->arg1.value.ptr);
         break;

      case DDS_SIGN_OP:
         *(HOST_DOUBLE_T*)expr->ptr = *(double*)expr->arg1.value.ptr > 0 ? 1
            : ( *(double*)expr->arg1.value.ptr < 0 ? -1 : 0 );
         break;

      case DDS_CEIL_OP:
         *(HOST_DOUBLE_T*)expr->ptr = ceil(*(double*)expr->arg1.value.ptr);
         break;

      case DDS_FLOOR_OP:
         *(HOST_DOUBLE_T*)expr->ptr = floor(*(double*)expr->arg1.value.ptr);
         break;

      case DDS_DATEINT_OP:
         ddsDate2Int(expr->ptr, expr->arg1.value.ptr);
         break;

      case DDS_DATESTRUCT_OP:
         {
            double tmp;
            CON_ANSS(DDS_CTYPE_DOUBLE, expra->stype1, &tmp, ptr1);
            ddsDate2Struct(expr->ptr, &tmp);
         }
         break;

      #define MULTIPLY(TYPE) \
         case DDS_MULTIPLY_OP_##TYPE : \
            *(HOST_##TYPE##_T*)expr->ptr = \
               *(HOST_##TYPE##_T*)expr->arg1.value.ptr * \
               *(HOST_##TYPE##_T*)expr->arg2.value.ptr; \
            break;

      #define MULTIPLY_CPX(TYPE) \
         case DDS_MULTIPLY_OP_##TYPE : \
            (*(HOST_##TYPE##_T*)expr->ptr).real = \
               (*(HOST_##TYPE##_T*)expr->arg1.value.ptr).real * \
               (*(HOST_##TYPE##_T*)expr->arg2.value.ptr).real - \
               (*(HOST_##TYPE##_T*)expr->arg1.value.ptr).imag * \
               (*(HOST_##TYPE##_T*)expr->arg2.value.ptr).imag; \
            (*(HOST_##TYPE##_T*)expr->ptr).imag = \
               (*(HOST_##TYPE##_T*)expr->arg1.value.ptr).real * \
               (*(HOST_##TYPE##_T*)expr->arg2.value.ptr).imag + \
               (*(HOST_##TYPE##_T*)expr->arg1.value.ptr).imag * \
               (*(HOST_##TYPE##_T*)expr->arg2.value.ptr).real; \
            break;
      MULTIPLY(INT)
      MULTIPLY(LONG)
      MULTIPLY(LLONG)
      MULTIPLY(UINT)
      MULTIPLY(ULONG)
      MULTIPLY(ULLONG)
      MULTIPLY(FLOAT)
      MULTIPLY(DOUBLE)
      MULTIPLY(LDOUBLE)
      MULTIPLY_CPX(CPX)
      MULTIPLY_CPX(DBLCPX)
      MULTIPLY_CPX(LDBLCPX)

      #define DIVIDE(TYPE) \
         case DDS_DIVIDE_OP_##TYPE : \
            if(*(HOST_##TYPE##_T*)expr->arg2.value.ptr) { \
               *(HOST_##TYPE##_T*)expr->ptr = \
                  *(HOST_##TYPE##_T*)expr->arg1.value.ptr / \
                  *(HOST_##TYPE##_T*)expr->arg2.value.ptr; \
            } else { \
               ddsError("ddsEvaluate: Divide by Zero\n"); \
                  *(HOST_##TYPE##_T*)expr->ptr = 0; \
            } \
            break;

      #define DIVIDE_CPX(TYPE, VTYPE) \
         case DDS_DIVIDE_OP_##TYPE : \
            VTYPE = \
               (*(HOST_##TYPE##_T*)expr->arg2.value.ptr).real * \
               (*(HOST_##TYPE##_T*)expr->arg2.value.ptr).real + \
               (*(HOST_##TYPE##_T*)expr->arg2.value.ptr).imag * \
               (*(HOST_##TYPE##_T*)expr->arg2.value.ptr).imag; \
            if(VTYPE) { \
               (*(HOST_##TYPE##_T*)expr->ptr).real = \
                  (  (*(HOST_##TYPE##_T*)expr->arg1.value.ptr).real * \
                     (*(HOST_##TYPE##_T*)expr->arg2.value.ptr).real + \
                     (*(HOST_##TYPE##_T*)expr->arg1.value.ptr).imag * \
                     (*(HOST_##TYPE##_T*)expr->arg2.value.ptr).imag) / VTYPE; \
               (*(HOST_##TYPE##_T*)expr->ptr).imag = \
                  (  (*(HOST_##TYPE##_T*)expr->arg1.value.ptr).imag * \
                     (*(HOST_##TYPE##_T*)expr->arg2.value.ptr).real - \
                     (*(HOST_##TYPE##_T*)expr->arg1.value.ptr).real * \
                     (*(HOST_##TYPE##_T*)expr->arg2.value.ptr).imag) / VTYPE; \
            } else { \
               ddsError("ddsEvaluate: Complex Divide by Zero\n"); \
               (*(HOST_##TYPE##_T*)expr->ptr).real = 0.; \
               (*(HOST_##TYPE##_T*)expr->ptr).imag = 0.; \
            } \
            break;
      DIVIDE(INT)
      DIVIDE(LONG)
      DIVIDE(LLONG)
      DIVIDE(UINT)
      DIVIDE(ULONG)
      DIVIDE(ULLONG)
      DIVIDE(FLOAT)
      DIVIDE(DOUBLE)
      DIVIDE(LDOUBLE)
      DIVIDE_CPX(CPX, FLOAT_TS)
      DIVIDE_CPX(DBLCPX, DOUBLE_TS)
      DIVIDE_CPX(LDBLCPX, LDOUBLE_TS)

      #define MODULO(TYPE) \
         case DDS_MODULO_OP_##TYPE : \
            if(*(HOST_##TYPE##_T*)expr->arg2.value.ptr) { \
               *(HOST_##TYPE##_T*)expr->ptr = \
                  *(HOST_##TYPE##_T*)expr->arg1.value.ptr % \
                  *(HOST_##TYPE##_T*)expr->arg2.value.ptr; \
            } else { \
               ddsError("ddsEvaluate: Modulo by Zero\n"); \
                  *(HOST_##TYPE##_T*)expr->ptr = 0; \
            } \
            break;
      MODULO(INT)
      MODULO(LONG)
      MODULO(LLONG)
      MODULO(UINT)
      MODULO(ULONG)
      MODULO(ULLONG)

      #define PLUS(TYPE) \
         case DDS_PLUS_OP_##TYPE : \
            *(HOST_##TYPE##_T*)expr->ptr = \
               *(HOST_##TYPE##_T*)expr->arg1.value.ptr + \
               *(HOST_##TYPE##_T*)expr->arg2.value.ptr; \
            break;

      #define PLUS_CPX(TYPE) \
         case DDS_PLUS_OP_##TYPE : \
            (*(HOST_##TYPE##_T*)expr->ptr).real = \
               (*(HOST_##TYPE##_T*)expr->arg1.value.ptr).real + \
               (*(HOST_##TYPE##_T*)expr->arg2.value.ptr).real; \
            (*(HOST_##TYPE##_T*)expr->ptr).imag = \
               (*(HOST_##TYPE##_T*)expr->arg1.value.ptr).imag + \
               (*(HOST_##TYPE##_T*)expr->arg2.value.ptr).imag; \
            break;
      PLUS(INT)
      PLUS(LONG)
      PLUS(LLONG)
      PLUS(UINT)
      PLUS(ULONG)
      PLUS(ULLONG)
      PLUS(FLOAT)
      PLUS(DOUBLE)
      PLUS(LDOUBLE)
      PLUS_CPX(CPX)
      PLUS_CPX(DBLCPX)
      PLUS_CPX(LDBLCPX)

      #define MINUS(TYPE) \
         case DDS_MINUS_OP_##TYPE : \
            *(HOST_##TYPE##_T*)expr->ptr = \
               *(HOST_##TYPE##_T*)expr->arg1.value.ptr - \
               *(HOST_##TYPE##_T*)expr->arg2.value.ptr; \
            break;

      #define MINUS_CPX(TYPE) \
         case DDS_MINUS_OP_##TYPE : \
            (*(HOST_##TYPE##_T*)expr->ptr).real = \
               (*(HOST_##TYPE##_T*)expr->arg1.value.ptr).real - \
               (*(HOST_##TYPE##_T*)expr->arg2.value.ptr).real; \
            (*(HOST_##TYPE##_T*)expr->ptr).imag = \
               (*(HOST_##TYPE##_T*)expr->arg1.value.ptr).imag - \
               (*(HOST_##TYPE##_T*)expr->arg2.value.ptr).imag; \
            break;
      MINUS(INT)
      MINUS(LONG)
      MINUS(LLONG)
      MINUS(UINT)
      MINUS(ULONG)
      MINUS(ULLONG)
      MINUS(FLOAT)
      MINUS(DOUBLE)
      MINUS(LDOUBLE)
      MINUS_CPX(CPX)
      MINUS_CPX(DBLCPX)
      MINUS_CPX(LDBLCPX)

      #define LESS_THAN(TYPE) \
         case DDS_LESS_THAN_OP_##TYPE : \
            *(HOST_INT_T*)expr->ptr = \
               *(HOST_##TYPE##_T*)expr->arg1.value.ptr < \
               *(HOST_##TYPE##_T*)expr->arg2.value.ptr; \
            break;
      LESS_THAN(INT)
      LESS_THAN(LONG)
      LESS_THAN(LLONG)
      LESS_THAN(UINT)
      LESS_THAN(ULONG)
      LESS_THAN(ULLONG)
      LESS_THAN(FLOAT)
      LESS_THAN(DOUBLE)
      LESS_THAN(LDOUBLE)

      #define GREATER_THAN(TYPE) \
         case DDS_GREATER_THAN_OP_##TYPE : \
            *(HOST_INT_T*)expr->ptr = \
               *(HOST_##TYPE##_T*)expr->arg1.value.ptr > \
               *(HOST_##TYPE##_T*)expr->arg2.value.ptr; \
            break;
      GREATER_THAN(INT)
      GREATER_THAN(LONG)
      GREATER_THAN(LLONG)
      GREATER_THAN(UINT)
      GREATER_THAN(ULONG)
      GREATER_THAN(ULLONG)
      GREATER_THAN(FLOAT)
      GREATER_THAN(DOUBLE)
      GREATER_THAN(LDOUBLE)

      #define LESS_EQUAL(TYPE) \
         case DDS_LESS_EQUAL_OP_##TYPE : \
            *(HOST_INT_T*)expr->ptr = \
               *(HOST_##TYPE##_T*)expr->arg1.value.ptr <= \
               *(HOST_##TYPE##_T*)expr->arg2.value.ptr; \
            break;
      LESS_EQUAL(INT)
      LESS_EQUAL(LONG)
      LESS_EQUAL(LLONG)
      LESS_EQUAL(UINT)
      LESS_EQUAL(ULONG)
      LESS_EQUAL(ULLONG)
      LESS_EQUAL(FLOAT)
      LESS_EQUAL(DOUBLE)
      LESS_EQUAL(LDOUBLE)

      #define GREATER_EQUAL(TYPE) \
         case DDS_GREATER_EQUAL_OP_##TYPE : \
            *(HOST_INT_T*)expr->ptr = \
               *(HOST_##TYPE##_T*)expr->arg1.value.ptr >= \
               *(HOST_##TYPE##_T*)expr->arg2.value.ptr; \
            break;
      GREATER_EQUAL(INT)
      GREATER_EQUAL(LONG)
      GREATER_EQUAL(LLONG)
      GREATER_EQUAL(UINT)
      GREATER_EQUAL(ULONG)
      GREATER_EQUAL(ULLONG)
      GREATER_EQUAL(FLOAT)
      GREATER_EQUAL(DOUBLE)
      GREATER_EQUAL(LDOUBLE)

      #define EQUAL_TO(TYPE) \
         case DDS_EQUAL_TO_OP_##TYPE : \
            *(HOST_INT_T*)expr->ptr = \
               *(HOST_##TYPE##_T*)expr->arg1.value.ptr == \
               *(HOST_##TYPE##_T*)expr->arg2.value.ptr; \
            break;
      EQUAL_TO(INT)
      EQUAL_TO(LONG)
      EQUAL_TO(LLONG)
      EQUAL_TO(UINT)
      EQUAL_TO(ULONG)
      EQUAL_TO(ULLONG)
      EQUAL_TO(FLOAT)
      EQUAL_TO(DOUBLE)
      EQUAL_TO(LDOUBLE)

      #define NOT_EQUAL_TO(TYPE) \
         case DDS_NOT_EQUAL_TO_OP_##TYPE : \
            *(HOST_INT_T*)expr->ptr = \
               *(HOST_##TYPE##_T*)expr->arg1.value.ptr != \
               *(HOST_##TYPE##_T*)expr->arg2.value.ptr; \
            break;
      NOT_EQUAL_TO(INT)
      NOT_EQUAL_TO(LONG)
      NOT_EQUAL_TO(LLONG)
      NOT_EQUAL_TO(UINT)
      NOT_EQUAL_TO(ULONG)
      NOT_EQUAL_TO(ULLONG)
      NOT_EQUAL_TO(FLOAT)
      NOT_EQUAL_TO(DOUBLE)
      NOT_EQUAL_TO(LDOUBLE)

      #define BIT_AND(TYPE) \
         case DDS_BIT_AND_OP_##TYPE : \
            *(HOST_##TYPE##_T*)expr->ptr = \
               *(HOST_##TYPE##_T*)expr->arg1.value.ptr & \
               *(HOST_##TYPE##_T*)expr->arg2.value.ptr; \
            break;
      BIT_AND(INT)
      BIT_AND(LONG)
      BIT_AND(LLONG)
      BIT_AND(UINT)
      BIT_AND(ULONG)
      BIT_AND(ULLONG)

      #define BIT_XOR(TYPE) \
         case DDS_BIT_XOR_OP_##TYPE : \
            *(HOST_##TYPE##_T*)expr->ptr = \
               *(HOST_##TYPE##_T*)expr->arg1.value.ptr ^ \
               *(HOST_##TYPE##_T*)expr->arg2.value.ptr; \
            break;
      BIT_XOR(INT)
      BIT_XOR(LONG)
      BIT_XOR(LLONG)
      BIT_XOR(UINT)
      BIT_XOR(ULONG)
      BIT_XOR(ULLONG)

      #define BIT_OR(TYPE) \
         case DDS_BIT_OR_OP_##TYPE : \
            *(HOST_##TYPE##_T*)expr->ptr = \
               *(HOST_##TYPE##_T*)expr->arg1.value.ptr | \
               *(HOST_##TYPE##_T*)expr->arg2.value.ptr; \
            break;
      BIT_OR(INT)
      BIT_OR(LONG)
      BIT_OR(LLONG)
      BIT_OR(UINT)
      BIT_OR(ULONG)
      BIT_OR(ULLONG)

   case DDS_CONDITIONAL_OP:
      /* Swap expr2 & expr3 because we have negated expr1 */
      if(*(int*)expr->arg1.value.ptr) {
         /* select false branch */
         branch = ((DDS_CONDITION*)expr->arg4.value.ptr)->expr2;
         if(branch) {
            /* unknown kludge */
            save = branch->next;
            branch->next = NULL;
            ddsEvaluate(branch);
            branch->next = save;
         }
         expr->ptr = expr->arg2.value.ptr;
      } else {
         /* select true branch */
         branch = ((DDS_CONDITION*)expr->arg4.value.ptr)->expr3;
         if(branch) {
            /* unknown kludge */
            save = branch->next;
            branch->next = NULL;
            ddsEvaluate(branch);
            branch->next = save;
         }
         expr->ptr = expr->arg3.value.ptr;
      }
      break;

   default:
      ddsPanic("ddsEvaluate: invalid opcode = %i\n", (int)expr->opcode);
   }

   if((expr->flag & DDS_SYM_RELOC) && expr->reloc) {
      /* relocate dependencies on the computed value (ptr) */
      RELOC_VALUE(expr);
   }
   expr = expr->next;
   if(expr) goto evaluate;

   return;
}
