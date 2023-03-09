
/***********************************************************/
/*                 Copyright (C) 2009                      */
/*                BP America, Houston, TX                  */
/***********************************************************/
/*                                                         */
/* This file is the common part of host.h.  The previous   */
/* part of host.h is generated automatically by "makehost" */
/* for each specific platform.                             */
/*                                                         */
/* Jerry Ehlers - January 2009                             */
/***********************************************************/

/*
RCS-t=HOST, C host, target architecture and dependencies

$Id: chost_com.h 163 2010-07-28 21:37:22Z user $
*/

#include <stddef.h>
#include <math.h>
#include <limits.h>

/*
   Macros are defined that identify various computer architectures.
   Macro naming conventions are consistence among the hosts.

      Example:  #define HOST_Sun           1
      Example:  #define HOST_sun4cg92      1

   Macros are also defined that describe host "C" types.
   This solves some portablility problems among architectures.
   For example, does *this* host support "long long int"?
   "Best" alternative, if not supported?  Byte precision?
   Required byte alignment? Corresponding DDS enum?

      Macro
      Name      Defined
      Suffix    Value
         _C        Defined only if type *is* supported in "C".
         _DEF      type specifier, or its "best" approximation.
         _P        Precision of type, or its "best" approximation.
         _A        Alignment of type or its "best" approximation.
         _DDS      Corresponding DDS enumeration.
         _T        typedef, derived from "_DEF"
         _QS       quoted scan format conversion specifier.
                      INT6, LLONG and ULLONG only.
         _QP       quoted print format conversion specifier.
                      INT6, LLONG and ULLONG only.

      Macro base names are VOID, PTR, CHAR, UCHAR, SCHAR,
         SHORT, INT, LONG, LLONG, USHORT, UINT, ULONG, ULLONG,
         FLOAT, DOUBLE, LDOUBLE.

      Example: "sun4" supports "int" integer types.
         Example:  #define HOST_INT_C         1
         Example:  #define HOST_INT_DEF       int
         Example:  #define HOST_INT_P         sizeof(HOST_INT_T)
         Example:  #define HOST_INT_A         HOST_INT_P
         Example:  #define HOST_INT_DDS       DDS_INTEGER4
         Example:  typedef HOST_INT_DEF       HOST_INT_T;

      Example: "sun4" does not support "long long" (use "long").
         Example:  #define HOST_LLONG_QS      "%li"
         Example:  #define HOST_LLONG_QP      "%li"
         Example:  #undef  HOST_LLONG_C
         Example:  #define HOST_LLONG_DEF     long
         Example:  #define HOST_LLONG_P       sizeof(HOST_LLONG_T)
         Example:  #define HOST_LLONG_A       HOST_LLONG_P
         Example:  #define HOST_LLONG_DDS     HOST_LONG_DDS
         Example:  typedef HOST_LLONG_DEF     HOST_LLONG_T;

   HOST_FAKE8 is number of "long" ints required to fake 8 byte integer.
   Defined if and only if, 8 byte integers are *not* intrinsic.
   See also "INT6", "INT8" and "UINT8" macro definitions.
*/


/*************************************************************/
/*** detect "$(TARCH)" and define "HOST_$(TARCH)"          ***/
/*************************************************************/

/* DDS make puts "-D$(ARCH)" on the compile line, */
/* i.e. the nominal architecture selection mechanism. */

#if defined(Linux) || defined(linux) || defined(LINUX)
   #define HOST_Linux        1
#elif defined(i86pc) || defined(sun4u)
   #define HOST_Sun          1
#elif defined(sun4)
   #define HOST_Sun          1
   #define HOST_sun4         1
#elif defined(sun4cg92) || defined(__SVR4)
   #define HOST_Sun          1
   #define HOST_sun4cg92     1
#elif defined(sun4fs64)
   #define HOST_Sun          1
   #define HOST_sun4fs64     1
