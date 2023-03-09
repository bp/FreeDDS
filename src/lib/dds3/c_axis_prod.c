static char rcsid[]="$Id: c_axis_prod.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, query axis product 

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int cdds_axis_prod(
   int rank)
{
   /* domain of data cube, relative to rank n (axis)
      example rank=  1  2  3   4
      example size=  3  5  7  11

         axis_prod(1)=  1 * 11 * 7 * 5 * 3
         axis_prod(2)=  1 * 11 * 7 * 5
         axis_prod(3)=  1 * 11 * 7
         axis_prod(4)=  1 * 11
         axis_prod(5+)= 1
   */
   int size, limit;
   double axis_prod;
   char size_name[DEFNNAME_MAX+1];

   DDS_API_ENTRY("axis_prod")

   axis_prod = 1;

   if(rank > 0) {
      if(! cdds_scanf("axis", "")) {
         limit = cdds_key("");
         while(rank <= limit && rank <= RANK_MAX) {
            sprintf(size_name, "size.axis(%i)", rank);
            size = 1;
            cdds_scanf(size_name, "%d", &size);
            if(size > 0) axis_prod *= size;
            rank++;
         }
      }
   } else {
      ddsDebug("axis_prod: bad calling argument, rank= %i\n", rank);
   }
   DDS_API_EXIT((int)axis_prod)
}
