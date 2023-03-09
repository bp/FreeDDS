#ifndef __CDDS_H
#define __CDDS_H

/*
RCS-t=DDS, public "C" API to Data Dictionary System

$Id: cdds.h 48 2009-02-06 21:08:40Z user $
*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

/***************************************************************************/
/***  (Most) symbols visible to users, but not part of the application     */
/***  program interface, are prefixed by "DDS" or "dds", to avoid clashes. */
/***************************************************************************/

#include "chost.h"

#ifdef  __cplusplus
extern "C" {
#endif

/*********************************************************/
/******  The following are generic DDS limits.      ******/
/*********************************************************/

   /* maximum number of cube dimensions */
   #define RANK_MAX 9

   /* maximum number of characters in a DDS definition name */
   #define DEFNNAME_MAX 63
   #define DEFNNAME_MAXQ "63"

   /* maximum number of characters in a cube axis name */
   #define AXISNAME_MAX 31
   #define AXISNAME_MAXQ "31"

   /* maximum number of characters in a axis units name */
   #define UNITNAME_MAX 31
   #define UNITNAME_MAXQ "31"

/***********************************************************/
/***  Host dependent limits, provided as a convenience.  ***/
/***********************************************************/

   /* maximum number of characters in one component of a filename */
   #define BASENAME_MAX 255
   #define BASENAME_MAXQ "255"

   /* maximum number of characters in a fully qualified filename */
   #define PATHNAME_MAX 1023
   #define PATHNAME_MAXQ "1023"

/*******************************************************/
/***  Strings for cdds_scank and flag/switch usage.  ***/
/*******************************************************/

   #define DDS_TRUE_KEY "true TRUE True t T yes YES Yes y Y"

   #define DDS_FALSE_KEY "false FALSE False f F no NO No n N"

   #define DDS_HELP_KEY "help HELP Help -help -HELP -Help -h -H -?"

   #define DDS_HELP_ALIAS "help HELP Help -help -HELP -Help"

/*****************************************/
/***  Types intrinsicly known by DDS.  ***/
/*****************************************/

   /************************/
   /* host dependent types */
   /************************/
   #define DDS_VOID            1  /* zero precision */
   #define DDS_PTR             2
   #define DDS_CHAR            3  /* character */
   #define DDS_SCHAR           4
   #define DDS_SHORT           5  /* signed integer */
   #define DDS_INT             6
   #define DDS_LONG            7
   #define DDS_LLONG           8
   #define DDS_UCHAR           9
   #define DDS_USHORT         10  /* unsigned integer */
   #define DDS_UINT           11
   #define DDS_ULONG          12
   #define DDS_ULLONG         13
   #define DDS_FLOAT          14  /* floating point */
   #define DDS_DOUBLE         15
   #define DDS_LDOUBLE        16
   /*************************************/
   /* extension to host dependent types */
   /*************************************/
   #define DDS_CPX            17   /* struct{float real, imag;} */
   #define DDS_DBLCPX         18
   #define DDS_LDBLCPX        19
   /***************************************/
   /* DDS stream types (host independent) */
   /***************************************/
   #define DDS_ASCII1         20  /* character */
   #define DDS_EBCDIC1        21
   #define DDS_INTEGER1       22  /* signed integer */
   #define DDS_INTEGER2       23
   #define DDS_INTEGER2X      24
   #define DDS_INTEGER4       25
   #define DDS_INTEGER4X      26
   #define DDS_INTEGER8       27
   #define DDS_INTEGER8X      28
   #define DDS_UNSIGNED1      29  /* unsigned integer */
   #define DDS_UNSIGNED2      30
   #define DDS_UNSIGNED2X     31
   #define DDS_UNSIGNED4      32
   #define DDS_UNSIGNED4X     33
   #define DDS_UNSIGNED8      34
   #define DDS_UNSIGNED8X     35
   #define DDS_FLOAT4         36 /* float */
   #define DDS_FLOAT4X        37
   #define DDS_FLOAT4IBM      38
   #define DDS_FLOAT8         39
   #define DDS_FLOAT8X        40
   #define DDS_FLOAT8IBM      41
   #define DDS_FLOAT16        42
   #define DDS_FLOAT16X       43
   #define DDS_COMPLEX8       44 /* complex */
   #define DDS_COMPLEX8X      45
   #define DDS_COMPLEX8IBM    46
   #define DDS_COMPLEX16      47
   #define DDS_COMPLEX16X     48
   #define DDS_COMPLEX16IBM   49
   #define DDS_COMPLEX32      50
   #define DDS_COMPLEX32X     51

/*******************/
/***  API types  ***/
/*******************/

   typedef int BIN_TAG;

   typedef int SYM_TAG;

   typedef SYM_TAG FIELD_TAG;  /* keep for consistency with documentation */

   typedef enum DDS_DEBUG
   {  /* DDS application debug control (on, off, one-time) */
      /* Application users may set debug flags by defining */
      /* dds_debug, DDS_DEBUG, dds_dump, or DDS_DUMP. */
      /* Flags set by calling cdds_check are ORed with defn flags. */
      DDS_DBG_HEAP         = 1 <<  0,      /* check heap once, right now */
                                           /*    quick and cheap */
      DDS_DBG_ALLOC        = 1 <<  1,      /* check heap on each alloc */
                                           /*    real expensive */
      DDS_DBG_FREE         = 1 <<  2,      /* check heap on each free */
                                           /*    expensive */
      DDS_DBG_CALL         = 1 <<  3,      /* check heap on each DDS API call */
                                           /*    REALLY expensive */
      DDS_DBG_TRACE        = 1 <<  4,      /* trace all API calls */
                                           /*    REALLY verbose */
      DDS_DBG_SCANNER      = 1 <<  5,      /* trace compiler scanner */
                                           /*    for DDS gurus */
      DDS_DBG_SYMBOL       = 1 <<  6,      /* dump binary symbol tables */
                                           /*    for DDS gurus */
      DDS_DBG_UNCOMPILE    = 1 <<  7,      /* dump ddsUncompileFmt stacks */
                                           /*    for DDS gurus */
      DDS_DBG_BINARY       = 1 <<  8,      /* dump binary ? tables */
                                           /*    for DDS gurus */
      DDS_DBG_PANIC        = 1 << 29       /* dump binary tables on panic */
                                           /*    for DDS gurus */
   } DDS_DEBUG;


/*******************************/
/***  Function declaration.  ***/
/*******************************/

#if defined(FUNCPROTO) || __STDC__ || defined(__STDC__) || \
   __SUNPRO_C || defined(__cplusplus) || defined(c_plusplus)

/* ANSI style function prototypes */

double cdds_scale(
   const char *out_units,
   const char *in_units)
;
DDS_DEBUG cdds_check(
   DDS_DEBUG new_flag)
;

   int cdds_dict(
      const char *name,
      const char *mode)
   ;
   int cdds_copy(
      const char *alias)
   ;
   int cdds_history(void)
   ;
   int cdds_addaxis(
      const char *dict,
      int num,
      const char *name)
   ;
   int cdds_rmaxis(
      const char *dict,
      int num)
   ;
   const char * cdds_in(
      const char *in_alias,
      const char *in_value,
      const char *title)
   ;
   const char * cdds_out(
      const char *out_alias,
      const char *out_value,
      const char *history)
   ;
   const char * cdds_mod(
      const char *mod_alias,
      const char *mod_value,
      const char *title,
      const char *history)
   ;


   const char * cdds_newer(
      const char *alias)
   ;
   int cdds_scanv(
      const char *alias,
      const char **value)
   ;
   int cdds_scant(
      const char *alias,
      const char **value)
   ;
   int cdds_scantn(
      const char *alias,
      const char **value,
      int count)
   ;
   int cdds_scank(
      const char *alias,
      const char *keys)
   ;
   int cdds_scanf(
      const char *alias,
      const char *fmt,
      ...)
   ;
   int cdds_printv(
      const char *alias,
      const char *value)
   ;
   int cdds_printt(
      const char *alias,
      const char *value)
   ;
   int cdds_printf(
      const char *name,
      const char *fmt,
      ...)
   ;
   int cdds_key(
      const char *keys)
   ;

   const char * cdds_datadefn(
      const char *data_dict,
      const char *data_alias)
   ;

   BIN_TAG cdds_open(
      const char *open_dict,
      const char *open_format,
      const char *open_data,
      const char *open_mode)
   ;
   int cdds_openm(
      BIN_TAG out_bin_tag,
      SYM_TAG out_sym_tag,
      BIN_TAG in_bin_tag,
      SYM_TAG in_sym_tag)
   ;
   int cdds_close(
      BIN_TAG bin_tag)
   ;
   int cdds_binfd(
      BIN_TAG bin_tag)
   ;
   int cdds_read(
      BIN_TAG bin_tag,
      SYM_TAG sym_tag,
      void *buf,
      int count)
   ;
   int cdds_readm(
      BIN_TAG in_bin_tag,
      SYM_TAG in_sym_tag,
      BIN_TAG out_bin_tag,
      SYM_TAG out_sym_tag,
      void *out_buf,
      int count)
   ;
   int cdds_write(
      BIN_TAG bin_tag,
      SYM_TAG tag,
      const void *buf,
      int count)
   ;
   int cdds_writem(
      BIN_TAG out_bin_tag,
      SYM_TAG out_sym_tag,
      BIN_TAG in_bin_tag,
      SYM_TAG in_sym_tag,
      const void *in_buf,
      int count)
   ;
   HOST_INT6_T cdds_lseek8(
      BIN_TAG bin_tag,
      SYM_TAG tag,
      HOST_INT6_T offset,
      int whence)
   ;
   long cdds_lseek(
      BIN_TAG bin_tag,
      SYM_TAG tag,
      long position,
      int whence)
   ;
   int cdds_fflush(
      BIN_TAG bin_tag)
   ;
   int cdds_seek(
      BIN_TAG bin_tag,
      SYM_TAG tag,
      long offset,
      int whence)
   ;
   int cdds_setpos(
      BIN_TAG bin_tag,
      SYM_TAG tag,
      HOST_INT6_T *offset,
      int whence)
   ;
   int cdds_map(
      BIN_TAG out_bin_tag,
      SYM_TAG out_sym_tag,
      void *out_buf,
      BIN_TAG in_bin_tag,
      SYM_TAG in_sym_tag,
      const void *in_buf)
   ;

   int cdds_isreg(
	   BIN_TAG bin_tag)
   ;
   int cdds_axis_prod(
      int rank)
   ;

   int cdds_offset(
      BIN_TAG bin_tag,
      SYM_TAG sym_tag)
   ;
   int cdds_index(
      BIN_TAG bin_tag,
      SYM_TAG sym_tag,
      int prime)
   ;
   int cdds_count(
      BIN_TAG bin_tag,
      SYM_TAG sym_tag)
   ;
   const char * cdds_genus(
      BIN_TAG bin_tag,
      SYM_TAG sym_tag)
   ;
   int cdds_align(
      BIN_TAG bin_tag,
      SYM_TAG sym_tag)
   ;
   int cdds_prec(
      BIN_TAG bin_tag,
      SYM_TAG sym_tag)
   ;
   SYM_TAG cdds_member(
      BIN_TAG bin_tag,
      SYM_TAG sym_tag,
      const char *field_name)
   ;
   SYM_TAG cdds_object(
      BIN_TAG bin_tag,
      SYM_TAG sym_tag,
      const char *field_name)
   ;
   SYM_TAG cdds_next(
      BIN_TAG bin_tag,
      SYM_TAG sym_tag)
   ;
   SYM_TAG cdds_prev(
      BIN_TAG bin_tag,
      SYM_TAG sym_tag)
   ;
   SYM_TAG cdds_parent(
      BIN_TAG bin_tag,
      SYM_TAG sym_tag)
   ;
   SYM_TAG cdds_prime(
      BIN_TAG bin_tag,
      SYM_TAG sym_tag)
   ;
   const char * cdds_name(
      BIN_TAG bin_tag,
      SYM_TAG sym_tag)
   ;


   int cdds_get(
      BIN_TAG bin_tag,
      SYM_TAG sym_tag,
      const void *in_buf,
      void *out_array,
      int array_n)
   ;
   int cdds_getcs(
      BIN_TAG bin_tag,
      SYM_TAG sym_tag,
      const void *in_buf,
      char *out_array,
      int array_n)
   ;
   int cdds_getcb(
      BIN_TAG bin_tag,
      SYM_TAG sym_tag,
      const void *in_buf,
      char *out_array,
      int array_n)
   ;
   int cdds_geti(
      BIN_TAG bin_tag,
      SYM_TAG sym_tag,
      const void *in_buf,
      int *out_array,
      int array_n)
   ;
   int cdds_gets(
      BIN_TAG bin_tag,
      SYM_TAG sym_tag,
      const void *in_buf,
      short int *out_array,
      int array_n)
   ;
   int cdds_getl(
      BIN_TAG bin_tag,
      SYM_TAG sym_tag,
      const void *in_buf,
      long int *out_array,
      int array_n)
   ;
   int cdds_getu(
      BIN_TAG bin_tag,
      SYM_TAG sym_tag,
      const void *in_buf,
      unsigned *out_array,
      int array_n)
   ;
   int cdds_getus(
      BIN_TAG bin_tag,
      SYM_TAG sym_tag,
      const void *in_buf,
      short unsigned *out_array,
      int array_n)
   ;
   int cdds_getf(
      BIN_TAG bin_tag,
      SYM_TAG sym_tag,
      const void *in_buf,
      float *out_array,
      int array_n)
   ;
   int cdds_getff(
      BIN_TAG bin_tag,
      SYM_TAG sym_tag,
      const void *in_buf,
      float *out_array,
      int array_n)
   ;
   int cdds_getc(
      BIN_TAG bin_tag,
      SYM_TAG sym_tag,
      const void *in_buf,
      float *out_array,
      int array_n)
   ;
   int cdds_getd(
      BIN_TAG bin_tag,
      SYM_TAG sym_tag,
      const void *in_buf,
      double *out_array,
      int array_n)
   ;


   int cdds_put(
      BIN_TAG out_bin_tag,
      SYM_TAG sym_tag,
      void *out_buf,
      const void *in_array,
      int array_n)
   ;
   int cdds_putcs(
      BIN_TAG out_bin_tag,
      SYM_TAG sym_tag,
      void *out_buf,
      const char *in_array,
      int array_n)
   ;
   int cdds_putcb(
      BIN_TAG out_bin_tag,
      SYM_TAG sym_tag,
      void *out_buf,
      const char *in_array,
      int array_n)
   ;
   int cdds_puti(
      BIN_TAG out_bin_tag,
      SYM_TAG sym_tag,
      void *out_buf,
      const int *in_array,
      int array_n)
   ;
   int cdds_puts(
      BIN_TAG out_bin_tag,
      SYM_TAG sym_tag,
      void *out_buf,
      const short int *in_array,
      int array_n)
   ;
   int cdds_putl(
      BIN_TAG out_bin_tag,
      SYM_TAG sym_tag,
      void *out_buf,
      const long int *in_array,
      int array_n)
   ;
   int cdds_putu(
      BIN_TAG out_bin_tag,
      SYM_TAG sym_tag,
      void *out_buf,
      const unsigned *in_array,
      int array_n)
   ;
   int cdds_putus(
      BIN_TAG out_bin_tag,
      SYM_TAG sym_tag,
      void *out_buf,
      const short unsigned *in_array,
      int array_n)
   ;
   int cdds_putf(
      BIN_TAG out_bin_tag,
      SYM_TAG sym_tag,
      void *out_buf,
      const float *in_array,
      int array_n)
   ;
   int cdds_putff(
      BIN_TAG out_bin_tag,
      SYM_TAG sym_tag,
      void *out_buf,
      const float *in_array,
      int array_n)
   ;
   int cdds_putd(
      BIN_TAG out_bin_tag,
      SYM_TAG sym_tag,
      void *out_buf,
      const double *in_array,
      int array_n)
   ;


   void * cdds_malloc(
      size_t size)
   ;
   void * cdds_realloc(
      void * ptr,
      size_t size)
   ;
   void * cdds_calloc(
      size_t nobj,
      size_t size)
   ;
   void cdds_free(
      void * ptr)
   ;
   size_t cdds_memuse();

   int (cdds_init)(void);

   int cdds_iniopemp(void);

   int cdds_initmpi(
      int *node,
      int *nodes)
   ;

   int cdds_initmpix(
      int *node,
      int *nodes)
   ;

   char *cdds_toebcdic(
      char *dest,
      const char *source,
      int count)
   ;
   char *cdds_toascii(
      char *dest,
      const char *source,
      int count)
   ;
   
   int setargcv(int argc, char* argv[]);

   int cdds_openpr(
      const char *pname,
      const char *id)
   ;
   int cdds_savepr()
   ;
   int cdds_prthdr(
      const char *prg,  
      const char *id)
   ;
   int cdds_prtmsg(
      const char *fmt,
      ...)
   ;
   int cdds_prtcon(
      const char *fmt,
      ...)
   ;
   int cdds_prterr(
      const char *fmt,
      ...)
   ;
   int cdds_errors();
		
   int cdds_info(
      int enable)
   ;

   int cdds_closepr();

   int cdds_closepr2(
      int status)
   ;
   
   void Monitor();

   int cdds_switch(
      const char *swtch,
      const int	dflt)
   ;

   /* Convenience Routines */

   BIN_TAG cddx_in(
      const char *in_alias,
      const char *in_value,
      const char *title)
   ;
   BIN_TAG cddx_in2(
      const char *in_alias,
      const char *in_value,
      const char *title)
   ;
   BIN_TAG cddx_inhdr(
      const char *alias,
      const char *value,
      const char *title,
      int smplPrime,
      int hdrPrime,
      const char *localHdrs)
   ;
   int cddx_index(
      BIN_TAG bin_tag,
      const char *name,
      int prime)
   ;
   BIN_TAG cddx_out(
      const char *out_alias,
      const char *out_value,
      const char *title,
      BIN_TAG history_tag)
   ;
   BIN_TAG cddx_outhdr(
      const char *alias,
      const char *value,
      const char *title,
      BIN_TAG old_tag,
      int smplPrime,
      int hdrPrime,
      const char *localHdrs)
   ;
   BIN_TAG cddx_mod(
      const char *mod_alias,
      const char *mod_value,
      const char *title,
      BIN_TAG history_tag)
   ;
   BIN_TAG cddx_mod2(
      const char *mod_alias,
      const char *mod_value,
      const char *title,
      BIN_TAG history_tag)
   ;
   BIN_TAG cddx_getbin(
      BIN_TAG bin_tag)
   ;
   int cddx_dict(
      BIN_TAG bin_tag,
      const char *mode)
   ;
   const char * cddx_getdict(
      BIN_TAG bin_tag)
   ;
   int cddx_addaxis(
      BIN_TAG bin_tag,
      int num,
      const char *name)
   ;
   int cddx_rmaxis(
      BIN_TAG bin_tag,
      int num)
   ;
   int cddx_read(
      BIN_TAG bin_tag,
      void *buffer,
      int count)
   ;
   int cddx_readfrom(
      BIN_TAG bin_tag,
      long offset,
      void *buffer,
      int count)
   ;
   int cddx_readall(
      char *alias,
      void **buffer,
      int *rank,
      int *size,
      float *delta,
      float *origin)
   ;
   int cddx_write(
      BIN_TAG bin_tag,
      const void *buffer,
      int count)
   ;
   int cddx_writeto(
      BIN_TAG bin_tag,
      long offset,
      const void *buffer,
      int count)
   ;
   double cddx_wtime();


#else
   /* old style function declaration */

   int cdds_dict();
   int cdds_copy();
   int cdds_history();
   int cdds_addaxis();
   int cdds_rmaxis();
   char * cdds_in();
   char * cdds_out();
   char * cdds_mod();

   int cdds_newer() ;
   int cdds_scanv() ;
   int cdds_scant() ;
   int cdds_scantn() ;
   int cdds_scank() ;
   int cdds_scanf() ;
   int cdds_printv() ;
   int cdds_printt() ;
   int cdds_printf() ;
   int cdds_key();

   char * cdds_datadefn();

   BIN_TAG cdds_open();
   int cdds_openm();
   int cdds_close();
   int cdds_read();
   int cdds_readm();
   int cdds_write();
   int cdds_writem();
   long cdds_lseek();
   HOST_INT6_T cdds_lseek8();
   int cdds_fflush();
   int cdds_seek();
   int cdds_setpos();
   int cdds_map();
   int cdds_isreg();

   int cdds_axis_prod();

   int cdds_offset();
   int cdds_index();
   int cdds_count();
   char * cdds_genus();
   int cdds_align();
   int cdds_prec();
   SYM_TAG cdds_member();
   SYM_TAG cdds_next();
   SYM_TAG cdds_prev();
   SYM_TAG cdds_parent();
   SYM_TAG cdds_prime();

   char * cdds_name();

   int cdds_get();
   int cdds_getcs();
   int cdds_getcb();
   int cdds_geti();
   int cdds_gets();
   int cdds_getl();
   int cdds_getu();
   int cdds_getus();
   int cdds_getul();
   int cdds_getf();
   int cdds_getff();
   int cdds_getd();

   int cdds_put();
   int cdds_putcs();
   int cdds_putcb();
   int cdds_puti();
   int cdds_puts();
   int cdds_putl();
   int cdds_putu();
   int cdds_putus();
   int cdds_putul();
   int cdds_putf();
   int cdds_putff();
   int cdds_putd();

   char *cdds_malloc();
   char *cdds_realloc();
   char *cdds_calloc();
   void cdds_free();
   size_t cdds_memuse();

   char *cdds_toebcdic();
   char *cdds_toascii();

   int setargcv();

   int cdds_openpr();
   int cdds_savepr();
   int cdds_prthdr();
   int cdds_prtmsg();
   int cdds_prtcon();
   int cdds_prterr();
   int cdds_errors();
   int cdds_info();
   int cdds_closepr();

   int cdds_switch();

   /* Convenience Routines */

   BIN_TAG cddx_in();
   BIN_TAG cddx_in2();
   BIN_TAG cddx_inhdr();
   BIN_TAG cddx_index();
   BIN_TAG cddx_out();
   BIN_TAG cddx_outhdr();
   BIN_TAG cddx_mod();
   BIN_TAG cddx_mod2();
   BIN_TAG cddx_getbin();
   int cddx_dict();
   int cddx_getdict();
   int cddx_addaxis();
   int cddx_rmaxis();
   int cddx_read();
   int cddx_readfrom();
   int cddx_readall();
   int cddx_write();
   int cddx_writeto();
   double cddx_wtime();


#endif

#ifdef  __cplusplus
}
#endif

#endif /* #ifndef __CDDS_H */
