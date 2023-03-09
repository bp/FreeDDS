static char rcsid[]="$Id: makefdds.c 169 2010-08-04 18:15:15Z user $";
/*
RCS-t=DDS main, make fdds.h or f90dds.h from f_com.h or f90_com.h 

*/

/***************************************/
/***   Copyright (C) 2005            ***/
/***   BP America, Houston, TX       ***/
/***   Jerry Ehlers, June 2005       ***/
/***************************************/

#include <stdio.h>
#include <string.h>
#include "chost.h"

int main(int argc, char **argv)
{
   int size;

   if (argc < 1 || (strcmp(argv[1],"-f") && strcmp(argv[1],"-f90"))) {
      fprintf(stderr, "*** ERROR(makefdds): Must use '-f' or '-f90'\n");
      return 1;
   }

   size = sizeof(void*);
   if (size != 4 && size != 8) {
      fprintf(stderr, "*** ERROR(makefdds): size=%d'\n", size);
      return 1;
   }
   
   if (!strcmp(argv[1],"-f")) {
      /* Append to f_com.h */
      printf("\n");
      printf("c The following added automatically by 'makefdds'\n");
#ifdef HOST_INT8_C
      printf("\n");
      printf("      integer*8 fdds_lseek8\n");
      printf("      external fdds_lseek8\n");
#endif
      printf("\n");
      printf("      integer*%d\n", size);
      printf("     :   fdds_malloc, fdds_realloc, fdds_calloc, fdds_ptr,\n");
      printf("     :   fdds_malloc8, fdds_realloc8, fdds_calloc8\n");
      printf("      external\n");
      printf("     :   fdds_malloc, fdds_realloc, fdds_calloc, fdds_ptr,\n");
      printf("     :   fdds_malloc8, fdds_realloc8, fdds_calloc8\n");
   } else {
      /* Append to f90_com.h */
      printf("\n");
      printf("! The following added automatically by 'makefdds'\n");
#ifdef HOST_INT8_C
      printf("\n");
      printf("      integer (kind=INT8), external :: &\n");
      printf("         fdds_lseek8\n");
#endif
      printf("\n");
      printf("      integer (kind=INT%d), external :: &\n", size);
      printf("         fdds_malloc, fdds_realloc, fdds_calloc, fdds_ptr, &\n");
      printf("         fdds_malloc8, fdds_realloc8, fdds_calloc8\n");
   }
   printf("\n");
  
   return 0;
}
