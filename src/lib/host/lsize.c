static char rcsid[]="$Id: lsize.c 50 2009-02-10 21:29:01Z user $";
/*
RCS-t=HOST, C routine to go with Ftest1.F

*/

#include "chost.h"


/*! C routine to go with Ftest1.F */
HOST_FTN_INT HOST_FTN_FUNC(lsize, LSIZE) (
   HOST_FTN_INT *l1,
   HOST_FTN_INT *l2)
{
   int size;

   size = (int)(l2 - l1);
   return(size);
}

