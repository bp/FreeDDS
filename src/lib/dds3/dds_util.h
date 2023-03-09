#ifndef __DDS_UTIL_H
#define __DDS_UTIL_H

/*
RCS-t=DDS, private "utility" declarations

$Id: dds_util.h 229 2011-06-02 18:53:57Z user $
*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

/* maximum value of size_t */
#ifndef SIZE_T_MAX
#define SIZE_T_MAX (~ (size_t)0)
#endif

#define ROUND(x) floor(.5 + (x))

/*********************************/
/* typedef and enumerated values */
/*********************************/

typedef unsigned DDS_HASH;              /* name hash value */


typedef enum DDS_BOOLEAN
{  /* boolean values, for convenience */
   DDS_FALSE    =  0,
   DDS_TRUE     =  1,
   DDS_NO       =  0,
   DDS_YES      =  1,
   DDS_UNKNOWN  = -1
} DDS_BOOLEAN;

/***********************************************************/
/* DDS_HEAP is used to validate dds malloc and free usage. */
/* Validation is turned on by defining DDS_HEAP_CHECK,     */
/* and recompiling DDS malloc.c, calloc.c, and realloc.c.  */
/* All blocks are validated whenever the heap is changed.  */
/*                                                         */
/* The size of each "user" buffer is increased for checks. */
/* Three copies of DDS_HEAP exist for each active block.   */
/*    1) immediately before the "user" buffer.             */
/*    2) immediately after  the "user" buffer.             */
/*    3) within the dds_heap "stack"                       */
/* (1) and (2) are used to detect "user" buffer overruns.  */
/* If the block is freed, (1) and (2) are discarded and    */
/* the "size" of (3) is reset to zero.                     */
/*                                                         */
/* DDS_HEAP_CREEP is the number of bytes needed for one    */
/* DDS_HEAP struct, rounded up to nominal alignment.       */
/***********************************************************/
typedef struct DDS_HEAP
{  /* heap validation descriptor */
   void *ptr;           /* returned by malloc */
   size_t size;         /* passed to malloc */
   size_t idx;          /* dds_heap stack index */
   void *ptr2; size_t size2, idx2; /* duplicates of originals */
} DDS_HEAP;

/* ieee, ibm and PC conversion support */
/* "ieeeibm.c" assumes "int" and "unsigned" are 4 bytes */
typedef int             DDS_IEEE4_IN;       /* input  4 byte ieee float */
typedef unsigned        DDS_IBM4_IN;        /* input  4 byte ibm float  */
typedef unsigned short  DDS_INT2_IN;        /* input  2 byte integer */
typedef unsigned        DDS_INT4_IN;        /* input  4 byte integer */
typedef unsigned        DDS_INT8_IN;        /* input  8 byte integer */
typedef unsigned        DDS_IBM8_IN;        /* input  8 byte ibm float  */
typedef int             DDS_IEEE4_OUT;      /* output 4 byte ieee float */
typedef int             DDS_IBM4_OUT;       /* output 4 byte ibm float  */
typedef unsigned short  DDS_INT2_OUT;       /* output 2 byte integer    */
typedef unsigned        DDS_INT4_OUT;       /* output 4 byte integer    */
typedef unsigned        DDS_INT8_OUT;       /* output 8 byte integer    */
typedef int             DDS_IEEE8_OUT;      /* output 8 byte ieee float */


typedef enum UTIL_MISC
{  /* miscellaneous constants for utilites */
   DDS_HEAP_CREEP = (HOST_MALLOC_A *
      ((2 * sizeof(DDS_HEAP) - 1) / HOST_MALLOC_A))
} UTIL_MISC;

/*****************************************/
/* extern global variables and constants */
/*****************************************/

/* heap validation descriptors */
extern DDS_STACK dds_heap;
extern double dds_mem_use;
extern double dds_mem_max;
extern double dds_samples_in;
extern double dds_samples_out;

extern DDS_HEAP dds_global_start;
extern DDS_HEAP dds_global_end;
 
/* global physical constants */
extern const double MM_PER_INCH;
extern const double CM_PER_INCH;
extern const double METER_PER_FOOT;
extern const int    INCHES_PER_FOOT;
extern const double INCHES_PER_METER;
extern const double INCHES_PER_CM;
extern const double FEET_PER_METER;
extern const double FEET_PER_INCH;
extern const int    USEC_PER_MSEC;
extern const int    USEC_PER_SEC;
extern const double MSEC_PER_USEC;
extern const int    MSEC_PER_SEC;
extern const double SEC_PER_MSEC;
extern const double SEC_PER_USEC;


/***********************/
/* function prototypes */
/***********************/

int ddsCmpDbl(
   double x,
   double y)
;


size_t ddsPrimeUp(
  size_t num)
;


DDS_HASH ddsHash(
   const char *name)
;


void * ddsZalloc(
   size_t size)
;
void * ddsMalloc(
   size_t size)
;
void * ddsRealloc(
   void *old,
   size_t size)
;
void ddsFree(
   void *mem)
;
int ddsHeapCheck(
   void *ptr)
;


char * ddsFtnCpy(
   char *array,
   size_t size,
   const char *str)
;


const char *GetArgEnvV(
   int n)
;
const char *GetArgCmdV(
   int n)
;


int ddsIBMIEEE(
   DDS_IBM4_OUT *out,
   DDS_IEEE4_IN *inp,
   int nsm)
;
int ddsIEEEIBM(
   DDS_IEEE4_OUT *out,      /* maybe same as inp */
   DDS_IBM4_IN *inp,
   int nsm)
;
int ddsIBMIEEEX(
   DDS_IBM4_OUT *out,
   DDS_IEEE4_IN *inp,
   int nsm)
;
int ddsIEEEXIBM(
   DDS_IEEE4_OUT *out,      /* maybe same as inp */
   DDS_IBM4_IN *inp,
   int nsm)
;
int ddsIEEEIBM8(
   DDS_IEEE8_OUT *out,      /* maybe same as inp */
   DDS_IBM8_IN *inp,
   int nsm)
;


void ddsFake2Float(
   float *fvalue,
   const int *hi,
   const int *lo)
;
void ddsFloat2Fake(
   const float *fvalue,
   int *hi,
   int *lo)
;

#endif /* __DDS_UTIL_H */
