!
!     !!! Beware !!!
!       Not all sample types are supported for read/write routines.
!       Some interfaces support more types than others.
!       Read/write interfaces accomodate 1D I/O buffers only.
!     !!! Beware !!!
!
!     Interface module containing strongly-typed declarations for
!     many DDS library routines
!     (optionally 'use dds' instead of "f90dds.h" and "fdds.h" header files)
!
!     Ver 0.1 June 2009 by Musa Maharramov, BP High Performance Computing
!
!     Ver 0.2 June 2009 by Musa Maharramov, BP High Performance Computing
!     -- fdds_printf2, fdds_scanf2 implemented as C functions
!     -- DDS_ADDR includes string length
!
!     ver 0.3 July 2009 by Musa Maharramov, BP High Performance Computing
!     -- new fdds_printf2, fdds_scanf2 based on old fdds_printf/scanf
!     -- DDS_ADDR type definitions changed to store printf-style format
!        character's ASCII code in DDS_ADDR%t and variable size in bytes in
!        DDS_ADDR%l (DDS_ADDR%t is still 's' for Fortran strings and
!        DDS_ADDR%l is Fortran string length or Fortran character array
!        length)
!
!     ver 0.4 August 2009  by Musa Maharramov, BP High Performance Computing
!     -- changed file extensions to F90
!     -- fdds_free made external symbol
!     -- corrected type errors in fdds_get(put)d_(arr)/var
!     -- implemented fdds_sprintf2/prtmsg2/con2/err2/ similar to fdds_printf2
!	-- explicit intent for all arguments
!     -- dropped adrdd() -- quad precision reals are not used by formatted i/o
!
!     ver 0.5 July 2015  by Michael Franco, BP High Performance Computing
!     -- added missing FFT functions
!     -- modified incorrect FFT functions
!     -- defined FFT parameters FFT_ESTIMATE and FFT_MEASURE

module DDS
  implicit none

  public

  integer,parameter :: DDS_SSI=selected_int_kind(2)       ! Short Short Int
  integer,parameter :: DDS_SHI=selected_int_kind(4)       ! SHort Int
  integer,parameter :: DDS_SPI=selected_int_kind(9)       ! Single Precision Int
  integer,parameter :: DDS_DPI=selected_int_kind(18)      ! Double Precision Int
  integer,parameter :: DDS_SPR=selected_real_kind(6,37)   ! Single Precision Real
  integer,parameter :: DDS_DPR=selected_real_kind(15,307) ! Double Precision Real
  private :: DDS_SSI, DDS_SHI, DDS_SPI, DDS_DPI, DDS_SPR, DDS_DPR

  ! ifort and gfortran? also have
  integer,parameter :: DDS_QPR=selected_real_kind(33,4931) ! Quad Precision Real
  private :: DDS_QPR

  !     These parameter definitions may be placed into an
  !     include file generated at DDS library compile time
  !********************************************************************
  !     basic variable type sizes
  integer, parameter :: &
       SIZEOF_INTEGER=4, SIZEOF_INTEGER2=2, SIZEOF_INTEGER8=8, &
       SIZEOF_REAL=4, SIZEOF_DOUBLE=8, &
       SIZEOF_COMPLEX=2*SIZEOF_REAL, SIZEOF_DBL_CPX=2*SIZEOF_COMPLEX

  !     DDS data types and limits
  integer, parameter :: &
       DDS_CHARACTER=3, DDS_LOGICAL=6, DDS_LOGICAL1=3, &
       DDS_INTEGER=6, DDS_INTEGER2=5, DDS_REAL=14, DDS_DOUBLE=15, &
       DDS_COMPLEX=17, DDS_DBL_CPX=18

  integer,parameter :: BASENAME_MAX=255, PATHNAME_MAX=1023

  character(len=*),parameter :: DEFNNAME_MAXQ='63', &
       AXISNAME_MAXQ='31', UNITNAME_MAXQ='31', &
       BASENAME_MAXQ='255', PATHNAME_MAXQ='1023'

  character(len=*),parameter :: &
       DDS_TRUE_KEY  ='true TRUE True t T yes YES Yes y Y', &
       DDS_FALSE_KEY ='false FALSE False f F no NO No n N', &
       DDS_HELP_KEY  ='help HELP Help -help -HELP -Help -h -H -?', &
       DDS_HELP_ALIAS='help HELP Help -help -HELP -Help'

  integer, parameter :: RANK_MAX=9, &
       DEFNNAME_MAX=63, AXISNAME_MAX=31, UNITNAME_MAX=31, &
       SEEK_SET=0, SEEK_CUR=1, SEEK_END=2, DDS_STREAM=-DDS_LOGICAL1, &
       DDS_STREAM2=-DDS_INTEGER2, DDS_STREAM4=-DDS_INTEGER

  ! DDS FFT data types
  integer, parameter :: FFT_ESTIMATE = 1
  integer, parameter :: FFT_MEASURE = 2

  !  Integer equivalent to pointer data type on various
  !  platforms (IA64,x86,i386 -- the rest assumed to use 4-byte pointers)