#elif defined(sgi) || defined(IRIX32) || defined(IRIX64)
   #define HOST_SGI          1
#elif defined(__FreeBSD__)
   #define HOST_freebsd      1
#elif defined (_WIN32) || defined(__MINGW32__) || defined(__MINGW64__)
   #define HOST_Windows
#elif defined (__APPLE__)
   #define HOST_Macos
#endif


/************************************************************/
/* Define lseek64 when necessary                            */
/************************************************************/

#undef  HOST_lseek64
#if !defined(HOST_freebsd) && defined(_FILE_OFFSET_BITS) && (_FILE_OFFSET_BITS == 64)
   #define HOST_lseek64       lseek64
#endif


/************************************/
/* special cases                    */
/************************************/

#define HOST_VOID_C       1
#define HOST_VOID_DEF     void /* bogus */
#define HOST_VOID_P       0
#define HOST_VOID_A       1
#define HOST_VOID_DDS     DDS_CTYPE_VOID
typedef HOST_VOID_DEF     HOST_VOID_T;



#define HOST_PTRDIFF_T_C     1
#define HOST_PTRDIFF_T_DEF   ptrdiff_t
#define HOST_PTRDIFF_T_P     sizeof(ptrdiff_t)
#define HOST_PTRDIFF_T_A     HOST_PTRDIFF_T_P
#define HOST_PTRDIFF_T_DDS   HOST_LONG_DDS
typedef HOST_PTRDIFF_T_DEF   HOST_PTRDIFF_T_T;


#define HOST_SIZE_T_C        1
#define HOST_SIZE_T_DEF      size_t
#define HOST_SIZE_T_P        sizeof(size_t)
#define HOST_SIZE_T_A        HOST_SIZE_T_P
#define HOST_SIZE_T_DDS      HOST_ULONG_DDS
typedef HOST_SIZE_T_DEF      HOST_SIZE_T_T;


/************************************************************/
/* Determine if we need to fake some types                  */
/************************************************************/

#ifndef HOST_LDOUBLE_C
   #define HOST_LDOUBLE_DEF   HOST_DOUBLE_DEF
   #define HOST_LDOUBLE_P     HOST_DOUBLE_P
   #define HOST_LDOUBLE_A     HOST_DOUBLE_A
   #define HOST_LDOUBLE_DDS   HOST_DOUBLE_DDS
   typedef HOST_DOUBLE_DEF    HOST_LDOUBLE_T;
#endif

#ifndef  HOST_LDBLCPX_C
   #define HOST_LDBLCPX_DEF   HOST_DBLCPX_DEF
   #define HOST_LDBLCPX_P     HOST_DBLCPX_P
   #define HOST_LDBLCPX_A     HOST_DBLCPX_A
   #define HOST_LDBLCPX_DDS   HOST_DBLCPX_DDS
   typedef HOST_DBLCPX_DEF    HOST_LDBLCPX_T;
#endif 

#ifndef HOST_LLONG_C
   #define HOST_LLONG_DEF     HOST_LONG_DEF
   #define HOST_LLONG_P       HOST_LONG_P
   #define HOST_LLONG_A       HOST_LONG_A
   #define HOST_LLONG_DDS     HOST_LONG_DDS
   typedef HOST_LONG_DEF      HOST_LLONG_T;
#endif

#ifndef HOST_ULLONG_C
   #define HOST_ULLONG_DEF    HOST_ULONG_DEF
   #define HOST_ULLONG_P      HOST_ULONG_P
   #define HOST_ULLONG_A      HOST_ULONG_A
   #define HOST_ULLONG_DDS    HOST_ULONG_DDS
   typedef HOST_LONG_DEF      HOST_ULULONG_T;
#endif


/********************************************************/
/* Define Fortran typedefs in terms of "C"              */
/********************************************************/

