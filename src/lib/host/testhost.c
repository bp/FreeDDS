static char rcsid[]="$Id: testhost.c 50 2009-02-10 21:29:01Z user $";
/*
RCS-t=HOST main, make chost.h 

*/

/***************************************/
/***   Copyright (C) 2009            ***/
/***   BP America, Houston, TX       ***/
/***   Jerry Ehlers, January 2009    ***/
/***************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <chost.h>

#define MAX6 7
#define BIG_MIN (LONG_MIN / 10)
#define BIG_MAX (LONG_MAX / 10)
#define RANGE   64

/*! TEST Program: tests for consistency of chost.h */
int main(int argc, char **argv)
{
   int error=0, m;
   HOST_INT6_T int6_out, int6_in;
   HOST_LLONG_T llong_out, llong_in;
   HOST_LLONG_T i6, j6, ij6;
   HOST_LLONG_T min_max[MAX6][4] = {
         {-RANGE, +RANGE, -RANGE, +RANGE},
         {BIG_MIN, BIG_MIN + RANGE, -RANGE, +RANGE},
         {-RANGE, +RANGE, BIG_MIN, BIG_MIN + RANGE},
         {BIG_MAX - RANGE, BIG_MAX, -RANGE, +RANGE},
         {-RANGE, +RANGE, BIG_MAX - RANGE, BIG_MAX},
         {BIG_MIN, BIG_MIN + RANGE, BIG_MAX - RANGE, BIG_MAX},
         {BIG_MAX - RANGE, BIG_MAX, BIG_MIN, BIG_MIN + RANGE}
      };

   for(m = 0; m < MAX6; m++) {
      for(i6 = min_max[m][0]; i6 <= min_max[m][1]; i6++) {
         for(j6 = min_max[m][2]; j6 <= min_max[m][3]; j6++) {
            ij6 = (HOST_INT6_T)i6 + (HOST_INT6_T)j6;
            if((i6 + j6) != ij6) {
               error++;
               fprintf(stderr,"FAKE8 failed '+',"
                 " i6= " HOST_LLONG_QP
                 " j6= " HOST_LLONG_QP
                 " j6= " HOST_LLONG_QP
                 "\n", i6, j6, ij6);
            }

            ij6 = (HOST_INT6_T)i6 - (HOST_INT6_T)j6;
            if((i6 - j6) != ij6) {
               error++;
               fprintf(stderr,"FAKE8 failed '-',"
                 " i6= " HOST_LLONG_QP
                 " j6= " HOST_LLONG_QP
                 " j6= " HOST_LLONG_QP
                 "\n", i6, j6, ij6);
            }

            if(i6 < LONG_MAX / (10 * RANGE) && i6 > LONG_MIN / (10 * RANGE) &&
               j6 < LONG_MAX / (10 * RANGE) && j6 > LONG_MIN / (10 * RANGE)) {
               ij6 = (HOST_INT6_T)i6 * (HOST_INT6_T)j6;
               if((i6 * j6) != ij6) {
                  error++;
                  fprintf(stderr,"FAKE8 failed '*',"
                    " i6= " HOST_LLONG_QP
                    " j6= " HOST_LLONG_QP
                    " j6= " HOST_LLONG_QP
                    "\n", i6, j6, ij6);
               }
            }

            if(j6) {
               ij6 = HOST_INT6_DIV(i6, j6);
               if((i6 / j6) != ij6) {
                  error++;
                  fprintf(stderr,"FAKE8 failed '/',"
                    " i6= " HOST_LLONG_QP
                    " j6= " HOST_LLONG_QP
                    " j6= " HOST_LLONG_QP
                    "\n", i6, j6, ij6);
               }

               ij6 = HOST_INT6_MOD(i6, j6);
               if((i6 % j6) != ij6) {
                  error++;
                  fprintf(stderr,"FAKE8 failed '%%',"
                    " i6= " HOST_LLONG_QP
                    " j6= " HOST_LLONG_QP
                    " j6= " HOST_LLONG_QP
                    "\n", i6, j6, ij6);
               }
            }
         }
      }
   }

   fprintf(stderr, "\n# of FAKE8 errors = %d\n\n", error);

   return 0;
}
