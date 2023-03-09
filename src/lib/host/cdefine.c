static char rcsid[]="$Id: cdefine.c 198 2010-10-28 19:59:55Z user $";
/*
HOST, main, test and display various C pre-processor #define
*/

#include <stdio.h>
#include <stddef.h>
#include <fcntl.h>
#include "chost.h" 

#define DDS_INTEGER4   1
#define DDS_INTEGER4X  2
#define DDS_INTEGER8   3
#define DDS_INTEGER8X  4

int main(argc, argv)
   int argc;
   char **argv;
{
   /*! TEST Program: attempt to discover compiler "#define" macros */


      printf("\n- System defines -\n");
#ifdef PATHNAME_MAX
      printf("#define PATHNAME_MAX %iL\n", (int)(PATHNAME_MAX));
#endif


#ifdef unix
      printf("#define unix %iL\n", (int)(unix));
#endif
#ifdef __unix
      printf("#define __unix %iL\n", (int)(__unix));
#endif
#ifdef BSD
      printf("#define BSD %iL\n", (int)(BSD));
#endif
#ifdef _BSD_COMPAT
      printf("#define _BSD_COMPAT %iL\n", (int)(_BSD_COMPAT));
#endif
#ifdef SYSV
      printf("#define SYSV %iL\n", (int)(SYSV));
#endif
#ifdef _VR2_COMPAT_CODE
      printf("#define _VR2_COMPAT_CODE %iL\n", (int)(_VR2_COMPAT_CODE));
#endif
#ifdef _VR3_COMPAT_CODE
      printf("#define _VR3_COMPAT_CODE %iL\n", (int)(_VR3_COMPAT_CODE));
#endif
#ifdef __osf__
      printf("#define __osf__ %iL\n", (int)(__osf__));
#endif

#ifdef linux
      printf("#define linux %iL\n", (int)(linux));
#endif
#ifdef PPC64
      printf("#define PPC64 %iL\n", (int)(PPC64));
#endif


#ifdef MSDOS
      printf("#define MSDOS %iL\n", (int)(MSDOS));
#endif


#ifdef SYSTEM
      printf("#define SYSTEM %iL\n", (int)(SYSTEM));
#endif


#ifdef STDC
      printf("#define STDC %iL\n", (int)(STDC));
#endif
#ifdef __STDC__
      printf("#define __STDC__ %iL\n", (int)(__STDC__));
#endif
#ifdef POSIX
      printf("#define POSIX %iL\n", (int)(POSIX));
#endif
#ifdef __POSIX__
      printf("#define __POSIX__ %iL\n", (int)(__POSIX__));
#endif
#ifdef _POSIX_SOURCE
      /* printf("#define _POSIX_SOURCE %iL\n", (int)(_POSIX_SOURCE)); */
      printf("#define _POSIX_SOURCE\n");
#endif
#ifdef ANSI
      printf("#define ANSI %iL\n", (int)(ANSI));
#endif
#ifdef __ANSI__
      printf("#define __ANSI__ %iL\n", (int)(__ANSI__));
#endif
#ifdef __ANSI_CPP__
      printf("#define __ANSI_CPP__ %iL\n", (int)(__ANSI_CPP__));
#endif
#ifdef _MODERN_C
      printf("#define _MODERN_C %iL\n", (int)(_MODERN_C));
#endif
#ifdef __cplusplus
      printf("#define __cplusplus %iL\n", (int)(__cplusplus));
#endif


#ifdef mc68000
      printf("#define mc68000 %iL\n", (int)(mc68000));
#endif
#ifdef i286
      printf("#define i286 %iL\n", (int)(i286));
#endif
#ifdef i386
      printf("#define i386 %iL\n", (int)(i386));
#endif
#ifdef __i386
      printf("#define __i386 %iL\n", (int)(__i386));
#endif
#ifdef i486
      printf("#define i486 %iL\n", (int)(i486));
#endif
#ifdef i586
      printf("#define i586 %iL\n", (int)(i586));
#endif
#ifdef __i686__
      printf("#define __i686__ %iL\n", (int)(__i686__));
#endif
#ifdef __ia64__
      printf("#define __ia64__ %iL\n", (int)(__ia64__));
#endif


#ifdef __hp9000s700
      printf("#define __hp9000s700 %iL\n", (int)(__hp9000s700));
#endif
#ifdef  __hp9000s800
      printf("#define __hp9000s800 %iL\n", (int)(__hp9000s800));
#endif
#ifdef  __hppa
      printf("#define __hppa %iL\n", (int)(__hppa));
#endif
#ifdef  _PA_RISC1_1
      printf("#define _PA_RISC1_1 %iL\n", (int)(_PA_RISC1_1));
#endif
#ifdef  _HPUX_SOURCE
      printf("#define _HPUX_SOURCE %iL\n", (int)(_HPUX_SOURCE));
#endif


#ifdef  __LITTLE_ENDIAN
      printf("#define __LITTLE_ENDIAN %iL\n", (int)(__LITTLE_ENDIAN));
#endif
#ifdef  __BIG_ENDIAN
      printf("#define __BIG_ENDIAN %iL\n", (int)(__BIG_ENDIAN));
#endif
#ifdef  __ppc
      printf("#define __ppc %iL\n", (int)(__ppc));
#endif
#ifdef  __PRAGMA_REDEFINE_EXTNAME
      printf("#define __PRAGMA_REDEFINE_EXTNAME %iL\n", (int)(__PRAGMA_REDEFINE_EXTNAME));
#endif


#ifdef  sun4
      printf("#define sun4 %iL\n", (int)(sun4));
#endif
#ifdef  sun4cg92
      printf("#define sun4cg92 %iL\n", (int)(sun4cg92));
#endif
#ifdef  sun4u
      printf("#define sun4u %iL\n", (int)(sun4u));
#endif

#ifdef ibm
      printf("#define ibm %iL\n", (int)(ibm));
#endif

#ifdef sun
      printf("#define sun %iL\n", (int)(sun));
#endif
#ifdef _sun
      printf("#define _sun %iL\n", (int)(_sun));
#endif
#ifdef _sun_
      printf("#define _sun_ %iL\n", (int)(_sun_));
#endif
#ifdef __sun
      printf("#define __sun %iL\n", (int)(__sun));
#endif
#ifdef SUN
      printf("#define SUN %iL\n", (int)(SUN));
#endif
#ifdef _SUN
      printf("#define _SUN %iL\n", (int)(_SUN));
#endif
#ifdef _SUN_
      printf("#define _SUN_ %iL\n", (int)(_SUN_));
#endif
#ifdef __SUN
      printf("#define __SUN %iL\n", (int)(__SUN));
#endif
#ifdef sun2
      printf("#define sun2 %iL\n", (int)(sun2));
#endif
#ifdef sun3
      printf("#define sun3 %iL\n", (int)(sun3));
#endif
#ifdef sun4
      printf("#define sun4 %iL\n", (int)(sun4));
#endif
#ifdef _sun4
      printf("#define _sun4 %iL\n", (int)(_sun4));
#endif
#ifdef _sun4_
      printf("#define _sun4_ %iL\n", (int)(_sun4_));
#endif
#ifdef __sun4
      printf("#define __sun4 %iL\n", (int)(__sun4));
#endif
#ifdef SUN4
      printf("#define SUN4 %iL\n", (int)(SUN4));
#endif
#ifdef _SUN4
      printf("#define _SUN4 %iL\n", (int)(_SUN4));
#endif
#ifdef _SUN4_
      printf("#define _SUN4_ %iL\n", (int)(_SUN4_));
#endif
#ifdef __SUN4
      printf("#define __SUN4 %iL\n", (int)(__SUN4));
#endif
#ifdef sparc
      printf("#define sparc %iL\n", (int)(sparc));
#endif
#ifdef _sparc
      printf("#define _sparc %iL\n", (int)(_sparc));
#endif
#ifdef _sparc_
      printf("#define _sparc_ %iL\n", (int)(_sparc_));
#endif
#ifdef __sparc
      printf("#define __sparc %iL\n", (int)(__sparc));
#endif
#ifdef SPARC
      printf("#define SPARC %iL\n", (int)(SPARC));
#endif
#ifdef _SPARC
      printf("#define _SPARC %iL\n", (int)(_SPARC));
#endif
#ifdef _SPARC_
      printf("#define _SPARC_ %iL\n", (int)(_SPARC_));
#endif
#ifdef __SPARC
      printf("#define __SPARC %iL\n", (int)(__SPARC));
#endif
#ifdef sun5
      printf("#define sun5 %iL\n", (int)(sun5));
#endif
#ifdef cg92
      printf("#define cg92 %iL\n", (int)(cg92));
#endif
#ifdef _cg92
      printf("#define _cg92 %iL\n", (int)(_cg92));
#endif
#ifdef _cg92_
      printf("#define _cg92_ %iL\n", (int)(_cg92_));
#endif
#ifdef __cg92
      printf("#define __cg92 %iL\n", (int)(__cg92));
#endif
#ifdef CG92
      printf("#define CG92 %iL\n", (int)(CG92));
#endif
#ifdef _CG92
      printf("#define _CG92 %iL\n", (int)(_CG92));
#endif
#ifdef _CG92_
      printf("#define _CG92_ %iL\n", (int)(_CG92_));
#endif
#ifdef __CG92
      printf("#define __CG92 %iL\n", (int)(__CG92));
#endif
#ifdef __BUILTIN_VA_ARG_INCR
      printf("#define __BUILTIN_VA_ARG_INCR %iL\n", (int)(__BUILTIN_VA_ARG_INCR));
#endif
#ifdef _REENTRANT
      printf("#define _REENTRANT %iL\n", (int)(_REENTRANT));
#endif
#ifdef __SUNPRO_C
      printf("#define __SUNPRO_C %iL\n", (int)(__SUNPRO_C));
#endif
#ifdef __SVR4
      printf("#define __SVR4 %iL\n", (int)(__SVR4));
#endif
#ifdef SOLARIS
      printf("#define SOLARIS %iL\n", (int)(SOLARIS));
#endif


#ifdef convex
      printf("#define convex %iL\n", (int)(convex));
#endif
#ifdef _convex
      printf("#define _convex %iL\n", (int)(_convex));
#endif
#ifdef __convex
      printf("#define __convex %iL\n", (int)(__convex));
#endif
#ifdef _convex_
      printf("#define _convex_ %iL\n", (int)(_convex_));
#endif
#ifdef CONVEX
      printf("#define CONVEX %iL\n", (int)(CONVEX));
#endif
#ifdef _CONVEX
      printf("#define _CONVEX %iL\n", (int)(_CONVEX));
#endif
#ifdef __CONVEX
      printf("#define __CONVEX %iL\n", (int)(__CONVEX));
#endif
#ifdef _CONVEX_
      printf("#define _CONVEX_ %iL\n", (int)(_CONVEX_));
#endif
#ifdef _CONVEX_SOURCE
      printf("#define _CONVEX_SOURCE %iL\n", (int)(_CONVEX_SOURCE));
#endif
#ifdef __convex__
      printf("#define __convex__ %iL\n", (int)(__convex__));
#endif


#ifdef HP_UX
      printf("#define HP_UX %iL\n", (int)(HP_UX));
#endif
#ifdef HPUX
      printf("#define HPUX %iL\n", (int)(HPUX));
#endif
#ifdef _HPUX
      printf("#define _HPUX %iL\n", (int)(_HPUX));
#endif
#ifdef __HPUX
      printf("#define __HPUX %iL\n", (int)(__HPUX));
#endif
#ifdef _HPUX_
      printf("#define _HPUX_ %iL\n", (int)(_HPUX_));
#endif
#ifdef hpux
      printf("#define hpux %iL\n", (int)(hpux));
#endif
#ifdef _hpux
      printf("#define _hpux %iL\n", (int)(_hpux));
#endif
#ifdef __hpux
      printf("#define __hpux %iL\n", (int)(__hpux));
#endif
#ifdef _hpux_
      printf("#define _hpux_ %iL\n", (int)(_hpux_));
#endif


#ifdef  IRIX32
      printf("#define IRIX32 %iL\n", (int)(IRIX32));
#endif
#ifdef  IRIX64
      printf("#define IRIX64 %iL\n", (int)(IRIX64));
#endif
#ifdef SGI
      printf("#define SGI %iL\n", (int)(SGI));
#endif
#ifdef _SGI
      printf("#define _SGI %iL\n", (int)(_SGI));
#endif
#ifdef __SGI
      printf("#define __SGI %iL\n", (int)(__SGI));
#endif
#ifdef _SGI_
      printf("#define _SGI_ %iL\n", (int)(_SGI_));
#endif
#ifdef sgi
      printf("#define sgi %iL\n", (int)(sgi));
#endif
#ifdef _sgi
      printf("#define _sgi %iL\n", (int)(_sgi));
#endif
#ifdef __sgi
      printf("#define __sgi %iL\n", (int)(__sgi));
#endif
#ifdef _sgi_
      printf("#define _sgi_ %iL\n", (int)(_sgi_));
#endif
#ifdef MIPS
      printf("#define MIPS %iL\n", (int)(MIPS));
#endif
#ifdef _MIPS
      printf("#define _MIPS %iL\n", (int)(_MIPS));
#endif
#ifdef __MIPS
      printf("#define __MIPS %iL\n", (int)(__MIPS));
#endif
#ifdef _MIPS_
      printf("#define _MIPS_ %iL\n", (int)(_MIPS_));
#endif
#ifdef mips
      printf("#define mips %iL\n", (int)(mips));
#endif
#ifdef _mips
      printf("#define _mips %iL\n", (int)(_mips));
#endif
#ifdef __mips
      printf("#define __mips %iL\n", (int)(__mips));
#endif
#ifdef _mips_
      printf("#define _mips_ %iL\n", (int)(_mips_));
#endif
#ifdef MIPSEL
      printf("#define MIPSEL %iL\n", (int)(MIPSEL));
#endif
#ifdef _MIPSEL
      printf("#define _MIPSEL %iL\n", (int)(_MIPSEL));
#endif
#ifdef __MIPSEL
      printf("#define __MIPSEL %iL\n", (int)(__MIPSEL));
#endif
#ifdef _MIPSEL_
      printf("#define _MIPSEL_ %iL\n", (int)(_MIPSEL_));
#endif
#ifdef mipsel
      printf("#define mipsel %iL\n", (int)(mipsel));
#endif
#ifdef _mipsel
      printf("#define _mipsel %iL\n", (int)(_mipsel));
#endif
#ifdef __mipsel
      printf("#define __mipsel %iL\n", (int)(__mipsel));
#endif
#ifdef _mipsel_
      printf("#define _mipsel_ %iL\n", (int)(_mipsel_));
#endif



      printf("\n- HOST defines -\n");
#ifdef HOST_Sun   
      printf("#define HOST_Sun %iL\n", (int)(HOST_Sun));
#endif
#ifdef HOST_sun4  
      printf("#define HOST_sun4 %iL\n", (int)(HOST_sun4));
#endif
#ifdef HOST_sun4cg92
      printf("#define HOST_sun4cg92 %iL\n", (int)(HOST_sun4cg92));
#endif
#ifdef HOST_i86pc 
      printf("#define HOST_i86pc %iL\n", (int)(HOST_i86pc));
#endif
#ifdef HOST_SGI   
      printf("#define HOST_SGI %iL\n", (int)(HOST_SGI));
#endif
#ifdef HOST_Linux 
      printf("#define HOST_Linux %iL\n", (int)(HOST_Linux));
#endif
#ifdef HOST_IRIX32
      printf("#define HOST_IRIX32 %iL\n", (int)(HOST_IRIX32));
#endif
#ifdef HOST_IRIX64   
      printf("#define HOST_IRIX64 %iL\n", (int)(HOST_IRIX64));
#endif
#ifdef HOST_SWAP   
      printf("#define HOST_SWAP %iL\n", (int)(HOST_SWAP));
#endif

#if HOST_INT_DDS == DDS_INTEGER4
   printf("HOST_INT_DDS == DDS_INTEGER4\n");
#elif HOST_INT_DDS == DDS_INTEGER4X
   printf("HOST_INT_DDS == DDS_INTEGER4X\n");
#elif HOST_INT_DDS == DDS_INTEGER8
   printf("HOST_INT_DDS == DDS_INTEGER8\n");
#elif HOST_INT_DDS == DDS_INTEGER8X
   printf("HOST_INT_DDS == DDS_INTEGER8X\n");
#endif

#if HOST_LONG_DDS == DDS_INTEGER4
   printf("HOST_LONG_DDS == DDS_INTEGER4\n");
#elif HOST_LONG_DDS == DDS_INTEGER4X
   printf("HOST_LONG_DDS == DDS_INTEGER4X\n");
#elif HOST_LONG_DDS == DDS_INTEGER8
   printf("HOST_LONG_DDS == DDS_INTEGER8\n");
#elif HOST_LONG_DDS == DDS_INTEGER8X
   printf("HOST_LONG_DDS == DDS_INTEGER8X\n");
#endif

#if HOST_LLONG_DDS == DDS_INTEGER8
   printf("HOST_LLONG_DDS == DDS_INTEGER8\n");
#elif HOST_LLONG_DDS == DDS_INTEGER8X
   printf("HOST_LLONG_DDS == DDS_INTEGER8X\n");
#endif

printf("HOST_LONG_P = %d\n", HOST_LONG_P);

   return 0;
}