/* 
   Makesure compiler options is set (eg. -qextname option)
   Othersize, conditionally modify. 
*/
#define HOST_FTN_FUNC(lname, uname) lname ## _

typedef char*                 HOST_FTN_STR;
typedef int                   HOST_FTN_LEN;
typedef int                   HOST_FTN_LOGICAL;
typedef int                   HOST_FTN_INT;
typedef float                 HOST_FTN_REAL;
typedef double                HOST_FTN_DBL;
typedef struct{float r, i;}   HOST_FTN_CPX;
typedef struct{double r, i;}  HOST_FTN_DBLCPX;
typedef void*                 HOST_FTN_PTR;
typedef int                   HOST_FTN_SUBR;

#ifdef HOST_INT2_C
   typedef HOST_INT2_DEF         HOST_FTN_INT2;
#endif
#ifdef HOST_INT4_C
   typedef HOST_INT4_DEF         HOST_FTN_INT4;
#endif
#ifdef HOST_INT8_C
   typedef HOST_INT8_DEF         HOST_FTN_INT8;
#endif

/******************************************************/
/* Support for faking INT6.                           */
/* If the host does NOT support an 8 byte integer,    */
/* then emulate using an 8 byte float (double) float. */
/* INT6 provide at least 6 bytes of integer precision */
/* (about 53 bits), sufficient for "2 gig" support.   */
/*                                                    */
/* The INT6 API Arithmetic and comparisions are done  */
/* without the aid of macros in most cases.           */
/******************************************************/

/*********************************************************/
/* Determine if we need to fake INT8 and make an attempt */
/* at a portable format specifier  for INT6              */
/*********************************************************/

#undef  HOST_FAKE8
#undef  HOST_FAKE8SIGN

#if HOST_INT_P >= 8
   #define HOST_INT6_C        1
   #define HOST_INT6_DEF      HOST_INT_DEF
   #define HOST_INT6_P        HOST_INT_P
   #define HOST_INT6_A        HOST_INT_A
   #define HOST_INT6_DDS      HOST_INT_DDS
   #define HOST_INT6_QP       "%i"
   #define HOST_INT6_QS       "%i"
#elif HOST_LONG_P >= 8
   #define HOST_INT6_C        1
   #define HOST_INT6_DEF      HOST_LONG_DEF
   #define HOST_INT6_P        HOST_LONG_P
   #define HOST_INT6_A        HOST_LONG_A
   #define HOST_INT6_DDS      HOST_LONG_DDS
   #define HOST_INT6_QP       "%li"
   #define HOST_INT6_QS       "%li"
#elif HOST_LLONG_P >= 8
   #define HOST_INT6_C        1
   #define HOST_INT6_DEF      HOST_LLONG_DEF
   #define HOST_INT6_P        HOST_LLONG_P
   #define HOST_INT6_A        HOST_LLONG_A
   #define HOST_INT6_DDS      HOST_LLONG_DDS
   #define HOST_INT6_QP       "%lli"
   #define HOST_INT6_QS       "%lli"
#else
   #define HOST_FAKE8         2
   #define HOST_FAKE8SIGN     (0x80000000)

   #define HOST_INT6_DEF      double
   #define HOST_INT6_QP       "%.0lf"
   #define HOST_INT6_QS       "%lf"

   union   onion {
	   double d;
      struct integer_form {
         unsigned long int1;
         unsigned long int0;
      } i;
   };
#endif
  
#ifdef HOST_LLONG_C 
   #define HOST_LLONG_QS      "%lli"
   #define HOST_LLONG_QP      "%lli"
#else
   #define HOST_LLONG_QS      "%li"
   #define HOST_LLONG_QP      "%li"
#endif

#ifndef HOST_INT8_C
   #define HOST_INT8_DEF     struct {long hi, lo;}
   #define HOST_UINT8_DEF    struct {long hi, lo;}
#endif


