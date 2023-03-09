#ifndef __DDS_API_H
#define __DDS_API_H

/*
RCS-t=DDS, application program interface

$Id: dds_api.h 135 2010-01-26 00:52:33Z user $
*/

/**********************************************/
/***   Copyright (C) 1996,2004              ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

/********************************************************/
/* Application Program Interface, entry and exit macros */
/********************************************************/

#define DDS_API_MAX  64  /* max api call stack depth */

#define DDS_API_INIT  if(dds_debug_api & DDS_DEBUG_INIT) cdds_init();
 
#define DDS_API_ENTRY(NAME) \
   if(dds_debug_api & DDS_DEBUG_INIT) cdds_init();  \
   if(dds_debug_api & DDS_DBG_TRACE) {  \
      ddsPrintStr("DDS: TRACE: "); ddsPrintStr(NAME); ddsPrintStr("\n");}  \
   if(dds_debug_api & DDS_DBG_CALL) ddsHeapCheck(NULL);  \
   dds_api_call[dds_api_next++] = NAME;  \
   if(dds_api_next > DDS_API_MAX) ddsPanic("API call stack overflow\n");

#define DDS_API_EXIT(STATUS) dds_api_next--; return (STATUS);
 
#define DDS_API_EXIT_VOID() dds_api_next--; return;

/**********************************************/
/* Conversion from John's Special Stream type */
/**********************************************/

#define STREAM_TO_SYM_TAG(TAG, SWAP) \
   if ((TAG) == -DDS_CHAR) { \
      (TAG) = -(TAG); \
      (SWAP) = 1; \
   } else if ((TAG) == -DDS_SHORT) { \
      (TAG) = -(TAG); \
      (SWAP) = 2; \
   } else if ((TAG) == -DDS_INT) { \
      (TAG) = -(TAG); \
      (SWAP) = 4; \
   } else if ((SWAP)) { \
      (SWAP) = 0; \
   }
 
/****************************************************************/
/* macro for API arguments (checking and non-checking versions) */
/****************************************************************/
 