!!!
!!!  Commented this. Appears to be always (KIND=DDS_DPI)
!!!  Might be better to make these type(c_ptr) and 'use iso_c_binding'
!!!
!!!#ifdef __ia64__
!!!#define PTR_TYPE INTEGER(KIND=DDS_DPI)
!!!#elif defined(__x86_64__)
!!!#define PTR_TYPE INTEGER(KIND=DDS_DPI)
!!!#elif defined(__i386__)
!!!#define PTR_TYPE INTEGER(KIND=DDS_SPI)
!!!#else
!!!#define PTR_TYPE INTEGER(KIND=DDS_SPI)
!!!#endif
  !  Alternatively, the above may be computed at the build time
  !  and placed into a platform-dependent include file
  !********************************************************************

  !     other host-dependent limits should be included
  !     (e.g. in this module) in a file generated during the
  !     DDS library make (similar to f90dds.h but excluding
  !     any "external" symbols)

  !     externally-defined DDS procedures (declared
  !     either in an "interface" or using "external" attribute)

  interface
     !     Print Control
     !       open print file
     integer function fdds_openpr(pname, id)
       character(LEN=*), intent(in) :: pname
       character(LEN=*), intent(in) :: id
     end function fdds_openpr

     !       redirect stdout & stderr to the printout file
     integer function fdds_savepr()
     end function fdds_savepr

     !       write print header
     integer function fdds_prthdr(pname, id)
       character(LEN=*), intent(in) :: pname
       character(LEN=*), intent(in) :: id
     end function fdds_prthdr

     !       control writing to a print file & console
     integer function fdds_info(enable)
       integer, intent(in) :: enable
     end function fdds_info

     !       query errors
     integer function fdds_errors()
     end function fdds_errors

     !       close a print file
     integer function fdds_closepr()
     end function fdds_closepr

     integer function fdds_closepr2(istat)
       integer, intent(in) :: istat
     end function fdds_closepr2

     !     Dictionary Routines
     !       initialize DDS dictionary for input
     integer function fdds_in(dict, alias, value, title)
       character(LEN=*), intent(out) :: dict
       character(LEN=*), intent(in) :: alias
       character(LEN=*), intent(in) :: value
       character(LEN=*), intent(in) :: title
     end function fdds_in

     !       initialize DDS dictionary for output
     integer function fdds_out(dict, alias, value, hist)
       character(LEN=*), intent(out) :: dict
       character(LEN=*), intent(in) :: alias
       character(LEN=*), intent(in) :: value
       character(LEN=*), intent(in) :: hist
     end function fdds_out

     !       initialize DDS dictionary for modification
     integer function fdds_mod(dict, alias, value, title, hist)
       character(LEN=*), intent(out) :: dict
       character(LEN=*), intent(in) :: alias
       character(LEN=*), intent(in) :: value
       character(LEN=*), intent(in) :: title
       character(LEN=*), intent(in) :: hist
     end function fdds_mod

     !       change DDS dictionary access
     integer function fdds_dict(names, mode)
       character(LEN=*), intent(in) :: names
       character(LEN=*), intent(in) :: mode
     end function fdds_dict

     !       query axis product
     integer function fdds_axis_prod(naxis)
       integer, intent(in) :: naxis
     end function fdds_axis_prod

     !       append definition to DDS dictionary
     integer function fdds_printt(alias, str)
       character(LEN=*), intent(in) :: alias
       character(LEN=*), intent(in) :: str
     end function fdds_printt

     !       append definition to DDS dictionary
     integer function fdds_printv(alias, str)
       character(LEN=*), intent(in) :: alias
       character(LEN=*), intent(in) :: str
     end function fdds_printv

     !       retrieve definition information from dictionary
     integer function fdds_scank(alias, keys)
       character(LEN=*), intent(in) :: alias
       character(LEN=*), intent(in) :: keys
     end function fdds_scank

     !       retrieve definition from DDS dictionary
     integer function fdds_scant(alias, str)
       character(LEN=*), intent(in) :: alias
       character(LEN=*), intent(out) :: str
     end function fdds_scant

     !       retrieve definition from DDS dictionary
     integer function fdds_scantn(alias, str, token)
       character(LEN=*), intent(in) :: alias
       character(LEN=*), intent(out) :: str
       integer, intent(in) :: token
     end function fdds_scantn

     !       retrieve definition from DDS dictionary
     integer function fdds_scanv(alias, str)
       character(LEN=*), intent(in) :: alias
       character(LEN=*), intent(out) :: str
     end function fdds_scanv

     !       convenience routine to return the logical value of a user switch
     integer function fdds_switch(switch, default)
       character(LEN=*), intent(in) :: switch
       integer, intent(in) :: default
     end function fdds_switch

     !     Binary Field Access
     !       fetch a field tag within a DDS structure
     integer function fdds_member(bin_tag, field_tag, field_name)
       integer, intent(in) :: bin_tag, field_tag
       character(len=*), intent(in) :: field_name
     end function fdds_member

     !       fetch field name
     integer function fdds_name(field_name, bin_tag, field_tag)
       character(len=*), intent(out) :: field_name
       integer, intent(in) :: bin_tag, field_tag
     end function fdds_name

     !       fetch field precision
     integer function fdds_prec(bin_tag, field_tag)
       integer, intent(in) :: bin_tag, field_tag
     end function fdds_prec

     !       fetch field genus type
     integer function fdds_genus(field_genus, bin_tag, field_tag)
       character(len=*), intent(out) :: field_genus
       integer, intent(in) :: bin_tag, field_tag
     end function fdds_genus

     !       fetch field prime type tag
     integer function fdds_prime(bin_tag, field_tag)
       integer, intent(in) :: bin_tag, field_tag
     end function fdds_prime

     !       fetch a field tag within a DDS structure
     integer function fdds_next(bin_tag, field_tag)
       integer, intent(in) :: bin_tag, field_tag
     end function fdds_next

     !       find a newer definition, given an alias list
     integer function fdds_newer(name, alias)
       character(LEN=*), intent(out) :: name
       character(LEN=*), intent(in) :: alias
     end function fdds_newer

     !       field index within DDS type
     integer function fdds_index(bin_tag, field_tag, prime)
       integer, intent(in) :: bin_tag, field_tag, prime
     end function fdds_index

     !       open a DDS binary stream
     integer function fdds_open(dict, format_alias, data_alias, mode)
       character(LEN=*), intent(in) :: dict
       character(LEN=*), intent(in) :: format_alias
       character(LEN=*), intent(in) :: data_alias
       character(LEN=*), intent(in) :: mode
     end function fdds_open

     !       open mapping to output binary data from input
     integer function fdds_openm &
          (out_bin, out_field, in_bin, in_field)
       integer, intent(in) :: out_bin
       integer, intent(in) :: out_field
       integer, intent(in) :: in_bin
       integer, intent(in) :: in_field
     end function fdds_openm

     !       is DDS binary data a regular file
     integer function fdds_isreg(bin)
       integer, intent(in) :: bin
     end function fdds_isreg

     !       flush DDS binary stream
     integer function fdds_fflush(bin)
       integer, intent(in) :: bin
     end function fdds_fflush

     !       close binary stream
     integer function fdds_close(bin)
       integer, intent(in) :: bin
     end function fdds_close

     !       add a new axis to a DDS dictionary
     integer function fdds_addaxis(dict, num, name)
       character(LEN=*), intent(in) :: dict
       integer, intent(in) :: num
       character(LEN=*), intent(in) :: name
     end function fdds_addaxis

     !       remove an existing axis from a DDS dictionary
     integer function fdds_rmaxis(dict, num)
       character(LEN=*), intent(in) :: dict
       integer, intent(in) :: num
     end function fdds_rmaxis

     !       fetch field alignment
     integer function fdds_align(bin_tag, field_tag)
       integer, intent(in) :: bin_tag, field_tag
     end function fdds_align

     !       copy one DDS definition
     integer function fdds_copy(alias)
       character(LEN=*), intent(in) :: alias
     end function fdds_copy

     !       fetch field count
     integer function fdds_count(bin_tag, field_tag)
       integer, intent(in) :: bin_tag, field_tag
     end function fdds_count

     !       create filename for binary output
     integer function fdds_datadefn(name, data_dict, data_alias)
       character(LEN=*), intent(out) :: name
       character(LEN=*), intent(in) :: data_dict
       character(LEN=*), intent(in) :: data_alias
     end function fdds_datadefn

     !       copy all DDS definitions
     integer function fdds_history()
     end function fdds_history

     !       DDS definition value search
     integer function fdds_key(alias)
       character(LEN=*), intent(in) :: alias
     end function fdds_key

     !       seek position within DDS binary stream
     integer function fdds_lseek(bin, tag, offset, whence)
       integer, intent(in) :: bin, tag, offset, whence
     end function fdds_lseek

     !       seek position (8-byte precision) within DDS binary stream
     integer(KIND=DDS_DPI) function fdds_lseek8(bin, tag, offset, whence)
       import :: DDS_DPI
       integer, intent(in) :: bin, tag, whence
       integer(KIND=DDS_DPI), intent(in) :: offset
     end function fdds_lseek8

     !       fetch a field tag within a DDS block
     integer function fdds_object(bin_tag, field_tag, field_name)
       integer, intent(in) :: bin_tag, field_tag
       character(LEN=*), intent(in) :: field_name
     end function fdds_object

     !       field offset within DDS type
     integer function fdds_offset(bin_tag, field_tag)
       integer, intent(in) :: bin_tag, field_tag
     end function fdds_offset

     !       fetch a field tag within a DDS structure
     integer function fdds_parent(bin_tag, field_tag)
       integer, intent(in) ::  bin_tag, field_tag
     end function fdds_parent

     !       fetch previous field tag within a DDS structure
     integer function fdds_prev(bin_tag, field_tag)
       integer, intent(in) ::  bin_tag, field_tag
     end function fdds_prev

     !     DDS memory allocation functions
     integer(KIND=DDS_DPI) function fdds_malloc(bytes)
       import :: DDS_DPI
       integer, intent(in) :: bytes
     end function fdds_malloc

     integer(KIND=DDS_DPI) function fdds_malloc8(bytes)
       import :: DDS_DPI,DDS_DPR
       real(KIND=DDS_DPR), intent(in) :: bytes
     end function fdds_malloc8

     integer(KIND=DDS_DPI) function fdds_calloc(nobj, bytes)
       import :: DDS_DPI
       integer, intent(in) :: nobj, bytes
     end function fdds_calloc

     integer(KIND=DDS_DPI) function fdds_calloc8(nobj, bytes)
       import :: DDS_DPI,DDS_DPR
       integer, intent(in) :: nobj
       real(KIND=DDS_DPR), intent(in) :: bytes
     end function fdds_calloc8

     integer(KIND=DDS_DPI) function fdds_realloc(ptr, bytes)
       import :: DDS_DPI
       integer(KIND=DDS_DPI), intent(in) :: ptr
       integer, intent(in) :: bytes
     end function fdds_realloc

     integer(KIND=DDS_DPI) function fdds_realloc8(ptr, bytes)
       import :: DDS_DPI,DDS_DPR
       integer(KIND=DDS_DPI), intent(in) :: ptr
       real(KIND=DDS_DPR), intent(in) :: bytes
     end function fdds_realloc8

     real(KIND=DDS_DPR) function fdds_memuse()
       import :: DDS_DPR
     end function fdds_memuse

     !       check memory heap
     integer function fdds_check(flags)
       integer, intent(in) :: flags
     end function fdds_check

     !     DDS FFT functions
     ! return mixed radix 2,3,5 size
     integer function fft_nrfft5(n)
       integer, intent(in) :: n
     end function fft_nrfft5

     ! return odd mixed radix 3,5 sizes
     integer function fft_nrfft5odd(n)
       integer, intent(in) :: n
     end function fft_nrfft5odd

     ! multiple complex-complex 1D FFTs
     integer function fft_ccfftm(sgn,scl,n1,m,ld1,cdata,iopt)
       integer,intent(in) :: sgn, n1, m, ld1
       integer,intent(in) :: iopt
       real,intent(in) :: scl
       complex, dimension(1:ld1,1:m),intent(inout) :: cdata
     end function fft_ccfftm

     ! complex-complex 2D FFT
     integer function fft_ccfft2d(sgn, scl, n1, n2, ld1, cdata, iopt)
       integer,intent(in) :: sgn, n1, n2, ld1
       integer,intent(in) :: iopt
       real,intent(in) :: scl
       complex, dimension(1:ld1,1:n2),intent(inout) :: cdata
     end function fft_ccfft2d

     ! complex-complex 3D FFT
     integer function fft_ccfft3d(sgn, scl, n1, n2, n3, ld1, ld2, cdata, iopt)
       integer,intent(in) :: sgn, n1, n2, n3, ld1, ld2
       integer,intent(in) :: iopt
       real,intent(in) :: scl
       complex, dimension(1:ld1,1:ld2,1:n3),intent(inout) :: cdata
     end function fft_ccfft3d

     ! multiple complex-real 1D FFTs
     integer function fft_crfftm(scl, n1, m, ld1, cdata, iopt)
       integer,intent(in) :: n1, m, ld1
       integer,intent(in) :: iopt
       real,intent(in) :: scl
       real, dimension(1:ld1,1:m),intent(inout) :: cdata
     end function fft_crfftm

     ! complex-real 2D FFT
     integer function fft_crfft2d(scl, n1, n2, ld1, cdata, iopt)
       integer, intent(in) :: n1, n2, ld1
       integer,intent(in) :: iopt
       real, intent(in) :: scl
       real, dimension(1:ld1,1:n2), intent(inout) :: cdata
     end function fft_crfft2d

     ! complex-real 3D FFT
     integer function fft_crfft3d(scl, n1, n2, n3, ld1, ld2, cdata, iopt)
       integer, intent(in) :: n1, n2, n3, ld1, ld2
       integer,intent(in) :: iopt
       real, intent(in) :: scl
       real, dimension(1:ld1,1:ld2,1:n3), intent(inout) :: cdata
     end function fft_crfft3d

     ! multiple real-complex 1D FFTs
     integer function fft_rcfftm(scl, n1, m, ld1, cdata, iopt)
       integer,intent(in) :: n1, m, ld1
       integer,intent(in) :: iopt
       real,intent(in) :: scl
       real, dimension(1:ld1,1:m),intent(inout) :: cdata
     end function fft_rcfftm

     ! real-complex 2D FFT
     integer function fft_rcfft2d(scl, n1, n2, ld1, cdata, iopt)
       integer, intent(in) :: n1, n2, ld1
       integer,intent(in) :: iopt
       real, intent(in) :: scl
       real, dimension(1:ld1,1:n2), intent(inout) :: cdata
     end function fft_rcfft2d

     ! real-complex 3D FFT
     integer function fft_rcfft3d(scl, n1, n2, n3, ld1, ld2, cdata, iopt)
       integer, intent(in) :: n1, n2, n3, ld1, ld2
       integer,intent(in) :: iopt
       real, intent(in) :: scl
       real, dimension(1:ld1,1:ld2,1:n3), intent(inout) :: cdata
     end function fft_rcfft3d

     ! multiple double complex-complex 1D FFTs
     integer function fft_zzfftm(sgn, scl, n1, m, ld1, cdata, iopt)
       import :: DDS_DPR
       integer,intent(in) :: sgn, n1, m, ld1
       integer,intent(in) :: iopt
       real(KIND=DDS_DPR),intent(in) :: scl
       complex(KIND=DDS_DPR), dimension(1:ld1,1:m),intent(inout) :: cdata
     end function fft_zzfftm

     ! double complex-complex 2D FFT
     integer function fft_zzfft2d(sgn, scl, n1, n2, ld1, cdata, iopt)
       import :: DDS_DPR
       integer,intent(in) :: sgn, n1, n2, ld1
       integer,intent(in) :: iopt
       real(KIND=DDS_DPR),intent(in) :: scl
       complex(KIND=DDS_DPR), dimension(1:ld1,1:n2),intent(inout) :: cdata
     end function fft_zzfft2d

     ! double complex-complex 3D FFT
     integer function fft_zzfft3d(sgn, scl, n1, n2, n3, ld1, ld2, cdata, iopt)
       import :: DDS_DPR
       integer,intent(in) :: sgn, n1, n2, n3, ld1, ld2
       integer, intent(in) :: iopt
       real(KIND=DDS_DPR),intent(in) :: scl
       complex(KIND=DDS_DPR), dimension(1:ld1,1:ld2,1:n3), intent(inout) :: cdata
     end function fft_zzfft3d

     ! Parallel utilities
     ! initialize the DDS system for MPI parallelization
     integer function fdds_initmpi(node, nodes)
       integer, intent(out) :: node, nodes
     end function fdds_initmpi

     ! initialize the DDS system for MPIX parallelization
     integer function fdds_initmpix(node, nodes)
       integer, intent(out) :: node, nodes
     end function fdds_initmpix

     ! initialize the DDS system for OpenMP parallelization
     integer function fdds_initopenmp( )
     end function fdds_initopenmp

  end interface

  !     The following formatted output functions accept variable
  !     argument lists and are still declared as untyped external symbols.
  !     Their strongly-typed F2003 counterparts are defined as
  !     fdds_(funcname)2 and take up to 4 arguments.
  integer, external :: fdds_scanf, fdds_printf, fdds_sprintf, &
       fdds_prtmsg, fdds_prtcon, fdds_prterr

  interface fdds_read
     module procedure fdds_read_i,fdds_read_r
  end interface fdds_read

  interface fdds_write
     module procedure fdds_write_i,fdds_write_r
  end interface fdds_write

  !     get one field from a DDS type buffer
  !     overloaded fdds_getxx functions
  interface fdds_geti
     module procedure fdds_geti_arr,fdds_geti_var
  end interface fdds_geti

  interface fdds_geti2
     module procedure fdds_geti2_arr,fdds_geti2_var
  end interface fdds_geti2

  interface fdds_getcb
     module procedure fdds_getcb_arr,fdds_getcb_var
  end interface fdds_getcb

  interface fdds_getr
     module procedure fdds_getr_arr,fdds_getr_var
  end interface fdds_getr

  interface fdds_getd
     module procedure fdds_getd_arr,fdds_getd_var
  end interface fdds_getd

  interface fdds_getc
     module procedure fdds_getc_arr,fdds_getc_var
  end interface fdds_getc

  interface fdds_getff
     module procedure fdds_getff_arr,fdds_getff_var
  end interface fdds_getff

  !     put one field into a DDS type buffer
  !     overloaded fdds_putxx functions
  interface fdds_puti
     module procedure fdds_puti_arr,fdds_puti_var
  end interface fdds_puti

  interface fdds_puti2
     module procedure fdds_puti2_arr,fdds_puti2_var
  end interface fdds_puti2

  interface fdds_putcb
     module procedure fdds_putcb_arr,fdds_putcb_var
  end interface fdds_putcb

  interface fdds_putr
     module procedure fdds_putr_arr,fdds_putr_var
  end interface fdds_putr

  interface fdds_putd
     module procedure fdds_putd_arr,fdds_putd_var
  end interface fdds_putd

  interface fdds_putc
     module procedure fdds_putc_arr,fdds_putc_var
  end interface fdds_putc

  interface fdds_putff
     module procedure fdds_putff_arr,fdds_putff_var
  end interface fdds_putff

  !     note fdds_get is now a generic interface to all fdds_getxx calls
  !     not just the old DDS fdds_get that reads host-dependent bytes
  !     except get fake-float (fdds_getff)
  interface fdds_get
     module procedure &
          fdds_geti_arr, fdds_geti_var, fdds_geti2_arr, fdds_geti2_var,&
          fdds_get_arr, fdds_get_var, fdds_getcb_arr, fdds_getcb_var, &
          fdds_getr_arr, fdds_getr_var, fdds_getd_arr, fdds_getd_var, &
          fdds_getc_arr, fdds_getc_var
  end interface fdds_get

  !     map buffer and write binary stream
  interface fdds_writem
     module procedure &
          fdds_writem_i, fdds_writem_r, fdds_writem_d, fdds_writem_c
  end interface fdds_writem

  !     read binary stream and map buffer
  interface fdds_readm
     module procedure &
          fdds_readm_i, fdds_readm_r, fdds_readm_d, fdds_readm_c
  end interface fdds_readm

  !     convert fields between DDS type buffers
  !     fdds_map allows any combination of input & output buffer
  !     types and shapes, hence up to 64 module procedures
  !     may be required for a strongly-type interface
  !     (int, real, double precision, complex, arrays and vars)
  !     an external declaration is used instead of an interface.
  !     Alternatively, an interface can be defined for fdds_map
  !     using optional buffer variables of each type and shape, but
  !     might result in argument-type mismatch errors going undetected.
  integer, external :: fdds_map

  !     fdds_free accepts a pointer arg and must be external for
  !     platform independence
  integer, external :: fdds_free


  !     note fdds_put is now a generic interface to all fdds_putxx calls
  !     not just the old DDS fdds_put that reads host-dependent bytes
  !     except put fake-float (fdds_putff)
  interface fdds_put
     module procedure &
          fdds_puti_arr,fdds_puti_var, fdds_puti2_arr, fdds_puti2_var, &
          fdds_put_arr, fdds_put_var, fdds_putcb_arr, fdds_putcb_var, &
          fdds_putr_arr, fdds_putr_var, fdds_putd_arr, fdds_putd_var, &
          fdds_putc_arr, fdds_putc_var
  end interface fdds_put

  !     auxiliary private procedures used in defining
  !     various public overloaded interfaces
  !     or auxiliaries used in wrappers
  private :: &
       fdds_geti_arr,fdds_geti_var, fdds_geti2_arr, fdds_geti2_var, &
       fdds_get_arr, fdds_get_var, fdds_getcb_arr, fdds_getcb_var, &
       fdds_getr_arr, fdds_getr_var, fdds_getd_arr, fdds_getd_var, &
       fdds_getc_arr, fdds_getc_var, fdds_getff_arr, fdds_getff_var, &
       fdds_puti_arr, fdds_puti_var, fdds_puti2_arr, fdds_puti2_var, &
       fdds_put_arr, fdds_put_var, fdds_putcb_arr, fdds_putcb_var, &
       fdds_putr_arr, fdds_putr_var, fdds_putd_arr, fdds_putd_var, &
       fdds_putc_arr, fdds_putc_var, fdds_putff_arr, fdds_putff_var, &
       fdds_writem_i, fdds_writem_r, fdds_writem_d, fdds_writem_c, &
       fdds_readm_i, fdds_readm_r, fdds_readm_d, fdds_readm_c, &
       fddx_read_i1, fddx_read_i2, fddx_read_i4, fddx_read_i8, &
       fddx_read_r, fddx_read_d, fddx_read_c, fddx_read_z, &
       fddx_readfrom_i, fddx_readfrom_r, &
       fddx_readfrom_d, fddx_readfrom_c, &
       fddx_write_i1, fddx_write_i2, fddx_write_i4, fddx_write_i8, &
       fddx_write_r, fddx_write_d, fddx_write_c, fddx_write_z, &
       fddx_writeto_i, fddx_writeto_r, &
       fddx_writeto_d, fddx_writeto_c, &
       fdds_read_i, fdds_read_r, fdds_write_i, fdds_write_r

  !     read a DDS dataset
  interface fddx_read
     module procedure &
          fddx_read_i1, fddx_read_i2, fddx_read_i4, fddx_read_i8, &
          fddx_read_r, fddx_read_d, fddx_read_c, fddx_read_z
  end interface fddx_read

  !     seek to and read from a DDS dataset
  interface fddx_readfrom
     module procedure &
          fddx_readfrom_i, fddx_readfrom_r, &
          fddx_readfrom_d, fddx_readfrom_c
  end interface fddx_readfrom

  !     write to a DDS dataset
  interface fddx_write
     module procedure &
          fddx_write_i1, fddx_write_i2, fddx_write_i4, fddx_write_i8, &
          fddx_write_r, fddx_write_d, fddx_write_c, fddx_write_z
  end interface fddx_write

  !     seek and write to a DDS dataset
  interface fddx_writeto
     module procedure &
          fddx_writeto_i, fddx_writeto_r, &
          fddx_writeto_d, fddx_writeto_c
  end interface fddx_writeto

  !     Convenience routines
  interface
     !       add a new axis to a DDS dictionary
     integer function fddx_addaxis(bin_tag, num, name)
       integer, intent(in) :: bin_tag, num
       character(*), intent(in) :: name
     end function fddx_addaxis

     !       change DDS dictionary access
     integer function fddx_dict(bin_tag, mode)
       integer, intent(in) :: bin_tag
       character(*), intent(in) :: mode
     end function fddx_dict

     !       retrieve external DDS binary tag
     integer function fddx_getbin(bin_tag)
       integer, intent(in) :: bin_tag
     end function fddx_getbin

     !       retrieve the DDS dictionary name
     integer function fddx_getdict(dict, bin_tag)
       character(*), intent(out) :: dict
       integer, intent(in) :: bin_tag
     end function fddx_getdict

     !       open a DDS dataset for input
     integer function fddx_in(alias, value, title)
       character(len=*), intent(in) :: alias, value, title
     end function fddx_in

     !       open a DDS dataset for input, passing headers
     integer function fddx_in2(alias, value, title)
       character(len=*), intent(in) :: alias, value, title
     end function fddx_in2

     !       retrieve index to a DDS field
     integer function fddx_index(bin_tag, name, prime)
       integer, intent(in) :: bin_tag, prime
       character(len=*) name
     end function fddx_index

     !       initialize DDS dataset for input, selected headers
     integer function fddx_inhdr &
          (alias, value, title, sprime, hprime, hdrs)
       character(len=*), intent(in) :: alias, value, title, hdrs
       integer, intent(in) :: sprime, hprime
     end function fddx_inhdr

     !       reopen a DDS dataset
     integer function fddx_mod(alias, value, title, old_tag)
       character(len=*), intent(in) :: alias, value, title
       integer, intent(in) :: old_tag
     end function fddx_mod

     !       reopen a DDS dataset, passing headers
     integer function fddx_mod2(alias, value, title, old_tag)
       character(len=*), intent(in) :: alias, value, title
       integer, intent(in) :: old_tag
     end function fddx_mod2

     !       open a DDS dataset for output
     integer function fddx_out(alias, value, title, old_tag)
       character(len=*), intent(in) :: alias, value, title
       integer, intent(in) :: old_tag
     end function fddx_out

     !       initialize DDS dataset for output, selected headers
     integer function fddx_outhdr &
          (alias, value, title, old_tag, sprime, hprime, hdrs)
       character(len=*), intent(in) :: alias, value, title, hdrs
       integer, intent(in) :: old_tag, sprime, hprime
     end function fddx_outhdr

     !       remove an axis from a DDS dictionary
     integer function fddx_rmaxis(bin_tag, num)
       integer, intent(in) :: bin_tag, num
     end function fddx_rmaxis

  end interface

  !     this takes multiple arguments of arbitrary type and shape, hence best
  !     declared as an external to avoid complictated interface declarations
  integer, external :: fddx_readall

  !     used in variable-argument-list functions with run-time type checking
  type dds_addr
     !       argument type
     !       'i' - int, 'f' - float, 's' - string or array of 1-strings
     integer :: t
     !       string length in chars or var size in bytes
     integer :: l
     !       address of the Fortran object passed as an actual parameter
     !       (returned by loc())
     integer(KIND=DDS_DPI) :: p
  end type dds_addr

  !     strongly-typed var-arg routines defined as external C functions
  integer, external :: fdds_printf2, fdds_scanf2, fdds_sprintf2
  integer, external :: fdds_prtcon2, fdds_prterr2, fdds_prtmsg2

  !     function returning a DDS_ADDR struct for its argument
  interface ptr
     module procedure adri1,adri2,adri4,adri8,adrf, adrd,adrs,adrca
  end interface ptr
  !     with the canonical fortran API prefix
  interface dds_ptr
     module procedure adri1,adri2,adri4,adri8,adrf, adrd,adrs,adrca
  end interface dds_ptr

  !     functions (fdds_)ptr are visible outside this module,
  !     but their implementation is not
  private :: adri1,adri2,adri4,adri8,adrf, adrd,adrs,adrca