/* Make sure these follow after their definitions */
typedef HOST_INT6_DEF        HOST_INT6_T;
typedef HOST_INT8_DEF        HOST_INT8_T;
typedef HOST_UINT8_DEF       HOST_UINT8_T;

#ifdef HOST_FAKE8
   #define HOST_INT6_TO_INT8(a) HostDoubleToInt8((HOST_INT6_T)(a))
   #define HOST_INT8_TO_INT6(a) Int8ToHostDouble((HOST_INT8_T)(a))
   #define HOST_INT6_DIV(a, b) HostInt6Div((HOST_INT6_T)(a), (HOST_INT6_T)(b))
   #define HOST_INT6_MOD(a, b) fmod((double)(a), (double)(b))
#else
   #define HOST_INT6_TO_INT8(a) (a)
   #define HOST_INT8_TO_INT6(a) (a)
   #define HOST_INT6_DIV(a, b) ((a) / (b))
   #define HOST_INT6_MOD(a, b) ((a) % (b))
#endif /* HOST_FAKE8 */

/***************************************************/
/* Support for faking INT8 and UINT8.              */
/* If the host does NOT support an 8 byte integer, */
/* then emulate using a struct of two longs.       */
/* This scheme is deprecated in favor of INT6.     */
/***************************************************/

typedef struct HOST_LDIV8_T {HOST_INT8_T quot, rem;} HOST_LDIV8_T;

/* Macros to manipulate HOST_INT8_T (greater than 2 gigabytes)  */

#ifdef HOST_FAKE8
   #define HOST_DOUBLE_TO_INT8(a)    (HostDoubleToInt8(a))
   #define HOST_INT8_EQ(a)           (! (a).hi && ! (a).lo)
   #define HOST_INT8_NE(a)           ((a).hi || (a).lo)
   #define HOST_INT8_LT(a)           ((a).hi < 0)
   #define HOST_INT8_GT(a)           ((a).hi > 0 || (!(a).hi && (a).lo))
   #define HOST_INT8_EQ_LONG(a)      (((a).lo & HOST_FAKE8SIGN) ? \
      ((a).hi == (long)-1) : ((a).hi == (long)0))
   #define HOST_LLONG_EQ_LONG(a)     (((a) & HOST_FAKE8SIGN) ? \
      (((a) & (HOST_LLONG_T)HOST_FAKE8SIGN) == (HOST_LLONG_T)HOST_FAKE8SIGN) \
      : (! ((a) & (HOST_LLONG_T)HOST_FAKE8SIGN)))
   #define HOST_INT8_TO_DOUBLE(a)    (Int8ToHostDouble(a))
   #define HOST_INT8_TO_LONG(a)      (long)((a).lo)
   #define HOST_INT8_TO_LLONG(a)     (HOST_LLONG_T)((a).lo)
   #define HOST_LONG_TO_INT8(a)      HostLongToInt8((a))
   #define HOST_LLONG_TO_INT8(a)     HostLLongToInt8((a))
   #define HOST_INT8_INC(a, b)       (HostInt8Inc(&(a), (b)), (a))
   #define HOST_INT8_DEC(a, b)       (HostInt8Dec(&(a), (b)), (a))
   #define HOST_INT8_MUL(a, b)       (HostInt8Mul(&(a), (b)), (a))
   #define HOST_INT8_DIV(a, b)       (HostInt8Div(&(a), (b)), (a))
   #define HOST_INT8_MOD(a, b)       (HostInt8Mod(&(a), (b)), (a))
   #define HOST_INT8_MOD_LONG(a, b)  (((a).lo & ~ HOST_FAKE8SIGN) % (b))