#ifdef DDS_SAFE_API
 
   #define SYM_TO_SYM_TAG(TAG, STACK, SYM) \
      if(SYM) { \
         if((SYM)->tag == -1) { \
            (TAG) = ddsStackPush((STACK), sizeof(DDS_SYM_TAG), 1) / \
               sizeof(DDS_SYM_TAG); \
            ((DDS_SYM_TAG*)((STACK)->ptr))[(TAG)].sym = (DDS_SYM*)(SYM); \
            ((DDS_SYM_TAG*)((STACK)->ptr))[(TAG)].offset = (long)-1; \
            (SYM)->tag = (TAG); \
         } else if((SYM)->tag) { \
            (TAG) = (SYM)->tag; \
         } else { \
            ddsPanic("SYM_TO_SYM_TAG: bad symbol tag\n"); \
         } \
      } else { \
         (TAG) = -1; \
      }

   #define BIN_TAG_TO_BIN(BIN, BIN_TAG, FAIL) \
      if(0 > (BIN_TAG) || (BIN_TAG) >= dds_bin_max) { \
         errno = EINVAL; \
         ddsDebug("bad binary tag= %i\n", (int)(BIN_TAG)); \
         FAIL \
      } else { \
         (BIN) = dds_bin[(BIN_TAG)]; \
         if(! (BIN)) { \
            errno = EINVAL; \
            ddsDebug("binary not open, bin_tag= %i\n", (int)(BIN_TAG)); \
            FAIL \
         } \
         if((BIN)->state & (DDS_BIN_RESERVED)) ddsDelayedOpen((int)(BIN_TAG)); \
      }
 
   #define SYM_TAG_TO_MEMBER(MEMBER, BIN, TAG, FAIL) \
      if((TAG) < 0 || \
         (TAG) * sizeof(DDS_SYM_TAG) >= (BIN)->sym_tag.top) { \
         errno = EINVAL; \
         ddsDebug("bad member tag= %i\n\tbinary= %s\n", \
            (int)(TAG), (BIN)->name); \
         FAIL \
      } else { \
         (MEMBER) = (DDS_MEMBER*)((DDS_SYM_TAG*)((BIN)->sym_tag.ptr))[(TAG)].sym; \
         if(! ((MEMBER)->flag & DDS_SYM_MEMBER)) { \
            errno = EINVAL; \
            ddsDebug("member not defined, sym_tag= %i, symbol= %s\n\tbinary= %s\n", \
               (int)(TAG), (MEMBER)->name ? (MEMBER)->name : "<anonymous>", \
               (BIN)->name); \
            FAIL \
         } \
      }
 
   #define SYM_TAG_TO_TYPE(TYPE, BIN, TAG, FAIL) \
      { DDS_SYM *sym; \
         if((TAG) < 0 || \
            (TAG) * sizeof(DDS_SYM_TAG) >= (BIN)->sym_tag.top) { \
            errno = EINVAL; \
            ddsDebug("bad field tag= %i\n\tbinary= %s\n", \
               (int)(TAG), (BIN)->name); \
            FAIL \
         } else { \
            sym = ((DDS_SYM_TAG*)((BIN)->sym_tag.ptr))[(TAG)].sym; \
            if(sym->flag & DDS_SYM_TYPE) { \
               (TYPE) = (DDS_TYPE*)sym; \
            } else if(sym->flag & (DDS_SYM_OBJECT | DDS_SYM_EXPR | \
               DDS_SYM_BLOCK | DDS_SYM_MEMBER)) { \
               (TYPE) = sym->type; \
            } else { \
               (TYPE) = NULL; \
            } \
            if(! (TYPE)) { \
               errno = EINVAL; \
               ddsDebug("type not defined, sym_tag= %i, symbol= %s\n\tbinary= %s\n", \
                  (int)(TAG), sym->name ? sym->name : "<anonymous>", \
                  (BIN)->name); \
               FAIL \
            } \
         } \
      }
 
   #define SYM_TAG_TO_SYM(SYM, BIN, TAG, FAIL) \
      if((TAG) < 0 || \
         (TAG) * sizeof(DDS_SYM_TAG) >= (BIN)->sym_tag.top) { \
         errno = EINVAL; \
         ddsDebug("bad field tag= %i\n\tbinary= %s\n", \
            (int)(TAG), (BIN)->name); \
         FAIL \
      } else { \
         (SYM) = ((DDS_SYM_TAG*)((BIN)->sym_tag.ptr))[(TAG)].sym; \
      }
 
   #define SYM_TAG_TO_BIAS(BIAS, BIN, TAG, FAIL) \
      if((TAG) < 0 || \
         (TAG) * sizeof(DDS_SYM_TAG) >= (BIN)->sym_tag.top) { \
         errno = EINVAL; \
         ddsDebug("bad field tag= %i\n\tbinary= %s\n", \
            (int)(TAG), (BIN)->name); \
         FAIL \
      } else { \
         (BIAS) = &((DDS_SYM_TAG*)((BIN)->sym_tag.ptr))[(TAG)].offset; \
      }
 
   #define VALID_PRIME(PRIME, FAIL) \
      if(DDS_CTYPE_PTR > (PRIME) || (PRIME) >= DDS_STYPE_MAX) { \
         ddsDebug("bad prime type= %i\n", (int)(PRIME)); \
         FAIL \
      }
 
 
   #define NON_NEGATIVE(VALUE, BIN, FAIL) \
      if((VALUE) < 0) { \
         errno = EINVAL; \
         ddsDebug("negative argument, " #VALUE "= %i\n\tbinary= %s\n", \
            (int)(VALUE), (BIN)->name); \
         FAIL \
      }
 
   #define NON_NULL(BUF, BIN, FAIL) \
      if(! (BUF)) { \
         errno = EINVAL; \
         ddsDebug("'" #BUF "' argument is NULL\n\tbinary= %s\n", \
            (BIN)->name); \
         FAIL \
      }
 
   #define WHENCE_CODE(VALUE, BIN, FAIL) \
      if((VALUE) < SEEK_SET || (VALUE) > SEEK_END) { \
         errno = EINVAL; \
         ddsDebug("invalid argument, " #VALUE "= %i\n\tbinary= %s\n", \
            (int)(VALUE), (BIN)->name); \
         FAIL \
      }
 
