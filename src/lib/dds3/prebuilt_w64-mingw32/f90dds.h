! Host independent
! RCS-t=DDS, Fortran 90 Application Program Interface (visible to programmers).

! $Id  f90_com.h,v 3.24 2003/01/13 17 46 35 user Exp $

! *****************************************
! ***   Copyright (C) 2005              ***
! ***   BP America, Inc, Houston, TX    ***
! ***   Jerry Ehlers, June 2005         ***
! *****************************************

      integer, parameter :: INT8=selected_int_kind(18)
      integer, parameter :: INT4=selected_int_kind(9)
      integer, parameter :: REAL8=selected_real_kind(15,300)

      integer, parameter :: &
         SIZEOF_INTEGER=4, SIZEOF_INTEGER2=2, SIZEOF_INTEGER8=8, &
         SIZEOF_REAL=4, SIZEOF_DOUBLE=8, &
         SIZEOF_COMPLEX=8, SIZEOF_DBL_CPX=16

      integer, parameter :: &
         BASENAME_MAX=255, PATHNAME_MAX=1023

      character (len=*), parameter :: &
         DEFNNAME_MAXQ='63', AXISNAME_MAXQ='31', UNITNAME_MAXQ='31', &
         BASENAME_MAXQ='255', PATHNAME_MAXQ='1023'

      character (len=*), parameter :: &
         DDS_TRUE_KEY  ='true TRUE True t T yes YES Yes y Y', &
         DDS_FALSE_KEY ='false FALSE False f F no NO No n N', &
         DDS_HELP_ALIAS='help HELP Help -help -HELP -Help',  &
         DDS_HELP_KEY  ='help HELP Help -help -HELP -Help -h -H -?'

      integer, parameter :: &
         DDS_CHARACTER=3, DDS_LOGICAL=6, DDS_LOGICAL1=3, &
         DDS_INTEGER=6, DDS_INTEGER2=5, DDS_REAL=14, DDS_DOUBLE=15, &
         DDS_COMPLEX=17, DDS_DBL_CPX=18

      integer, parameter :: &
         RANK_MAX=9, DEFNNAME_MAX=63, AXISNAME_MAX=31, &
         UNITNAME_MAX=31, SEEK_SET=0, SEEK_CUR=1, SEEK_END=2, &
         DDS_STREAM=-DDS_LOGICAL1, DDS_STREAM2=-DDS_INTEGER2, &
         DDS_STREAM4=-DDS_INTEGER

      integer, parameter :: &
         DDS_DBG_HEAP=1, DDS_DBG_ALLOC=2, DDS_DBG_FREE=4, DDS_DBG_CALL=8, &
	 DDS_DBG_TRACE=16, DDS_DBG_SCANNER=32, DDS_DBG_SYMBOL=64, &
	 DDS_DBG_UNCOMPILE=128, DDS_DBG_BINARY=256, DDS_DBG_PANIC=536870912

      integer, external :: &
         fdds_init, fdds_dict, fdds_copy, fdds_history, &
         fdds_addaxis, fdds_rmaxis, fdds_in, fdds_out, fdds_mod, &
         fdds_scanv, fdds_scant, fdds_scantn, fdds_scank, fdds_scanf, &
         fdds_sscanf, fdds_key, fdds_printv, fdds_printt, fdds_printf, &
         fdds_open, fdds_close, fdds_read, fdds_readm, &
         fdds_write, fdds_writem, fdds_lseek, fdds_fflush, fdds_map, &
         fdds_axis_prod, fdds_print

      integer, external :: &
         fdds_free, fdds_check

      real (KIND=REAL8) :: &
         fdds_memuse, fdds_scale

      integer, external :: &
         fdds_offset, fdds_index, fdds_count, &
         fdds_genus, fdds_align, fdds_prec, fdds_name, &
         fdds_get, fdds_getcb, fdds_geti, &
         fdds_geti2, fdds_getr, fdds_getff, fdds_getc, fdds_getd, &
         fdds_put, fdds_putcb, fdds_puti, &
         fdds_puti2, fdds_putr, fdds_putff, fdds_putc, fdds_putd, &
         fdds_member, fdds_object, fdds_next, &
         fdds_prev, fdds_parent, fdds_prime

      integer, external :: &
         fdds_openpr, fdds_savepr, fdds_prthdr, fdds_sprintf, &
         fdds_prtmsg, fdds_prtcon, fdds_prterr, fdds_errors, &
         fdds_info, fdds_closepr, fdds_closepr2, fdds_switch, fdds_isreg, &
         fdds_newer, fdds_openm

!     convenience routines
      integer, external :: &
         fddx_in, fddx_in2, fddx_inhdr, fddx_index, &
         fddx_out, fddx_outhdr, fddx_mod, fddx_mod2, &
         fddx_getbin, fddx_dict, fddx_getdict, fddx_addaxis, &
         fddx_rmaxis, fddx_read, fddx_readfrom, fddx_readall, &
         fddx_write, fddx_writeto

      real*8, external :: &
         fddx_wtime

      integer, external :: &
         fdds_datadefn


! The following added automatically by 'makefdds'

      integer (kind=INT8), external :: &
         fdds_lseek8

      integer (kind=INT8), external :: &
         fdds_malloc, fdds_realloc, fdds_calloc, fdds_ptr, &
         fdds_malloc8, fdds_realloc8, fdds_calloc8

