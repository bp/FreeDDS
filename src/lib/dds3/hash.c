static char rcsid[]="$Id: hash.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, compute hash key, given name string.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

#define HASH_SCHEME  WALL_1

/* Alfred V. Aho, "Compilers: Principles, Techniques and Tools" */
#define AHO_1           1
#define AHO_2           2
#define AHO_3           3

/* Larry Wall, "Perl" */
#define WALL_1          4

/* Allen I. Holub, "Compiler Design in C" */
#define HOLUB_1         5


#if (HASH_SCHEME == WALL_1)
   /***
   static DDS_HASH random_primes[16] = {
           39769,  5857,   78167,  73529,
           25243,  11299,  17483,  797,
           95383,  23747,  79357,  61463,
           45137,  55337,  4271,   20551};
   static DDS_HASH random_primes[16] = {
           94598,  82601,   46447,  49927,
           58027,  77711,  17483,  32443,
           95383,  23747,  79357,  61463,
           45137,  55337,  4271,   20551};
   static DDS_HASH random_primes[16] = {
           69877,  82602,   46447,  49927,
           58027,  77711,  17483,  32443,
           95383,  23747,  79357,  61463,
           45137,  55337,  4271,   20551};
   ***/
   static DDS_HASH random_primes[16] = {
           82014,  40927,   46447,  49927,
           58027,  77711,  17483,  32443,
           95383,  23747,  79357,  61463,
           45137,  55337,  4271,   20551};
#endif /* (HASH_SCHEME == WALL_1) */

DDS_HASH ddsHash(
   const char *name)
{
   #if (HASH_SCHEME == AHO_1)
      #define ALPHA 65599 /* prime */
         const unsigned char *p;
         unsigned h = 0;
         for(p = (const unsigned char *)name; *p; p++)
            h = ALPHA * h + *p;
         return h;
   #endif /* (HASH_SCHEME == AHO_1) */

   #if (HASH_SCHEME == AHO_2)
      #define ALPHA 65599 /* prime */
         const unsigned char *p;
         unsigned h = 0;
         for(p = (const unsigned char *)name; *p; p++)
            h = ALPHA * h ^ *p;
         return h;
   #endif /* (HASH_SCHEME == AHO_2) */

   #if (HASH_SCHEME == AHO_3)
      /* hash_pjw scheme (assumes 32 bit unsigned word ?) */
      const unsigned char *p;
      unsigned h = 0, g;

      for(p = (const unsigned char *)name; *p; p++) {
         h = (h << 4) + *p;
         g = h & 0xf0000000;
         if(g) {
            h = h ^ (g >> 24);
            h = h ^ g;
         }
      }
      return h;
   #endif /* (HASH_SCHEME == AHO_3) */

   #if (HASH_SCHEME == WALL_1)
      DDS_HASH hash, prime;
      const unsigned char *p;
   
      hash =  95273;
      for(p = (const unsigned char *)name; *p; p++) {
         prime = random_primes[hash & 15];
         hash += prime + prime * (*p);
      }
      return hash;
   #endif /* (HASH_SCHEME == WALL_1) */

}