#else /* DDS_SAFE_API */
 
   #define BIN_TAG_TO_BIN(BIN, BIN_TAG, FAIL) \
      (BIN) = dds_bin[(BIN_TAG)]; \
      if((BIN)->state & (DDS_BIN_RESERVED)) ddsDelayedOpen((int)(BIN_TAG));
 
   #define SYM_TO_SYM_TAG(TAG, STACK, SYM) \
      if(SYM) { \
         if((SYM)->tag == -1) { \
            (TAG) = ddsStackPush((STACK), sizeof(DDS_SYM_TAG), 1) / \
               sizeof(DDS_SYM_TAG); \
            ((DDS_SYM_TAG*)((STACK)->ptr))[(TAG)].sym = (DDS_SYM*)(SYM); \
            ((DDS_SYM_TAG*)((STACK)->ptr))[(TAG)].offset = (long)-1; \
            (SYM)->tag = (TAG); \
         } else (TAG) = (SYM)->tag; \
      } else (TAG) = -1; 
 
   #define SYM_TAG_TO_MEMBER(MEMBER, BIN, TAG, FAIL) \
      (MEMBER) = (DDS_MEMBER*)((DDS_SYM_TAG*)((BIN)->sym_tag.ptr))[(TAG)].sym;
 
   #define SYM_TAG_TO_TYPE(TYPE, BIN, TAG, FAIL) \
      (TYPE) = (DDS_TYPE*)((DDS_SYM_TAG*)((BIN)->sym_tag.ptr))[(TAG)].sym; \
      if(! ((TYPE)->flag & DDS_SYM_TYPE)) (TYPE) = (TYPE)->type; 
 
   #define SYM_TAG_TO_SYM(SYM, BIN, TAG, FAIL) \
      (SYM) = ((DDS_SYM_TAG*)((BIN)->sym_tag.ptr))[(TAG)].sym;
 
   #define SYM_TAG_TO_BIAS(BIAS, BIN, TAG, FAIL) \
      (BIAS) = &((DDS_SYM_TAG*)((BIN)->sym_tag.ptr))[(TAG)].offset;
 
   #define VALID_PRIME(PRIME, FAIL)
 
   #define NON_NEGATIVE(VALUE, BIN, FAIL)
 
   #define NON_NULL(VALUE, BIN, FAIL)
 
   #define WHENCE_CODE(VALUE, BIN, FAIL)
 
#endif /* DDS_SAFE_API */