#else /* HOST_FAKE8 */
   #define HOST_DOUBLE_TO_INT8(a)    ((HOST_INT8_T)(a))
   #define HOST_INT8_EQ(a)           ((a) == (HOST_INT8_T)0)
   #define HOST_INT8_NE(a)           ((a) != (HOST_INT8_T)0)
   #define HOST_INT8_LT(a)           ((a) <  (HOST_INT8_T)0)
   #define HOST_INT8_GT(a)           ((a) >  (HOST_INT8_T)0)
   #define HOST_INT8_EQ_LONG(a)      ((HOST_INT8_T)(a) == (long)(a))
   #define HOST_LLONG_EQ_LONG(a)     ((a) == (long)(a))
   #define HOST_INT8_TO_DOUBLE(a)    ((HOST_DOUBLE_T)(a))
   #define HOST_INT8_TO_LONG(a)      ((long)(a))
   #define HOST_INT8_TO_LLONG(a)     ((HOST_LLONG_T)(a))
   #define HOST_LONG_TO_INT8(a)      ((HOST_INT8_T)(a))
   #define HOST_LLONG_TO_INT8(a)     ((HOST_INT8_T)(a))
   #define HOST_INT8_INC(a, b)       ((a) += (HOST_INT8_T)(b))
   #define HOST_INT8_DEC(a, b)       ((a) -= (HOST_INT8_T)(b))
   #define HOST_INT8_MUL(a, b)       ((a) *= (HOST_INT8_T)(b))
   #define HOST_INT8_DIV(a, b)       ((a) /= (HOST_INT8_T)(b))
   #define HOST_INT8_MOD(a, b)       ((a) %= (HOST_INT8_T)(b))
   #define HOST_INT8_MOD_LONG(a, b)  ((a) % (HOST_LLONG_T)(b))
#endif /* #ifdef HOST_FAKE8 */

#ifdef HOST_NO_PROTO
   /* old style function declaration */

   HOST_INT6_T HostInt6Div();
   HOST_INT8_T HostDoubleToInt8();
   HOST_INT8_T HostLongToInt8();
   HOST_INT8_T HostLLongToInt8();
   HostInt8Inc();
   HostInt8Dec();
   HostInt8Mul();
   HostInt8Div();
   HostInt8Mod();
   int HostInt8EQ();
   int HostInt8NE();
   int HostInt8LT();
   int HostInt8GT();
   int HostInt8EQLong();
   long HostInt8ModLong();
   HOST_DOUBLE_T Int8ToHostDouble();

#else /* HOST_NO_PROTO */
   /* ANSI style function prototypes */

   HOST_INT6_T HostInt6Div(
      HOST_INT6_T a,
      HOST_INT6_T b)
   ;

   HOST_INT8_T HostDoubleToInt8(
      HOST_DOUBLE_T arg)
   ;
   HOST_INT8_T HostLongToInt8(
      long arg)
   ;
   HOST_INT8_T HostLLongToInt8(
      HOST_LLONG_T arg)
   ;
   void HostInt8Inc(
      HOST_INT8_T *a,
      HOST_INT8_T b)
   ;
   void HostInt8Dec(
      HOST_INT8_T *a,
      HOST_INT8_T b)
   ;
   void HostInt8Mul(
      HOST_INT8_T *a,
      HOST_INT8_T b)
   ;
   void HostInt8Div(
      HOST_INT8_T *a,
      HOST_INT8_T b)
   ;
   void HostInt8Mod(
      HOST_INT8_T *a,
      HOST_INT8_T b)
   ;
   int HostInt8EQ(
      HOST_INT8_T a)
   ;
   int HostInt8NE(
      HOST_INT8_T a)
   ;
   int HostInt8LT(
      HOST_INT8_T a)
   ;
   int HostInt8GT(
      HOST_INT8_T a)
   ;
   int HostInt8EQLong(
      HOST_INT8_T a)
   ;
   long HostInt8ModLong(
      HOST_INT8_T *a,
      long b)
   ;
   HOST_DOUBLE_T Int8ToHostDouble(
      HOST_INT8_T arg)
   ;

#endif /* HOST_NO_PROTO */

#endif /* __HOST_H */
