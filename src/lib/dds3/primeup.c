static char rcsid[]="$Id: primeup.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, return prime number >= num.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

size_t ddsPrimeUp(
  size_t num)
{
   DDS_BOOLEAN prime;
   size_t div;

   if(num <= 1) return 1;

   prime = DDS_TRUE;
   while(DDS_TRUE) {
      for(div = 2, prime = DDS_TRUE; (div * div) <= num; div++) {
         if(num % div == 0) {
            prime = DDS_FALSE;
            break;
         }
      }
      if(prime) break;
      num++;
   }
   return num;
}
