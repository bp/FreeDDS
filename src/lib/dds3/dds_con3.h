#ifndef __DDS_CON3_H
#define __DDS_CON3_H

/*
RCS-t=DDS, macros convert data between binary representations
 
$Id: dds_con3.h 39 2009-01-13 23:00:15Z user $
*/

/***************************************************************/
/*  These macros convert data between binary representations.  */
/*  Summary of functionality and associated macro names.       */
/*     Error, conversion deferred                              */
/*        CON3_ERROR                                           */
/*     Simple byte copy                                        */
/*        CON3_COPY                                            */
/*     Character translation                                   */
/*        CON3_TRAN                                            */
/*     Conversion via ONE intermediate type                    */
/*        CON3_SS_MONO                                         */
/*        CON3_VV_MONO                                         */
/*        CON3_VS_MONO                                         */
/*     Conversion via TWO intermediate types                   */
/*        CON3_SS_DUAL                                         */
/*        CON3_VV_DUAL                                         */
/*        CON3_VS_DUAL                                         */
/*     Alignment assumptions for left and right PTR            */
/*        CON3_AA_SS                                           */
/*        CON3_AA_VV                                           */
/*        CON3_AA_VS                                           */
/*        CON3_AN_SS                                           */
/*        CON3_AN_VV                                           */
/*        CON3_AN_VS                                           */
/*        CON3_NA_SS                                           */
/*        CON3_NA_VV                                           */
/*        CON3_NA_VS                                           */
/*        CON3_NN_SS                                           */
/*        CON3_NN_VV                                           */
/*        CON3_NN_VS                                           */
/*                                                             */
/*  Syntax of infix notation used by macro names               */
/*     Left and right PTR alignment.                           */
/*        AA   => aligned                                      */
/*        NA   => not-aligned and aligned                      */
/*        AN   => aligned and not-aligned                      */
/*        NN   => not-aligned                                  */
/*     Left and right operand SHAPE                            */
/*        SS => to scalar from scalar                          */
/*        VV => to vector from vector                          */
/*        VS => to vector from scalar                          */
/***************************************************************/

#define ASCII1_TO_ASCII1   to_ascii_tilde
#define ASCII1_TO_EBCDIC1  ascii_to_ebcdic_tilde
#define EBCDIC1_TO_EBCDIC1 to_ebcdic_tilde
#define EBCDIC1_TO_ASCII1  ebcdic_to_ascii_tilde

/************************************/
/* Macro if conversion is deferred. */
/************************************/