contains

  !     get a host-dependent integer from a raw int buffer
  !     into an int array
  integer function fdds_geti_arr &
       (bin, tag, in_buf, offset, out_buf, ncount)
    implicit none
    integer, external :: fdds_geti
    integer,intent(in) :: bin, tag
    !     raw input buffer
    integer, dimension(*),intent(in) :: in_buf
    integer,intent(in) :: offset, ncount
    !     out buffer -- array
    integer, dimension(*),intent(out) ::  out_buf

    fdds_geti_arr=fdds_geti(bin, tag, in_buf, offset, out_buf, ncount)
  end function fdds_geti_arr

  !     get a host-dependent integer from a raw int buffer
  !     into an int array
  integer function fdds_geti_var &
       (bin, tag, in_buf, offset, out_buf, ncount)
    implicit none
    integer, external :: fdds_geti
    integer,intent(in) :: bin, tag
    !     raw input buffer
    integer, dimension(*),intent(in) :: in_buf
    integer,intent(in) :: offset, ncount
    !     out buffer -- variable
    integer,intent(out) :: out_buf

    fdds_geti_var=fdds_geti(bin, tag, in_buf, offset, out_buf, ncount)
  end function fdds_geti_var

  !     get a host-dependent 2-byte integer from a raw int buffer
  !     into an int array
  integer function fdds_geti2_arr &
       (bin, tag, in_buf, offset, out_buf, ncount)
    implicit none
    integer, external :: fdds_geti2
    integer,intent(in) :: bin, tag
    !     raw input buffer
    integer, dimension(*),intent(in) :: in_buf
    integer,intent(in) :: offset, ncount
    !     out buffer -- array
    integer(KIND=DDS_SHI), dimension(*),intent(out) :: out_buf

    fdds_geti2_arr=fdds_geti2(bin,tag,in_buf,offset,out_buf,ncount)
  end function fdds_geti2_arr

  !     get a host-dependent 2-byte integer from a raw int buffer
  !     into an int array
  integer function fdds_geti2_var &
       (bin, tag, in_buf, offset, out_buf, ncount)
    implicit none
    integer, external :: fdds_geti2
    integer,intent(in) :: bin, tag
    !     raw input buffer
    integer, dimension(*),intent(in) :: in_buf
    integer,intent(in) :: offset, ncount
    !     out buffer -- variable
    integer(KIND=DDS_SHI),intent(out) :: out_buf

    fdds_geti2_var=fdds_geti2(bin,tag,in_buf,offset,out_buf,ncount)
  end function fdds_geti2_var

  !     get a host-dependent byte(s) from a raw int buffer
  !     into a byte array
  integer function fdds_get_arr &
       (bin, tag, in_buf, offset, out_buf, ncount)
    implicit none
    integer, external :: fdds_get
    integer,intent(in) :: bin, tag
    !     raw input buffer
    integer, dimension(*),intent(in) :: in_buf
    integer,intent(in) :: offset, ncount
    !     out buffer -- array
    integer(KIND=DDS_SSI), dimension(*),intent(out) :: out_buf

    fdds_get_arr=fdds_get(bin, tag, in_buf, offset, out_buf, ncount)
  end function fdds_get_arr

  !     get a host-dependent byte(s) from a raw int buffer
  !     into a byte array
  integer function fdds_get_var &
       (bin, tag, in_buf, offset, out_buf, ncount)
    implicit none
    integer, external :: fdds_get
    integer,intent(in) :: bin, tag
    !     raw input buffer
    integer, dimension(*),intent(in) :: in_buf
    integer,intent(in) :: offset, ncount
    !     out buffer -- variable
    integer(KIND=DDS_SSI),intent(out) :: out_buf

    fdds_get_var=fdds_get(bin, tag, in_buf, offset, out_buf, ncount)
  end function fdds_get_var

  !     get a host-dependent character(s) from a raw int buffer
  !     into a string array
  integer function fdds_getcb_arr &
       (bin, tag, in_buf, offset, out_buf)
    implicit none
    integer, external :: fdds_getcb
    integer,intent(in) :: bin, tag
    !     raw input buffer
    integer, dimension(*),intent(in) :: in_buf
    integer,intent(in) :: offset
    !     out buffer -- array
    character(len=*), dimension(*),intent(out) :: out_buf

    fdds_getcb_arr=fdds_getcb(bin, tag, in_buf, offset, out_buf)
  end function fdds_getcb_arr

  !     get a host-dependent byte(s) from a raw int buffer
  !     into a byte array
  integer function fdds_getcb_var(bin, tag, in_buf, offset, out_buf)
    implicit none
    integer, external :: fdds_getcb
    integer,intent(in) :: bin, tag
    !     raw input buffer
    integer, dimension(*),intent(in) :: in_buf
    integer,intent(in) :: offset
    !     out buffer -- variable
    character(len=*),intent(out) :: out_buf

    fdds_getcb_var=fdds_getcb(bin, tag, in_buf, offset, out_buf)
  end function fdds_getcb_var

  !     get a host-dependent real(s) from a raw int buffer
  !     into a real array
  integer function fdds_getr_arr &
       (bin, tag, in_buf, offset, out_buf, ncount)
    implicit none
    integer, external :: fdds_getr
    integer,intent(in) :: bin, tag
    !     raw input buffer
    integer, dimension(*),intent(in) :: in_buf
    integer,intent(in) :: offset, ncount
    !     out buffer -- array
    real, dimension(*),intent(out) :: out_buf

    fdds_getr_arr=fdds_getr(bin, tag, in_buf, offset, out_buf, ncount)
  end function fdds_getr_arr

  !     get a host-dependent real(s) from a raw int buffer
  !     into a real var
  integer function fdds_getr_var &
       (bin, tag, in_buf, offset, out_buf, ncount)
    implicit none
    integer, external :: fdds_getr
    integer,intent(in) :: bin, tag
    !     raw input buffer
    integer, dimension(*),intent(in) :: in_buf
    integer,intent(in) :: offset, ncount
    !     out buffer -- variable
    real,intent(out) :: out_buf

    fdds_getr_var=fdds_getr(bin, tag, in_buf, offset, out_buf, ncount)
  end function fdds_getr_var

  !     get a host-dependent complex(es) from a raw int buffer
  !     into a complex array
  integer function fdds_getc_arr &
       (bin, tag, in_buf, offset, out_buf, ncount)
    implicit none
    integer, external :: fdds_getc
    integer,intent(in) :: bin, tag
    !     raw input buffer
    integer, dimension(*),intent(in) :: in_buf
    integer,intent(in) :: offset, ncount
    !     out buffer -- array
    complex, dimension(*),intent(out) :: out_buf

    fdds_getc_arr=fdds_getc(bin, tag, in_buf, offset, out_buf, ncount)
  end function fdds_getc_arr

  !     get a host-dependent complex(es) from a raw int buffer
  !     into a complex var
  integer function fdds_getc_var &
       (bin, tag, in_buf, offset, out_buf, ncount)
    implicit none
    integer, external :: fdds_getc
    integer,intent(in) :: bin, tag
    !     raw input buffer
    integer, dimension(*),intent(in) :: in_buf
    integer,intent(in) :: offset, ncount
    !     out buffer -- variable
    complex,intent(out) :: out_buf

    fdds_getc_var=fdds_getc(bin, tag, in_buf, offset, out_buf, ncount)
  end function fdds_getc_var

  !     get a host-dependent double(s) from a raw int buffer
  !     into a doubles array
  integer function fdds_getd_arr &
       (bin, tag, in_buf, offset, out_buf, ncount)
    implicit none
    integer, external :: fdds_getd
    integer,intent(in) :: bin, tag
    !     raw input buffer
    integer, dimension(*),intent(in) :: in_buf
    integer,intent(in) :: offset, ncount
    !     out buffer -- array
    real(KIND=DDS_DPR), dimension(*),intent(out) :: out_buf

    fdds_getd_arr=fdds_getd(bin, tag, in_buf, offset, out_buf, ncount)
  end function fdds_getd_arr

  !     get a host-dependent double(s) from a raw int buffer
  !     into a doubles var
  integer function fdds_getd_var &
       (bin, tag, in_buf, offset, out_buf, ncount)
    implicit none
    integer, external :: fdds_getd
    integer,intent(in) :: bin, tag
    !     raw input buffer
    integer, dimension(*),intent(in) :: in_buf
    integer,intent(in) :: offset, ncount
    !     out buffer -- variable
    real(KIND=DDS_DPR),intent(out) :: out_buf

    fdds_getd_var=fdds_getd(bin, tag, in_buf, offset, out_buf, ncount)
  end function fdds_getd_var

  !     get DDS fake float(s) from a raw int buffer
  !     into a real array
  integer function fdds_getff_arr &
       (bin, tag, in_buf, offset, out_buf, ncount)
    implicit none
    integer, external :: fdds_getff
    integer,intent(in) :: bin, tag
    !     raw input buffer
    integer, dimension(*),intent(in) :: in_buf
    integer,intent(in) :: offset, ncount
    !     out buffer -- array
    real, dimension(*),intent(out) :: out_buf

    fdds_getff_arr=fdds_getff(bin,tag,in_buf,offset,out_buf,ncount)
  end function fdds_getff_arr

  !     get DDS fake float(s) from a raw int buffer
  !     into a real var
  integer function fdds_getff_var &
       (bin, tag, in_buf, offset, out_buf, ncount)
    implicit none
    integer, external :: fdds_getff
    integer,intent(in) :: bin, tag
    !     raw input buffer
    integer, dimension(*),intent(in) :: in_buf
    integer,intent(in) :: offset, ncount
    !     out buffer -- variable
    real,intent(out) :: out_buf

    fdds_getff_var=fdds_getff(bin,tag,in_buf,offset,out_buf,ncount)
  end function fdds_getff_var

  !     fdds_putxx -- output counterparts of fdds_getxx
  !     put a host-dependent integer in a raw int buffer
  !     from an int array
  integer function fdds_puti_arr &
       (bin, tag, out_buf, offset, in_buf, ncount)
    implicit none
    integer, external :: fdds_puti
    integer,intent(in) :: bin, tag
    integer, dimension(*),intent(out) :: out_buf
    integer,intent(in) :: offset, ncount
    integer, dimension(*),intent(in) ::  in_buf

    fdds_puti_arr=fdds_puti(bin, tag, out_buf, offset, in_buf, ncount)
  end function fdds_puti_arr

  !     put a host-dependent integer in a raw int buffer
  !     from an int array
  integer function fdds_puti_var &
       (bin, tag, out_buf, offset, in_buf, ncount)
    implicit none
    integer, external :: fdds_puti
    integer,intent(in) :: bin, tag
    integer, dimension(*),intent(out) :: out_buf
    integer,intent(in) :: offset, ncount
    integer,intent(in) ::  in_buf

    fdds_puti_var=fdds_puti(bin, tag, out_buf, offset, in_buf, ncount)
  end function fdds_puti_var

  !     put a host-dependent 2-byte integer in a raw int buffer
  !     from an int array
  integer function fdds_puti2_arr &
       (bin, tag, out_buf, offset, in_buf, ncount)
    implicit none
    integer, external :: fdds_puti2
    integer,intent(in) :: bin, tag
    integer, dimension(*),intent(out) :: out_buf
    integer,intent(in) :: offset, ncount
    integer(KIND=DDS_SHI), dimension(*),intent(in) ::  in_buf

    fdds_puti2_arr=fdds_puti2(bin,tag,out_buf,offset,in_buf,ncount)
  end function fdds_puti2_arr

  !     put a host-dependent 2-byte integer in a raw int buffer
  !     from an int array
  integer function fdds_puti2_var &
       (bin, tag, out_buf, offset, in_buf, ncount)
    implicit none
    integer, external :: fdds_puti2
    integer,intent(in) :: bin, tag
    integer, dimension(*),intent(out) :: out_buf
    integer,intent(in) :: offset, ncount
    integer(KIND=DDS_SHI),intent(in) ::  in_buf

    fdds_puti2_var=fdds_puti2(bin,tag,out_buf,offset,in_buf,ncount)
  end function fdds_puti2_var

  !     put a host-dependent byte(s) in a raw int buffer
  !     from a byte array
  integer function fdds_put_arr &
       (bin, tag, out_buf, offset, in_buf, ncount)
    implicit none
    integer, external :: fdds_put
    integer,intent(in) :: bin, tag
    integer, dimension(*),intent(out) :: out_buf
    integer,intent(in) :: offset, ncount
    integer(KIND=DDS_SSI), dimension(*),intent(in) ::  in_buf

    fdds_put_arr=fdds_put(bin, tag, out_buf, offset, in_buf, ncount)
  end function fdds_put_arr

  !     put a host-dependent byte(s) in a raw int buffer
  !     from a byte array
  integer function fdds_put_var &
       (bin, tag, out_buf, offset, in_buf, ncount)
    implicit none
    integer, external :: fdds_put
    integer,intent(in) :: bin, tag
    integer, dimension(*),intent(out) :: out_buf
    integer,intent(in) :: offset, ncount
    integer(KIND=DDS_SSI),intent(in) ::  in_buf

    fdds_put_var=fdds_put(bin, tag, out_buf, offset, in_buf, ncount)
  end function fdds_put_var

  !     put a host-dependent character(s) in a raw int buffer
  !     from a string array
  integer function fdds_putcb_arr(bin, tag, out_buf, offset, in_buf)
    implicit none
    integer, external :: fdds_putcb
    integer,intent(in) :: bin, tag
    integer, dimension(*),intent(out) :: out_buf
    integer,intent(in) :: offset
    character(len=*), dimension(*),intent(in) ::  in_buf

    fdds_putcb_arr=fdds_putcb(bin, tag, out_buf, offset, in_buf)
  end function fdds_putcb_arr

  !     put a host-dependent byte(s) in a raw int buffer
  !     from a byte array
  integer function fdds_putcb_var(bin, tag, out_buf, offset, in_buf)
    implicit none
    integer, external :: fdds_putcb
    integer,intent(in) :: bin, tag
    integer, dimension(*),intent(out) :: out_buf
    integer,intent(in) :: offset
    character(len=*),intent(in) ::  in_buf

    fdds_putcb_var=fdds_putcb(bin, tag, out_buf, offset, in_buf)
  end function fdds_putcb_var

  !     put a host-dependent real(s) in a raw int buffer
  !     from a real array
  integer function fdds_putr_arr &
       (bin, tag, out_buf, offset, in_buf, ncount)
    implicit none
    integer, external :: fdds_putr
    integer,intent(in) :: bin, tag
    integer, dimension(*),intent(out) :: out_buf
    integer,intent(in) :: offset, ncount
    real, dimension(*),intent(in) ::  in_buf

    fdds_putr_arr=fdds_putr(bin, tag, out_buf, offset, in_buf, ncount)
  end function fdds_putr_arr

  !     put a host-dependent real(s) in a raw int buffer
  !     from a real var
  integer function fdds_putr_var &
       (bin, tag, out_buf, offset, in_buf, ncount)
    implicit none
    integer, external :: fdds_putr
    integer,intent(in) :: bin, tag
    integer, dimension(*),intent(out) :: out_buf
    integer,intent(in) :: offset, ncount
    real,intent(in) ::  in_buf

    fdds_putr_var=fdds_putr(bin, tag, out_buf, offset, in_buf, ncount)
  end function fdds_putr_var

  !     put a host-dependent complex(es) in a raw int buffer
  !     from a complex array
  integer function fdds_putc_arr &
       (bin, tag, out_buf, offset, in_buf, ncount)
    implicit none
    integer, external :: fdds_putc
    integer,intent(in) :: bin, tag
    integer, dimension(*),intent(out) :: out_buf
    integer,intent(in) :: offset, ncount
    complex, dimension(*),intent(in) ::  in_buf

    fdds_putc_arr=fdds_putc(bin, tag, out_buf, offset, in_buf, ncount)
  end function fdds_putc_arr

  !     put a host-dependent complex(es) in a raw int buffer
  !     from a complex var
  integer function fdds_putc_var &
       (bin, tag, out_buf, offset, in_buf, ncount)
    implicit none
    integer, external :: fdds_putc
    integer,intent(in) :: bin, tag
    integer, dimension(*),intent(out) :: out_buf
    integer,intent(in) :: offset, ncount
    complex,intent(in) ::  in_buf

    fdds_putc_var=fdds_putc(bin, tag, out_buf, offset, in_buf, ncount)
  end function fdds_putc_var

  !     put a host-dependent double(s) in a raw int buffer
  !     from a doubles array
  integer function fdds_putd_arr &
       (bin, tag, out_buf, offset, in_buf, ncount)
    implicit none
    integer, external :: fdds_putd
    integer,intent(in) :: bin, tag
    integer, dimension(*),intent(out) :: out_buf
    integer,intent(in) :: offset, ncount
    real(KIND=DDS_DPR), dimension(*),intent(in) ::  in_buf

    fdds_putd_arr=fdds_putd(bin, tag, out_buf, offset, in_buf, ncount)
  end function fdds_putd_arr

  !     put a host-dependent double(s) in a raw int buffer
  !     from a doubles var
  integer function fdds_putd_var &
       (bin, tag, out_buf, offset, in_buf, ncount)
    implicit none
    integer, external :: fdds_putd
    integer,intent(in) :: bin, tag
    integer, dimension(*),intent(out) :: out_buf
    integer,intent(in) :: offset, ncount
    real(KIND=DDS_DPR),intent(in) ::  in_buf

    fdds_putd_var=fdds_putd(bin, tag, out_buf, offset, in_buf, ncount)
  end function fdds_putd_var

  !     put DDS fake float(s) in a raw int buffer
  !     from a real array
  integer function fdds_putff_arr &
       (bin, tag, out_buf, offset, in_buf, ncount)
    implicit none
    integer, external :: fdds_putff
    integer,intent(in) :: bin, tag
    integer, dimension(*),intent(out) :: out_buf
    integer,intent(in) :: offset, ncount
    real, dimension(*),intent(in) ::  in_buf

    fdds_putff_arr=fdds_putff(bin,tag,out_buf,offset,in_buf,ncount)
  end function fdds_putff_arr

  !     put DDS fake float(s) in a raw int buffer
  !     from a real var
  integer function fdds_putff_var &
       (bin, tag, out_buf, offset, in_buf, ncount)
    implicit none
    integer, external :: fdds_putff
    integer,intent(in) :: bin, tag
    integer, dimension(*),intent(out) :: out_buf
    integer,intent(in) :: offset, ncount
    real,intent(in) ::  in_buf

    fdds_putff_var=fdds_putff(bin,tag,out_buf,offset,in_buf,ncount)
  end function fdds_putff_var

  !     read DDS binary data
  integer function fdds_read_i(bin, tag, buf, ncount)
    implicit none
    integer, external :: fdds_read
    integer, intent(in) :: bin
    integer, intent(in) :: tag
    integer,intent(out) :: buf(*)
    integer, intent(in) :: ncount
    fdds_read_i = fdds_read(bin, tag, buf, ncount)
  end function fdds_read_i

  integer function fdds_read_r(bin, tag, buf, ncount)
    implicit none
    integer, external :: fdds_read
    integer, intent(in) :: bin
    integer, intent(in) :: tag
    real,   intent(out) :: buf(*)
    integer, intent(in) :: ncount
    fdds_read_r = fdds_read(bin, tag, buf, ncount)
  end function fdds_read_r

  !     write DDS binary data
  integer function fdds_write_i(bin, tag, buf, ncount)
    implicit none
    integer, external :: fdds_write
    integer, intent(in) :: bin
    integer, intent(in) :: tag
    integer, intent(in) :: buf(*)
    integer, intent(in) :: ncount
    fdds_write_i = fdds_write(bin, tag, buf, ncount)
  end function fdds_write_i

  integer function fdds_write_r(bin, tag, buf, ncount)
    implicit none
    integer, external :: fdds_write
    integer, intent(in) :: bin
    integer, intent(in) :: tag
    real,    intent(in) :: buf(*)
    integer, intent(in) :: ncount
    fdds_write_r = fdds_write(bin, tag, buf, ncount)
  end function fdds_write_r

  !     map buffer and write binary stream - int buffer
  integer function fdds_writem_i &
       (out_bin, out_tag, in_bin, in_tag, buf, ncount)
    implicit none
    integer, external :: fdds_writem
    integer, intent(in) :: out_bin, out_tag, in_bin, in_tag, ncount
    integer, dimension(*), intent(in) :: buf

    fdds_writem_i = fdds_writem &
         (out_bin, out_tag, in_bin, in_tag, buf, ncount)
  end function fdds_writem_i

  !     map buffer and write binary stream - real buffer
  integer function fdds_writem_r &
       (out_bin, out_tag, in_bin, in_tag, buf, ncount)
    implicit none
    integer, external :: fdds_writem
    integer, intent(in) :: out_bin, out_tag, in_bin, in_tag, ncount
    real, dimension(*), intent(in) :: buf

    fdds_writem_r = fdds_writem &
         (out_bin, out_tag, in_bin, in_tag, buf, ncount)
  end function fdds_writem_r

  !     map buffer and write binary stream - double buffer
  integer function fdds_writem_d &
       (out_bin, out_tag, in_bin, in_tag, buf, ncount)
    implicit none
    integer, external :: fdds_writem
    integer, intent(in) :: out_bin, out_tag, in_bin, in_tag, ncount
    real(KIND=DDS_DPR), dimension(*), intent(in) :: buf

    fdds_writem_d = fdds_writem &
         (out_bin, out_tag, in_bin, in_tag, buf, ncount)
  end function fdds_writem_d

  !     map buffer and write binary stream - compplex buffer
  integer function fdds_writem_c &
       (out_bin, out_tag, in_bin, in_tag, buf, ncount)
    implicit none
    integer, external :: fdds_writem
    integer, intent(in) :: out_bin, out_tag, in_bin, in_tag, ncount
    complex, dimension(*), intent(in) :: buf

    fdds_writem_c = fdds_writem &
         (out_bin, out_tag, in_bin, in_tag, buf, ncount)
  end function fdds_writem_c

  !     read DDS binary stream and map buffer - integer buffer
  integer function fdds_readm_i &
       (in_bin, in_tag, out_bin, out_tag, buf, ncount)
    implicit none
    integer, external :: fdds_readm
    integer, intent(in) :: in_bin, in_tag, out_bin, out_tag, ncount
    integer, dimension(*), intent(out) :: buf

    fdds_readm_i = fdds_readm &
         (in_bin, in_tag, out_bin, out_tag, buf, ncount)
  end function fdds_readm_i

  !     read DDS binary stream and map buffer - real buffer
  integer function fdds_readm_r &
       (in_bin, in_tag, out_bin, out_tag, buf, ncount)
    implicit none
    integer, external :: fdds_readm
    integer, intent(in) :: in_bin, in_tag, out_bin, out_tag, ncount
    real, dimension(*), intent(out) :: buf

    fdds_readm_r = fdds_readm &
         (in_bin, in_tag, out_bin, out_tag, buf, ncount)
  end function fdds_readm_r

  !     read DDS binary stream and map buffer - double precision buffer
  integer function fdds_readm_d &
       (in_bin, in_tag, out_bin, out_tag, buf, ncount)
    implicit none
    integer, external :: fdds_readm
    integer, intent(in) :: in_bin, in_tag, out_bin, out_tag, ncount
    real(KIND=DDS_DPR), dimension(*), intent(out) :: buf

    fdds_readm_d = fdds_readm &
         (in_bin, in_tag, out_bin, out_tag, buf, ncount)
  end function fdds_readm_d

  !     read DDS binary stream and map buffer - complex buffer
  integer function fdds_readm_c &
       (in_bin, in_tag, out_bin, out_tag, buf, ncount)
    implicit none
    integer, external :: fdds_readm
    integer, intent(in) :: in_bin, in_tag, out_bin, out_tag, ncount
    complex, dimension(*), intent(out) :: buf

    fdds_readm_c = fdds_readm &
         (in_bin, in_tag, out_bin, out_tag, buf, ncount)
  end function fdds_readm_c

  !     DDS convenience -- write to a DDS dataset (from a 1-byte integer buffer)
  integer function fddx_write_i1(bin_tag, buf, ntrc)
    implicit none
    integer, intent(in) :: bin_tag, ntrc
    integer(KIND=DDS_SSI), dimension(*), intent(in) :: buf
    integer, external :: fddx_write

    fddx_write_i1 = fddx_write(bin_tag, buf, ntrc)
  end function fddx_write_i1

  !     DDS convenience -- write to a DDS dataset (from a 2-byte integer buffer)
  integer function fddx_write_i2(bin_tag, buf, ntrc)
    implicit none
    integer, intent(in) :: bin_tag, ntrc
    integer(KIND=DDS_SHI), dimension(*), intent(in) :: buf
    integer, external :: fddx_write

    fddx_write_i2 = fddx_write(bin_tag, buf, ntrc)
  end function fddx_write_i2

  !     DDS convenience -- write to a DDS dataset (from a 4-byte integer buffer)
  integer function fddx_write_i4(bin_tag, buf, ntrc)
    implicit none
    integer, intent(in) :: bin_tag, ntrc
    integer(KIND=DDS_SPI), dimension(*), intent(in) :: buf
    integer, external :: fddx_write

    fddx_write_i4 = fddx_write(bin_tag, buf, ntrc)
  end function fddx_write_i4

  !     DDS convenience -- write to a DDS dataset (from an 8-byte integer buffer)
  integer function fddx_write_i8(bin_tag, buf, ntrc)
    implicit none
    integer, intent(in) :: bin_tag, ntrc
    integer(KIND=DDS_DPI), dimension(*), intent(in) :: buf
    integer, external :: fddx_write

    fddx_write_i8 = fddx_write(bin_tag, buf, ntrc)
  end function fddx_write_i8

  !     DDS convenience -- write to a DDS dataset (from a 4-byte real buffer)
  integer function fddx_write_r(bin_tag, buf, ntrc)
    implicit none
    integer, intent(in) :: bin_tag, ntrc
    real(KIND=DDS_SPR), dimension(*), intent(in) :: buf
    integer, external :: fddx_write

    fddx_write_r = fddx_write(bin_tag, buf, ntrc)
  end function fddx_write_r

  !     DDS convenience -- write to a DDS dataset (from an 8-byte real buffer)
  integer function fddx_write_d(bin_tag, buf, ntrc)
    implicit none
    integer, intent(in) :: bin_tag, ntrc
    real(KIND=DDS_DPR), dimension(*), intent(in) :: buf
    integer, external :: fddx_write

    fddx_write_d = fddx_write(bin_tag, buf, ntrc)
  end function fddx_write_d

  !     DDS convenience -- write to a DDS dataset (from an 8-byte complex buffer)
  integer function fddx_write_c(bin_tag, buf, ntrc)
    implicit none
    integer, intent(in) :: bin_tag, ntrc
    complex(KIND=DDS_SPR), dimension(*), intent(in) :: buf
    integer, external :: fddx_write

    fddx_write_c = fddx_write(bin_tag, buf, ntrc)
  end function fddx_write_c

  !     DDS convenience -- write to a DDS dataset (from an 16-byte complex buffer)
  integer function fddx_write_z(bin_tag, buf, ntrc)
    implicit none
    integer, intent(in) :: bin_tag, ntrc
    complex(KIND=DDS_DPR), dimension(*), intent(in) :: buf
    integer, external :: fddx_write

    fddx_write_z = fddx_write(bin_tag, buf, ntrc)
  end function fddx_write_z

  !     DDS convenience -- seek and write an integer buffer to a DDS dataset
  integer function fddx_writeto_i(bin_tag, offset, buf, ntrc)
    implicit none
    integer, intent(in) :: bin_tag, offset, ntrc
    integer, dimension(*), intent(in) :: buf
    integer, external :: fddx_writeto

    fddx_writeto_i = fddx_writeto(bin_tag, offset, buf, ntrc)
  end function fddx_writeto_i

  !     DDS convenience -- seek and write a real buffer to a DDS dataset
  integer function fddx_writeto_r(bin_tag, offset, buf, ntrc)
    implicit none
    integer, intent(in) :: bin_tag, offset, ntrc
    real, dimension(*), intent(in) :: buf
    integer, external :: fddx_writeto

    fddx_writeto_r = fddx_writeto(bin_tag, offset, buf, ntrc)
  end function fddx_writeto_r

  !     DDS convenience -- seek and write a real*8 buffer to a DDS dataset
  integer function fddx_writeto_d(bin_tag, offset, buf, ntrc)
    implicit none
    integer, intent(in) :: bin_tag, offset, ntrc
    real(KIND=DDS_DPR), dimension(*), intent(in) :: buf
    integer, external :: fddx_writeto

    fddx_writeto_d = fddx_writeto(bin_tag, offset, buf, ntrc)
  end function fddx_writeto_d

  !     DDS convenience -- seek and write a complex buffer to a DDS dataset
  integer function fddx_writeto_c(bin_tag, offset, buf, ntrc)
    implicit none
    integer, intent(in) :: bin_tag, offset, ntrc
    complex, dimension(*), intent(in) :: buf
    integer, external :: fddx_writeto

    fddx_writeto_c = fddx_writeto(bin_tag, offset, buf, ntrc)
  end function fddx_writeto_c

  !     DDS convenience -- read traces (into a 1-byte integer buffer)
  integer function fddx_read_i1(bin_tag, buf, ntrc)
    implicit none
    integer, intent(in) :: bin_tag, ntrc
    integer(KIND=DDS_SSI), dimension(*), intent(out) :: buf
    integer, external :: fddx_read

    fddx_read_i1 = fddx_read(bin_tag, buf, ntrc)
  end function fddx_read_i1

  !     DDS convenience -- read traces (into a 2-byte integer buffer)
  integer function fddx_read_i2(bin_tag, buf, ntrc)
    implicit none
    integer, intent(in) :: bin_tag, ntrc
    integer(KIND=DDS_SHI), dimension(*), intent(out) :: buf
    integer, external :: fddx_read

    fddx_read_i2 = fddx_read(bin_tag, buf, ntrc)
  end function fddx_read_i2

  !     DDS convenience -- read traces (into a 4-byte integer buffer)
  integer function fddx_read_i4(bin_tag, buf, ntrc)
    implicit none
    integer, intent(in) :: bin_tag, ntrc
    integer(KIND=DDS_SPI), dimension(*), intent(out) :: buf
    integer, external :: fddx_read

    fddx_read_i4 = fddx_read(bin_tag, buf, ntrc)
  end function fddx_read_i4

  !     DDS convenience -- read traces (into an 8-byte integer buffer)
  integer function fddx_read_i8(bin_tag, buf, ntrc)
    implicit none
    integer, intent(in) :: bin_tag, ntrc
    integer(KIND=DDS_DPI), dimension(*), intent(out) :: buf
    integer, external :: fddx_read

    fddx_read_i8 = fddx_read(bin_tag, buf, ntrc)
  end function fddx_read_i8

  !     DDS convenience -- read traces (into a 4-byte real buffer)
  integer function fddx_read_r(bin_tag, buf, ntrc)
    implicit none
    integer, intent(in) :: bin_tag, ntrc
    real(KIND=DDS_SPR), dimension(*), intent(out) :: buf
    integer, external :: fddx_read

    fddx_read_r = fddx_read(bin_tag, buf, ntrc)
  end function fddx_read_r

  !     DDS convenience -- read traces (into an 8-byte real buffer)
  integer function fddx_read_d(bin_tag, buf, ntrc)
    implicit none
    integer, intent(in) :: bin_tag, ntrc
    real(KIND=DDS_DPR), dimension(*), intent(out) :: buf
    integer, external :: fddx_read

    fddx_read_d = fddx_read(bin_tag, buf, ntrc)
  end function fddx_read_d

  !     DDS convenience -- read traces (into an 8-byte complex buffer)
  integer function fddx_read_c(bin_tag, buf, ntrc)
    implicit none
    integer, intent(in) :: bin_tag, ntrc
    complex(KIND=DDS_SPR), dimension(*), intent(out) :: buf
    integer, external :: fddx_read

    fddx_read_c = fddx_read(bin_tag, buf, ntrc)
  end function fddx_read_c

  !     DDS convenience -- read traces (into a 16-byte complex buffer)
  integer function fddx_read_z(bin_tag, buf, ntrc)
    implicit none
    integer, intent(in) :: bin_tag, ntrc
    complex(KIND=DDS_DPR), dimension(*), intent(out) :: buf
    integer, external :: fddx_read

    fddx_read_z = fddx_read(bin_tag, buf, ntrc)
  end function fddx_read_z

  !     seek to and read from a DDS dataset (into an integer buffer)
  integer function fddx_readfrom_i(bin_tag, offset, buf, ncount)
    implicit none
    integer, intent(in) :: bin_tag, offset, ncount
    integer, dimension(*), intent(out) :: buf
    integer, external :: fddx_readfrom

    fddx_readfrom_i = fddx_readfrom(bin_tag, offset, buf, ncount)
  end function fddx_readfrom_i

  !     seek to and read from a DDS dataset (into a real buffer)
  integer function fddx_readfrom_r(bin_tag, offset, buf, ncount)
    implicit none
    integer, intent(in) :: bin_tag, offset, ncount
    real, dimension(*), intent(out) :: buf
    integer, external :: fddx_readfrom

    fddx_readfrom_r = fddx_readfrom(bin_tag, offset, buf, ncount)
  end function fddx_readfrom_r

  !     seek to and read from a DDS dataset (into a double precision buffer)
  integer function fddx_readfrom_d(bin_tag, offset, buf, ncount)
    implicit none
    integer, intent(in) :: bin_tag, offset, ncount
    real(KIND=DDS_DPR), dimension(*), intent(out) :: buf
    integer, external :: fddx_readfrom

    fddx_readfrom_d = fddx_readfrom(bin_tag, offset, buf, ncount)
  end function fddx_readfrom_d

  !     seek to and read from a DDS dataset (into a complex buffer)
  integer function fddx_readfrom_c(bin_tag, offset, buf, ncount)
    implicit none
    integer, intent(in) :: bin_tag, offset, ncount
    complex, dimension(*), intent(out) :: buf
    integer, external :: fddx_readfrom

    fddx_readfrom_c = fddx_readfrom(bin_tag, offset, buf, ncount)
  end function fddx_readfrom_c

  type(dds_addr) function adri1(a)
    implicit none
    integer(KIND=DDS_SSI),intent(inout) :: a
    adri1%p = loc(a)
    adri1%t = iachar('i')
    adri1%l=1
  end function adri1

  type(dds_addr) function adri2(a)
    implicit none
    integer(KIND=DDS_SHI),intent(inout) :: a
    adri2%p = loc(a)
    adri2%t = iachar('i')
    adri2%l=2
  end function adri2

  type(dds_addr) function adri4(a)
    implicit none
    integer(KIND=DDS_SPI),intent(inout) :: a
    adri4%p = loc(a)
    adri4%t = iachar('i')
    adri4%l= 4
  end function adri4

  type(dds_addr) function adri8(a)
    implicit none
    integer(KIND=DDS_DPI),intent(inout) :: a
    adri8%p = loc(a)
    adri8%t = iachar('i')
    adri8%l= 8
  end function adri8

  type(dds_addr) function adrf(a)
    implicit none
    real,intent(inout) :: a
    adrf%p = loc(a)
    adrf%t = iachar('f')
    adrf%l= 4
  end function adrf

  type(dds_addr) function adrd(a)
    implicit none
    real(KIND=DDS_DPR),intent(inout) :: a
    adrd%p = loc(a)
    adrd%t = iachar('f')
    adrd%l= 8
  end function adrd

  !     string
  type(dds_addr) function adrs(a)
    implicit none
    character(len=*),intent(inout) :: a
    adrs%p = loc(a)
    adrs%t = iachar('s')
    adrs%l= len(a)
  end function adrs

  !     char array
  type(dds_addr) function adrca(a)
    implicit none
    character(len=1),dimension(:),intent(inout) :: a
    adrca%p = loc(a)
    adrca%t = iachar('s')
    adrca%l= ubound(a,1)
  end function adrca

end module DDS