/* get Fake Float data into host array */
#define GET_FFLOAT(TYPE, BIN_TAG, SYM_TAG, BUF, ARRAY, COUNT)      \
    if ((SYM_TAG) < 0) { \
       float *ffloat = (float*)malloc((COUNT)*sizeof(float)); \
       num = cdds_getff((BIN_TAG), (SYM_TAG), (BUF), ffloat, (COUNT)); \
       CON_AAVV(DDS_CTYPE_##TYPE, DDS_CTYPE_FLOAT, (ARRAY), ffloat, num); \
       free(ffloat); \
       DDS_API_EXIT(num); \
    }

/* put Fake Float data into buffer data */
#define PUT_FFLOAT(TYPE, BIN_TAG, SYM_TAG, BUF, ARRAY, COUNT)      \
    if ((SYM_TAG) < 0) { \
       float *ffloat = (float*)malloc((COUNT)*sizeof(float)); \
       CON_AAVV(DDS_CTYPE_FLOAT, DDS_CTYPE_##TYPE, ffloat, (ARRAY), (COUNT)); \
       num = cdds_putff((BIN_TAG), (SYM_TAG), (BUF), ffloat, (COUNT)); \
       free(ffloat); \
       DDS_API_EXIT(num); \
    }

#if defined(_WIN32) || defined(MINGW)

/* get buffer data into host array with pad */
#define GET_DATA(FUNC, TYPE, PAD, BIN_TAG, SYM_TAG, BUF, ARRAY, COUNT) \
    { DDS_BIN *bin;  \
      DDS_TYPE *type;  \
      DDS_SYM *sym;  \
      DDS_CON_STATUS status;  \
      size_t prec, num, nbytes; \
      size_t *bias; \
      DDS_API_ENTRY(FUNC)  \
      GET_FFLOAT(TYPE, BIN_TAG, SYM_TAG, BUF, ARRAY, COUNT);    \
      BIN_TAG_TO_BIN(bin, (BIN_TAG), DDS_API_EXIT(0));  \
      NON_NULL((BUF), bin, DDS_API_EXIT(0));  \
      NON_NULL((ARRAY), bin, DDS_API_EXIT(0));  \
      NON_NEGATIVE((COUNT), bin, DDS_API_EXIT(0));  \
      SYM_TAG_TO_SYM(sym, bin, SYM_TAG, DDS_API_EXIT(0));  \
      SYM_TAG_TO_BIAS(bias, bin, SYM_TAG, DDS_API_EXIT(0));  \
      num = 0; \
      if(sym->type) {  \
         type = sym->type;  \
         prec = dds_prime[type->ptype].prec;  \
         if(*bias < 0) {  \
            ddsDebug("cdds_" FUNC ": offset not set\n\tbin=%s\n\tsymbol= %s\n", \
               bin->name, sym->name ? sym->name : "<anonymous>"); \
         } else if (!prec) {  \
            ddsDebug("cdds_" FUNC ": invalid type\n\tbin=%s\n\tsymbol= %s, type= %s\n",  \
               bin->name, sym->name ? sym->name : "<anonymous>",  \
               ddsHintType(bin, type));  \
         } else {  \
            num = type->stroke / prec;  \
            if(num > (COUNT)) {  \
               nbytes = 0;  \
               num = (COUNT);  \
            } else {  \
               nbytes = ((COUNT) - num) * HOST_##TYPE##_P;  \
            }  \
            status = CON_ANVV(DDS_CTYPE_##TYPE, type->ptype,  \
               (ARRAY), (char*)(BUF) + *bias, num);  \
            if(nbytes && status == DDS_CON_OK)  \
               memset((char*)(ARRAY) + num * HOST_##TYPE##_P, (PAD), nbytes);  \
         }  \
      } else {  \
         ddsDebug("cdds_" FUNC ": type not defined\n\tbin=%s\n\tsymbol= %s\n", \
            bin->name, sym->name ? sym->name : "<anonymous>"); \
      } \
      DDS_API_EXIT(num); \
   }

/* put host array into buffer data with pad */
#define PUT_DATA(FUNC, TYPE, PAD, BIN_TAG, SYM_TAG, BUF, ARRAY, COUNT) \
    { DDS_BIN *bin;  \
      DDS_TYPE *type;  \
      DDS_SYM *sym;  \
      DDS_CON_STATUS status;  \
      size_t prec, num, nbytes; \
      size_t *bias;  \
      DDS_API_ENTRY(FUNC)  \
      PUT_FFLOAT(TYPE, BIN_TAG, SYM_TAG, BUF, ARRAY, COUNT);    \
      BIN_TAG_TO_BIN(bin, (BIN_TAG), DDS_API_EXIT(0));  \
      NON_NULL((BUF), bin, DDS_API_EXIT(0));  \
      NON_NULL((ARRAY), bin, DDS_API_EXIT(0));  \
      NON_NEGATIVE((COUNT), bin, DDS_API_EXIT(0));  \
      SYM_TAG_TO_SYM(sym, bin, (SYM_TAG), DDS_API_EXIT(0));  \
      SYM_TAG_TO_BIAS(bias, bin, (SYM_TAG), DDS_API_EXIT(0));  \
      num = 0;  \
      if(sym->type) {  \
         type = sym->type;  \
         prec = dds_prime[type->ptype].prec;  \
         if (*bias < 0) {\
            ddsDebug("cdds_" FUNC ": offset not set\n\tbin=%s\n\tsymbol= %s\n",  \
               bin->name, sym->name ? sym->name : "<anonymous>"); \
			} else if(!prec) {  \
            ddsDebug("cdds_" FUNC ": invalid type\n\tbin=%s\n\tsymbol= %s, type= %s\n",  \
               bin->name, sym->name ? sym->name : "<anonymous>",  \
               ddsHintType(bin, type));  \
         } else {  \
            num = type->stroke / prec;  \
            if(num > (COUNT)) {  \
               nbytes = (num - (COUNT)) * prec;  \
               num = (COUNT);  \
            } else {  \
               nbytes = 0;  \
            }  \
            status = CON_NAVV(type->ptype, DDS_CTYPE_##TYPE,  \
               (char*)(BUF) + *bias, (ARRAY), num);  \
            if(nbytes && status == DDS_CON_OK)  \
               memset((char*)(BUF) + num * prec, (PAD), nbytes);  \
         }  \
      } else {  \
         ddsDebug("cdds_" FUNC ": type not defined\n\tbin=%s\n\tsymbol= %s\n",  \
            bin->name, sym->name ? sym->name : "<anonymous>");  \
      }  \
      DDS_API_EXIT(num);  \
   }
#else

/* get buffer data into host array with pad */
#define GET_DATA(FUNC, TYPE, PAD, BIN_TAG, SYM_TAG, BUF, ARRAY, COUNT) \
    { DDS_BIN *bin;  \
      DDS_TYPE *type;  \
      DDS_SYM *sym;  \
      DDS_CON_STATUS status;  \
      size_t prec, num, nbytes; \
      long *bias; \
      DDS_API_ENTRY(FUNC)  \
      GET_FFLOAT(TYPE, BIN_TAG, SYM_TAG, BUF, ARRAY, COUNT);    \
      BIN_TAG_TO_BIN(bin, (BIN_TAG), DDS_API_EXIT(0));  \
      NON_NULL((BUF), bin, DDS_API_EXIT(0));  \
      NON_NULL((ARRAY), bin, DDS_API_EXIT(0));  \
      NON_NEGATIVE((COUNT), bin, DDS_API_EXIT(0));  \
      SYM_TAG_TO_SYM(sym, bin, SYM_TAG, DDS_API_EXIT(0));  \
      SYM_TAG_TO_BIAS(bias, bin, SYM_TAG, DDS_API_EXIT(0));  \
      num = 0; \
      if(sym->type) {  \
         type = sym->type;  \
         prec = dds_prime[type->ptype].prec;  \
         if(*bias < 0) {  \
            ddsDebug("cdds_" FUNC ": offset not set\n\tbin=%s\n\tsymbol= %s\n", \
               bin->name, sym->name ? sym->name : "<anonymous>"); \
         } else if (!prec) {  \
            ddsDebug("cdds_" FUNC ": invalid type\n\tbin=%s\n\tsymbol= %s, type= %s\n",  \
               bin->name, sym->name ? sym->name : "<anonymous>",  \
               ddsHintType(bin, type));  \
         } else {  \
            num = type->stroke / prec;  \
            if(num > (COUNT)) {  \
               nbytes = 0;  \
               num = (COUNT);  \
            } else {  \
               nbytes = ((COUNT) - num) * HOST_##TYPE##_P;  \
            }  \
            status = CON_ANVV(DDS_CTYPE_##TYPE, type->ptype,  \
               (ARRAY), (char*)(BUF) + *bias, num);  \
            if(nbytes && status == DDS_CON_OK)  \
               memset((char*)(ARRAY) + num * HOST_##TYPE##_P, (PAD), nbytes);  \
         }  \
      } else {  \
         ddsDebug("cdds_" FUNC ": type not defined\n\tbin=%s\n\tsymbol= %s\n", \
            bin->name, sym->name ? sym->name : "<anonymous>"); \
      } \
      DDS_API_EXIT(num); \
   }

/* put host array into buffer data with pad */
#define PUT_DATA(FUNC, TYPE, PAD, BIN_TAG, SYM_TAG, BUF, ARRAY, COUNT) \
    { DDS_BIN *bin;  \
      DDS_TYPE *type;  \
      DDS_SYM *sym;  \
      DDS_CON_STATUS status;  \
      size_t prec, num, nbytes; \
      long *bias;  \
      DDS_API_ENTRY(FUNC)  \
      PUT_FFLOAT(TYPE, BIN_TAG, SYM_TAG, BUF, ARRAY, COUNT);    \
      BIN_TAG_TO_BIN(bin, (BIN_TAG), DDS_API_EXIT(0));  \
      NON_NULL((BUF), bin, DDS_API_EXIT(0));  \
      NON_NULL((ARRAY), bin, DDS_API_EXIT(0));  \
      NON_NEGATIVE((COUNT), bin, DDS_API_EXIT(0));  \
      SYM_TAG_TO_SYM(sym, bin, (SYM_TAG), DDS_API_EXIT(0));  \
      SYM_TAG_TO_BIAS(bias, bin, (SYM_TAG), DDS_API_EXIT(0));  \
      num = 0;  \
      if(sym->type) {  \
         type = sym->type;  \
         prec = dds_prime[type->ptype].prec;  \
         if (*bias < 0) {\
            ddsDebug("cdds_" FUNC ": offset not set\n\tbin=%s\n\tsymbol= %s\n",  \
               bin->name, sym->name ? sym->name : "<anonymous>"); \
			} else if(!prec) {  \
            ddsDebug("cdds_" FUNC ": invalid type\n\tbin=%s\n\tsymbol= %s, type= %s\n",  \
               bin->name, sym->name ? sym->name : "<anonymous>",  \
               ddsHintType(bin, type));  \
         } else {  \
            num = type->stroke / prec;  \
            if(num > (COUNT)) {  \
               nbytes = (num - (COUNT)) * prec;  \
               num = (COUNT);  \
            } else {  \
               nbytes = 0;  \
            }  \
            status = CON_NAVV(type->ptype, DDS_CTYPE_##TYPE,  \
               (char*)(BUF) + *bias, (ARRAY), num);  \
            if(nbytes && status == DDS_CON_OK)  \
               memset((char*)(BUF) + num * prec, (PAD), nbytes);  \
         }  \
      } else {  \
         ddsDebug("cdds_" FUNC ": type not defined\n\tbin=%s\n\tsymbol= %s\n",  \
            bin->name, sym->name ? sym->name : "<anonymous>");  \
      }  \
      DDS_API_EXIT(num);  \
   }
#endif
/**********************************/
/* global variables and constants */
/**********************************/

extern int dds_api_next;                /* API call, next stack entry */
extern char *dds_api_call[DDS_API_MAX]; /* API call name stack */

#endif /* __DDS_API_H */