/* Example: CON3_ERROR(INTEGER4, FLOAT8IBM) */
#define CON3_ERROR(LT3, RT3)  \
   ddsError("conversion deferred to '%s' from '%s'\n", #LT3, #RT3);

/**********************************************/
/* Macro if conversion is a simple byte copy. */
/**********************************************/

/* Example: CON3_COPY(out, in, count) */
#define CON3_COPY(LP3, RP3, MAX3)  \
   memcpy(LP3, RP3, (size_t)(MAX3));

/***************************************************/
/* Macro if conversion is a character translation. */
/***************************************************/

/* Example: CON3_TRAN(ASCII1, EBCDIC1, out, in, count) */
#define CON3_TRAN(LT3, RT3, LP3, RP3, MAX3)  \
   {size_t n;  \
      char *out = (char*)LP3;  \
      const char *in = (const char*)RP3;  \
      for(n = 0; n < MAX3; n++) *(out++) = RT3##_TO_##LT3(*(in++));  \
   }

/*****************************************************/
/* Macros to convert data via ONE Intermediate Type. */
/*****************************************************/

/* Example: CON3_SS_MONO(A, A, INTEGER2, INTEGER4, out, in, count, INTEGER8) */
#define CON3_SS_MONO(LA3, RA3, LT3, RT3, LP3, RP3, MAX3, IT3)  \
   {size_t reset = dds_tmp.top;  \
      void *p3m;  \
      p3m = ddsStackTmp(1, DDS_##IT3##_P);  \
      CON2_##IT3##_##RT3(SS, A, RA3, p3m, RP3, 1)  \
      CON2_##LT3##_##IT3(SS, LA3, A, LP3, p3m, 1)  \
      ddsStackSet(&dds_tmp, reset);  \
   }

/* Example: CON3_VV_MONO(A, A, INTEGER2, INTEGER4, out, in, count, INTEGER8) */
#define CON3_VV_MONO(LA3, RA3, LT3, RT3, LP3, RP3, MAX3, IT3)  \
   {size_t reset = dds_tmp.top;  \
      void *p3m;  \
      p3m = ddsStackTmp(MAX3, DDS_##IT3##_P);  \
      CON2_##IT3##_##RT3(VV, A, RA3, p3m, RP3, MAX3)  \
      CON2_##LT3##_##IT3(VV, LA3, A, LP3, p3m, MAX3)  \
      ddsStackSet(&dds_tmp, reset);  \
   }

/* Example: CON3_VS_MONO(A, A, INTEGER2, INTEGER4, out, in, count, INTEGER8) */
#define CON3_VS_MONO(LA3, RA3, LT3, RT3, LP3, RP3, MAX3, IT3)  \
   {size_t reset = dds_tmp.top;  \
      void *p3m;  \
      p3m = ddsStackTmp(1, DDS_##IT3##_P);  \
      CON2_##IT3##_##RT3(SS, A, RA3, p3m, RP3, 1)  \
      CON2_##LT3##_##IT3(VS, LA3, A, LP3, p3m, MAX3)  \
      ddsStackSet(&dds_tmp, reset);  \
   }

/******************************************************/
/* Macros to convert data via TWO Intermediate Types. */
/******************************************************/

/* Example: CON3_SS_DUAL(A, A, FLOAT4, INTEGER4, out, in, count, FLOAT8IBM, INTEGER8) */
#define CON3_SS_DUAL(LA3, RA3, LT3, RT3, LP3, RP3, MAX3, LIT3, RIT3)  \
   {size_t reset = dds_tmp.top;  \
      void *lp3d, *rp3d;  \
      lp3d = ddsStackTmp(1, DDS_##LIT3##_P);  \
      rp3d = ddsStackTmp(1, DDS_##RIT3##_P);  \
      CON2_##RIT3##_##RT3(SS, A, RA3, rp3d, RP3, 1)  \
      CON2_##LIT3##_##RIT3(SS, A, A, lp3d, rp3d, 1)  \
      CON2_##LT3##_##LIT3(SS, LA3, A, LP3, lp3d, 1)  \
      ddsStackSet(&dds_tmp, reset);  \
   }

/* Example: CON3_VV_DUAL(A, A, FLOAT4, INTEGER4, out, in, count, FLOAT8IBM, INTEGER8) */
#define CON3_VV_DUAL(LA3, RA3, LT3, RT3, LP3, RP3, MAX3, LIT3, RIT3)  \
   {size_t reset = dds_tmp.top;  \
      void *lp3d, *rp3d;  \
      lp3d = ddsStackTmp(MAX3, DDS_##LIT3##_P);  \
      rp3d = ddsStackTmp(MAX3, DDS_##RIT3##_P);  \
      CON2_##RIT3##_##RT3(VV, A, RA3, rp3d, RP3, MAX3)  \
      CON2_##LIT3##_##RIT3(VV, A, A, lp3d, rp3d, MAX3)  \
      CON2_##LT3##_##LIT3(VV, LA3, A, LP3, lp3d, MAX3)  \
      ddsStackSet(&dds_tmp, reset);  \
   }

/* Example: CON3_VS_DUAL(A, A, FLOAT4, INTEGER4, out, in, count, FLOAT8IBM, INTEGER8) */
#define CON3_VS_DUAL(LA3, RA3, LT3, RT3, LP3, RP3, MAX3, LIT3, RIT3)  \
   {size_t reset = dds_tmp.top;  \
      void *lp3d, *rp3d;  \
      lp3d = ddsStackTmp(1, DDS_##LIT3##_P);  \
      rp3d = ddsStackTmp(1, DDS_##RIT3##_P);  \
      CON2_##RIT3##_##RT3(SS, A, RA3, rp3d, RP3, 1)  \
      CON2_##LIT3##_##RIT3(SS, A, A, lp3d, rp3d, 1)  \
      CON2_##LT3##_##LIT3(VS, LA3, A, LP3, lp3d, MAX3)  \
      ddsStackSet(&dds_tmp, reset);  \
   }

/************************************************************/
/* This macro yields address misalignment for a given type. */
/************************************************************/

/* Example: MISALIGN(INTEGER4, buf_ptr) */
#if defined(_WIN32) || defined(MINGW)
#define MISALIGN(TYPE, PTR) ((size_t)(char*)(PTR) & (DDS_##TYPE##_A - 1))
#else
#define MISALIGN(TYPE, PTR) ((long)(char*)(PTR) & (DDS_##TYPE##_A - 1))
#endif

/*********************************************************/
/* Macros to force buffer alignment by copying data      */
/* to temporary input and output buffers, if needed.     */
/* Preprocessing required for ASSN, ASSR, ASSX and FUNC. */
/*********************************************************/

#define CON3_AA_SS(F3, LT3, RT3, LP3, RP3, MAX3)  \
   CON4_SS_##F3(LT3, RT3, LP3, RP3, 1)

#define CON3_AN_SS(F3, LT3, RT3, LP3, RP3, MAX3)  \
   {size_t reset = dds_tmp.top;  \
      void *rp3a; \
      if(MISALIGN(RT3, RP3)) {  \
         rp3a = ddsStackTmp(1, DDS_##RT3##_P);  \
         memcpy(rp3a, RP3, DDS_##RT3##_P);  \
         CON4_SS_##F3(LT3, RT3, LP3, rp3a, 1)  \
         ddsStackSet(&dds_tmp, reset);  \
      } else {  \
         CON4_SS_##F3(LT3, RT3, LP3, RP3, 1)  \
      }  \
   }

#define CON3_NA_SS(F3, LT3, RT3, LP3, RP3, MAX3)  \
   {size_t reset = dds_tmp.top;  \
      void *lp3a;  \
      if(MISALIGN(LT3, LP3)) {  \
         lp3a = ddsStackTmp(1, DDS_##LT3##_P);  \
         CON4_SS_##F3(LT3, RT3, lp3a, RP3, 1)  \
         memcpy(LP3, lp3a, DDS_##LT3##_P);  \
         ddsStackSet(&dds_tmp, reset);  \
      } else {  \
         CON4_SS_##F3(LT3, RT3, LP3, RP3, 1)  \
      }  \
   }

#define CON3_NN_SS(F3, LT3, RT3, LP3, RP3, MAX3)  \
   {size_t reset = dds_tmp.top;  \
      void *lp3a, *rp3a;  \
      if(MISALIGN(RT3, RP3)) {  \
         rp3a = ddsStackTmp(1, DDS_##RT3##_P);  \
         memcpy(rp3a, RP3, DDS_##RT3##_P);  \
         if(MISALIGN(LT3, LP3)) {  \
            lp3a = ddsStackTmp(1, DDS_##LT3##_P);  \
            CON4_SS_##F3(LT3, RT3, lp3a, rp3a, 1)  \
            memcpy(LP3, lp3a, DDS_##LT3##_P);  \
         } else {  \
            CON4_SS_##F3(LT3, RT3, LP3, rp3a, 1)  \
         }  \
         ddsStackSet(&dds_tmp, reset);  \
      } else {  \
         if(MISALIGN(LT3, LP3)) {  \
            lp3a = ddsStackTmp(1, DDS_##LT3##_P);  \
            CON4_SS_##F3(LT3, RT3, lp3a, RP3, 1)  \
            memcpy(LP3, lp3a, DDS_##LT3##_P);  \
            ddsStackSet(&dds_tmp, reset);  \
         } else {  \
            CON4_SS_##F3(LT3, RT3, LP3, RP3, 1)  \
         }  \
      }  \
   }


#define CON3_AA_VV(F3, LT3, RT3, LP3, RP3, MAX3)  \
   CON4_VV_##F3(LT3, RT3, LP3, RP3, MAX3);

#define CON3_AN_VV(F3, LT3, RT3, LP3, RP3, MAX3)  \
   {size_t reset = dds_tmp.top; \
      void *rp3a;  \
      if(MISALIGN(RT3, RP3)) {  \
         rp3a = ddsStackTmp(MAX3, DDS_##RT3##_P);  \
         memcpy(rp3a, RP3, MAX3 * DDS_##RT3##_P);  \
         CON4_VV_##F3(LT3, RT3, LP3, rp3a, MAX3)  \
        ddsStackSet(&dds_tmp, reset);  \
      } else {  \
         CON4_VV_##F3(LT3, RT3, LP3, RP3, MAX3)  \
      }  \
   }

#define CON3_NA_VV(F3, LT3, RT3, LP3, RP3, MAX3)  \
   {size_t reset = dds_tmp.top;  \
      void *lp3a;  \
      if(MISALIGN(LT3, LP3)) {  \
         lp3a = ddsStackTmp(MAX3, DDS_##LT3##_P);  \
         CON4_VV_##F3(LT3, RT3, lp3a, RP3, MAX3)  \
         memcpy(LP3, lp3a, MAX3 * DDS_##LT3##_P);  \
         ddsStackSet(&dds_tmp, reset);  \
      } else {  \
         CON4_VV_##F3(LT3, RT3, LP3, RP3, MAX3)  \
      }  \
   }

#define CON3_NN_VV(F3, LT3, RT3, LP3, RP3, MAX3)  \
   {size_t reset = dds_tmp.top;  \
      void *lp3a, *rp3a;  \
      if(MISALIGN(RT3, RP3)) {  \
         rp3a = ddsStackTmp(MAX3, DDS_##RT3##_P);  \
         memcpy(rp3a, RP3, MAX3 * DDS_##RT3##_P);  \
         if(MISALIGN(LT3, LP3)) {  \
            lp3a = ddsStackTmp(MAX3, DDS_##LT3##_P);  \
            CON4_VV_##F3(LT3, RT3, lp3a, rp3a, MAX3)  \
         } else {  \
            CON4_VV_##F3(LT3, RT3, LP3, rp3a, MAX3)  \
         }  \
         ddsStackSet(&dds_tmp, reset);  \
      } else {  \
         if(MISALIGN(LT3, LP3)) {  \
            lp3a = ddsStackTmp(MAX3, DDS_##LT3##_P);  \
            CON4_VV_##F3(LT3, RT3, lp3a, RP3, MAX3)  \
            memcpy(LP3, lp3a, MAX3 * DDS_##LT3##_P);  \
            ddsStackSet(&dds_tmp, reset);  \
         } else {  \
            CON4_VV_##F3(LT3, RT3, LP3, RP3, MAX3)  \
         }  \
      }  \
   }


#define CON3_AA_VS(F3, LT3, RT3, LP3, RP3, MAX3)  \
   CON4_VS_##F3(LT3, RT3, LP3, RP3, MAX3)

#define CON3_AN_VS(F3, LT3, RT3, LP3, RP3, MAX3)  \
   {size_t reset = dds_tmp.top;  \
      void *rp3a;  \
      if(MISALIGN(RT3, RP3)) {  \
         rp3a = ddsStackTmp(1, DDS_##RT3##_P);  \
         memcpy(rp3a, RP3, DDS_##RT3##_P);  \
         CON4_VS_##F3(LT3, RT3, LP3, rp3a, MAX3)  \
         ddsStackSet(&dds_tmp, reset);  \
      } else {  \
         CON4_VS_##F3(LT3, RT3, LP3, RP3, MAX3)  \
      }  \
   }

#define CON3_NA_VS(F3, LT3, RT3, LP3, RP3, MAX3)  \
   {size_t reset = dds_tmp.top;  \
      void *lp3a;  \
      if(MISALIGN(LT3, LP3)) {  \
         lp3a = ddsStackTmp(MAX3, DDS_##LT3##_P);  \
         CON4_VS_##F3(LT3, RT3, lp3a, RP3, MAX3)  \
         memcpy(LP3, lp3a, MAX3 * DDS_##LT3##_P);  \
         ddsStackSet(&dds_tmp, reset);  \
      } else {  \
         CON4_VS_##F3(LT3, RT3, LP3, RP3, MAX3)  \
      }  \
   }

#define CON3_NN_VS(F3, LT3, RT3, LP3, RP3, MAX3)  \
   {size_t reset = dds_tmp.top;  \
      void *lp3a, *rp3a;  \
      if(MISALIGN(RT3, RP3)) {  \
         rp3a = ddsStackTmp(1, DDS_##RT3##_P);  \
         memcpy(rp3a, RP3, DDS_##RT3##_P);  \
         if(MISALIGN(LT3, LP3)) {  \
            lp3a = ddsStackTmp(MAX3, DDS_##LT3##_P);  \
            CON4_VS_##F3(LT3, RT3, lp3a, rp3a, MAX3)  \
            memcpy(LP3, lp3a, MAX3 * DDS_##LT3##_P);  \
         } else {  \
            CON4_VS_##F3(LT3, RT3, LP3, rp3a, MAX3)  \
         }  \
         ddsStackSet(&dds_tmp, reset);  \
      } else {  \
         if(MISALIGN(LT3, LP3)) {  \
            lp3a = ddsStackTmp(MAX3, DDS_##LT3##_P);  \
            CON4_VS_##F3(LT3, RT3, lp3a, RP3, MAX3)  \
            memcpy(LP3, lp3a, MAX3 * DDS_##LT3##_P);  \
            ddsStackSet(&dds_tmp, reset);  \
         } else {  \
            CON4_VS_##F3(LT3, RT3, LP3, RP3, MAX3)  \
         }  \
      }  \
   }

#endif /* __DDS_CON3_H */
