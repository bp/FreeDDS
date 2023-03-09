c Host independent
c RCS-t=DDS, Fortran Application Program Interface (visible to programmers).

c $Id: f_com.h 309 2018-12-18 22:15:38Z user $

c **********************************************
c ***   Copyright (C) 2005                   ***
c ***   BP America, Inc, Houston, TX         ***
c ***   Jerry Ehlers, June 2005              ***
c **********************************************

      integer SIZEOF_INTEGER, SIZEOF_INTEGER2, SIZEOF_INTEGER8,
     *   SIZEOF_REAL, SIZEOF_DOUBLE,
     *   SIZEOF_COMPLEX, SIZEOF_DBL_CPX
      parameter (SIZEOF_INTEGER=4, SIZEOF_INTEGER2=2, SIZEOF_INTEGER8=8,
     *   SIZEOF_REAL=4, SIZEOF_DOUBLE=8,
     *   SIZEOF_COMPLEX=8, SIZEOF_DBL_CPX=16)

      integer BASENAME_MAX, PATHNAME_MAX
      parameter (BASENAME_MAX=255, PATHNAME_MAX=1023)

      character DEFNNAME_MAXQ*2,
     *   AXISNAME_MAXQ*2, UNITNAME_MAXQ*2,
     *   BASENAME_MAXQ*3, PATHNAME_MAXQ*4
      parameter (DEFNNAME_MAXQ='63',
     *   AXISNAME_MAXQ='31', UNITNAME_MAXQ='31',
     *   BASENAME_MAXQ='255', PATHNAME_MAXQ='1023')

      character DDS_TRUE_KEY*48, DDS_FALSE_KEY*48,
     *   DDS_HELP_KEY*48, DDS_HELP_ALIAS*48
      parameter (
     *   DDS_TRUE_KEY  ='true TRUE True t T yes YES Yes y Y',
     *   DDS_FALSE_KEY ='false FALSE False f F no NO No n N',
     *   DDS_HELP_KEY  ='help HELP Help -help -HELP -Help -h -H -?',
     *   DDS_HELP_ALIAS='help HELP Help -help -HELP -Help')

      integer DDS_CHARACTER,
     *   DDS_LOGICAL, DDS_LOGICAL1,
     *   DDS_INTEGER, DDS_INTEGER2,
     *   DDS_REAL, DDS_DOUBLE,
     *   DDS_COMPLEX, DDS_DBL_CPX
      parameter (DDS_CHARACTER=3,
     *   DDS_LOGICAL=6, DDS_LOGICAL1=3,
     *   DDS_INTEGER=6, DDS_INTEGER2=5,
     *   DDS_REAL=14, DDS_DOUBLE=15,
     *   DDS_COMPLEX=17, DDS_DBL_CPX=18)

      integer RANK_MAX, DEFNNAME_MAX, AXISNAME_MAX, UNITNAME_MAX,
     *   SEEK_SET, SEEK_CUR, SEEK_END,
     *   DDS_STREAM, DDS_STREAM2, DDS_STREAM4
      parameter (RANK_MAX=9,
     *   DEFNNAME_MAX=63, AXISNAME_MAX=31, UNITNAME_MAX=31,
     *   SEEK_SET=0, SEEK_CUR=1, SEEK_END=2, DDS_STREAM=-DDS_LOGICAL1,
     *   DDS_STREAM2=-DDS_INTEGER2, DDS_STREAM4=-DDS_INTEGER)
 
      integer DDS_DBG_HEAP, DDS_DBG_ALLOC, DDS_DBG_FREE, 
     *   DDS_DBG_CALL, DDS_DBG_TRACE, DDS_DBG_SCANNER, DDS_DBG_SYMBOL,
     *   DDS_DBG_UNCOMPILE, DDS_DBG_BINARY, DDS_DBG_PANIC
      parameter (DDS_DBG_HEAP=1, DDS_DBG_ALLOC=2, DDS_DBG_FREE=4,       
     *   DDS_DBG_CALL=8, DDS_DBG_TRACE=16, DDS_DBG_SCANNER=32, 
     *   DDS_DBG_SYMBOL=64, DDS_DBG_UNCOMPILE=128, DDS_DBG_BINARY=256,
     *   DDS_DBG_PANIC=536870912)
 
      integer
     *   fdds_init, fdds_dict, fdds_copy, fdds_history,
     *   fdds_addaxis, fdds_rmaxis, fdds_in, fdds_out, fdds_mod,
     *   fdds_scanv, fdds_scant, fdds_scantn, fdds_scank, fdds_scanf,
     *   fdds_sscanf, fdds_key, fdds_printv, fdds_printt, fdds_printf,
     *   fdds_open, fdds_close, fdds_read, fdds_readm,
     *   fdds_write, fdds_writem, fdds_lseek, fdds_fflush, fdds_map, 
     *   fdds_axis_prod, fdds_print
      integer
     *   fdds_free, fdds_check
      real*8 
     *   fdds_memuse, fdds_scale
      integer
     *   fdds_offset, fdds_index, fdds_count,
     *   fdds_genus, fdds_align, fdds_prec, fdds_name,
     *   fdds_get, fdds_getcb, fdds_geti,
     *   fdds_geti2, fdds_getr, fdds_getff, fdds_getc, fdds_getd,
     *   fdds_put, fdds_putcb, fdds_puti,
     *   fdds_puti2, fdds_putr, fdds_putff, fdds_putc, fdds_putd,
     *   fdds_member, fdds_object, fdds_next, 
     *   fdds_prev, fdds_parent, fdds_prime
      integer
     *   fdds_openpr, fdds_savepr, fdds_prthdr, fdds_sprintf, 
     *   fdds_prtmsg, fdds_prtcon, fdds_prterr, fdds_errors, fdds_info, 
     *   fdds_closepr, fdds_closepr2, fdds_switch, fdds_isreg, 
     *   fdds_newer, fdds_openm
      external
     *   fdds_init, fdds_dict, fdds_copy, fdds_history,
     *   fdds_addaxis, fdds_rmaxis, fdds_in, fdds_out, fdds_mod,
     *   fdds_scanv, fdds_scant, fdds_scantn, fdds_scank, fdds_scanf,
     *   fdds_sscanf, fdds_key, fdds_printv, fdds_printt, fdds_printf,
     *   fdds_open, fdds_close, fdds_read, fdds_readm,
     *   fdds_write, fdds_writem, fdds_lseek, fdds_fflush, fdds_map, 
     *   fdds_axis_prod, fdds_print
      external
     *   fdds_free, fdds_check
      external
     *   fdds_memuse, fdds_scale
      external
     *   fdds_offset, fdds_index, fdds_count,
     *   fdds_genus, fdds_align, fdds_prec, fdds_name,
     *   fdds_get, fdds_getcb, fdds_geti,
     *   fdds_geti2, fdds_getr, fdds_getff, fdds_getc, fdds_getd,
     *   fdds_put, fdds_putcb, fdds_puti,
     *   fdds_puti2, fdds_putr, fdds_putff, fdds_putc, fdds_putd,
     *   fdds_member, fdds_object, fdds_next, 
     *   fdds_prev, fdds_parent, fdds_prime
      external
     *   fdds_openpr, fdds_savepr, fdds_prthdr, fdds_sprintf, 
     *   fdds_prtmsg, fdds_prtcon, fdds_prterr, fdds_errors, fdds_info, 
     *   fdds_closepr, fdds_closepr2, fdds_switch, fdds_isreg,
     *   fdds_newer, fdds_openm

c
c     convenience routines
c
      integer
     *   fddx_in, fddx_in2, fddx_inhdr, fddx_index,
     *   fddx_out, fddx_outhdr, fddx_mod, fddx_mod2,  
     *   fddx_getbin, fddx_dict, fddx_getdict, fddx_addaxis, 
     *   fddx_rmaxis,fddx_read, fddx_readfrom, fddx_readall, 
     *   fddx_write, fddx_writeto
      external
     *   fddx_in, fddx_in2, fddx_inhdr, fddx_index,
     *   fddx_out, fddx_outhdr, fddx_mod, fddx_mod2, 
     *   fddx_getbin, fddx_dict, fddx_getdict, fddx_addaxis, 
     *   fddx_rmaxis, fddx_read, fddx_readfrom, fddx_readall, 
     *   fddx_write, fddx_writeto

      real*8   fddx_wtime
      external fddx_wtime

      integer  fdds_datadefn
      external fdds_datadefn

