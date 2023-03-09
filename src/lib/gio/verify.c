static char rcsid[]="$Id: verify.c 13 2009-01-06 16:56:43Z user $";
/*
verify, verify buffer contents

*/

#include "tstsys.h"

HOST_FTN_INT HOST_FTN_FUNC(verify, VERIFY)(
   int *buf,
   int *ubuf,
   HOST_LLONG_T *lloffset,
   int *ibytes)
{
   unsigned char *expect, *actual;
   int j, n_start, n_stop;

   fprintf(stderr, "verify: VERIFY ERROR, offset = %lu + ",
      (unsigned long)*lloffset);
   expect = (unsigned char*)ubuf;
   actual = (unsigned char*)buf;
   for(j = 0; j < *ibytes; j++) {
      if(expect[j] != actual[j]) {
         fprintf(stderr, "%i\n", j);

         n_start = j;
         n_stop = (j + 24 < *ibytes) ? (j + 24) : *ibytes;

         fprintf(stderr, "   offset%%10= ");
         for(j = n_start; j < n_stop; j++)
            fprintf(stderr, " %i", (int)((*lloffset + j) % 10));
         fprintf(stderr, "\n");

         fprintf(stderr, "   expected=  ");
         for(j = n_start; j < n_stop; j++)
            fprintf(stderr, "%02x", (int)expect[j]);
         fprintf(stderr, "\n");

         fprintf(stderr, "   actual=    ");
         for(j = n_start; j < n_stop; j++)
            fprintf(stderr, "%02x", (int)actual[j]);
         fprintf(stderr, "\n");

         return 36;
      }
   }
   return 36;
}
