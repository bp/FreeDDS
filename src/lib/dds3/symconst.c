static char rcsid[]="$Id: symconst.c 186 2010-09-27 19:29:14Z user $";
/*
RCS-t=DDS, evaluate constant expression and return its value

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

#ifdef HOST_Linux

#define SYM_CONST(FNAME, CTYPE, RTYPE, OVERFLOW_TEST)  \
HOST_##CTYPE##_T ddsSymConst##FNAME(  \
   DDS_VALUE *value,  \
   HOST_##CTYPE##_T dvalue,  \
   DDS_SYM_FLAG mask)  \
{  \
   DDS_CON_STATUS status;  \
   HOST_##RTYPE##_T result;  \
   if((value->flag & DDS_SYM_ERROR) || ! (value->flag & mask)) {  \
      ddsYYError("constant expression required\n");  \
      return dvalue;  \
   } else if(dds_thread) {  \
      /* evaluate expressions threaded by the needle */  \
      ddsEvaluate(dds_thread);  \
      dds_thread = 0;  \
      dds_needle = 0;  \
   }  \
   result = dvalue;  \
   status = CON_AASS(HOST_##RTYPE##_DDS, value->type->ptype,  \
      &result, value->ptr);  \
   return (HOST_##CTYPE##_T)result;  \
}

#else /* HOST_Linux */

#define SYM_CONST(FNAME, CTYPE, RTYPE, OVERFLOW_TEST)  \
HOST_##CTYPE##_T ddsSymConst##FNAME(  \
   DDS_VALUE *value,  \
   HOST_##CTYPE##_T dvalue,  \
   DDS_SYM_FLAG mask)  \
{  \
   DDS_CON_STATUS status;  \
   HOST_##RTYPE##_T result;  \
   if((value->flag & DDS_SYM_ERROR) || ! (value->flag & mask)) {  \
      ddsYYError("constant expression required\n");  \
      return dvalue;  \
   } else if(dds_thread) {  \
      /* evaluate expressions threaded by the needle */  \
      ddsEvaluate(dds_thread);  \
      dds_thread = 0;  \
      dds_needle = 0;  \
   }  \
   result = dvalue;  \
   status = CON_AASS(HOST_##RTYPE##_DDS, value->type->ptype,  \
      &result, value->ptr);  \
  \
   if(status != DDS_CON_OK) {  \
      ddsYYWarn("constant conversion error to type '%s'\n",  \
         dds_prime[HOST_##CTYPE##_DDS].name);  \
   } else if(OVERFLOW_TEST) { \
      ddsYYWarn("integral constant overflow type '%s'\n",  \
         dds_prime[HOST_##CTYPE##_DDS].name);  \
   }  \
   return (HOST_##CTYPE##_T)result;  \
}

#endif /* HOST_Linux */

/*******************************************************/
/*  SYM_CONST(FNAME, CTYPE, RTYPE, OVERFLOW_TEST)      */
/*     FNAME: function name, i.e. Int, Long, etc.      */
/*               Suffix for "ddsSymConst".             */
/*     CTYPE: function type, i.e. INT, LONG, etc.      */
/*               and type of"dvalue" (default value).  */
/*     RTYPE: type of intermediate "result", choosen   */
/*               to avoid overflow (LLONG or ULLONG).  */
/*     OVERFLOW_TEST: based upon "result" value.       */
/*                                                     */
/*  Expand SYM_CONST to create the following functions */ 
/*     ddsSymConstInt                                  */
/*     ddsSymConstLong                                 */
/*     ddsSymConstLLong                                */
/*     ddsSymConstSizet                                */
/*     ddsSymConstUInt                                 */
/*     ddsSymConstULong                                */
/*     ddsSymConstULLong                               */
/*******************************************************/

SYM_CONST(Int,    INT,    LLONG, (result < INT_MIN || result > INT_MAX))
SYM_CONST(Long,   LONG,   LLONG, (result < LONG_MIN || result > LONG_MAX))
SYM_CONST(LLong,  LLONG,  LLONG, (result < LONG_MIN || result > LONG_MAX))

SYM_CONST(Sizet, SIZE_T,  ULLONG, (result > ULONG_MAX))
SYM_CONST(UInt,   UINT,   ULLONG, (result > UINT_MAX))
SYM_CONST(ULong,  ULONG,  ULLONG, (result > ULONG_MAX))
SYM_CONST(ULLong, ULLONG, ULLONG, (result > ULONG_MAX))
