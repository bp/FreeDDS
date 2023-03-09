static char rcsid[]="$Id: lnblnk.c 50 2009-02-10 21:29:01Z user $";
/*
RCS-t=HOST, Fortran API, To provide "lnblnk" function on systems that don't provide it.
*/

#include "chost.h"

#if defined(__ia64__) || defined(ppc64)
/*! Fortran API, To provide "lnblnk" function on systems that don't provide it. */
HOST_FTN_INT HOST_FTN_FUNC(lnblnk, LNBLNK) ( 
   HOST_FTN_STR str,
   HOST_FTN_INT len)
{ 
   int i;
   for(i=len-1;i>0;i--) if (str[i] != ' ') break;
   return(i+1); 
}
#endif
