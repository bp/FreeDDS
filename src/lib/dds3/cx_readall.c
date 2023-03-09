static char rcsid[]="$Id: cx_readall.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, convenience read entire dataset

*/

/***************************************/
/***   Copyright (C) 2006            ***/
/***   BP America, Inc.              ***/
/***   Jerry Ehlers, August, 2006    ***/
/***************************************/

#include "dds_sys.h"

int cddx_readall(
   char *alias,
   void **buffer,
   int *rank,
   int *size,
   float *delta, 
   float *origin)
{
   int i, n_read, ntr;
   char str[32];
   size_t sizet;
   BIN_TAG bin_tag;

   DDS_API_ENTRY("xreadall")

   /*****************************/
   /* validate input parameters */
   /*****************************/

   /* Open the file */
   bin_tag = cddx_in(alias, "", "cddx_readall");
   if (bin_tag < 0) {
      DDS_API_EXIT(bin_tag);
   }

   /* Read the parameters */
   *rank = cdds_scank("axis","");
   if (*rank > RANK_MAX) *rank = RANK_MAX;

   for (i=0;i<*rank;i++) {
      sprintf(str, "size.axis(%d)", i+1);
      cdds_scanf(str, "%d", &size[i]);

      sprintf(str, "delta.axis(%d)", i+1);
      cdds_scanf(str, "%f", &delta[i]);

      sprintf(str, "origin.axis(%d)", i+1);
      cdds_scanf(str, "%f", &origin[i]);
   }

   for (i=*rank;i<RANK_MAX;i++) {
      size[i] = 1;
      delta[i] = 1.0;
      origin[i] = 0.0;
   }

   /* Allocate the buffer */
   ntr = cdds_axis_prod(2);
   sizet = ntr;
   sizet = sizet * cdds_prec(bin_tag, 0);
   *buffer = (void*)cdds_malloc(sizet);

   /* Read the data */
   n_read = cddx_read(bin_tag, *buffer, ntr);
   if (n_read != ntr) {
      cdds_close(bin_tag);
      DDS_API_EXIT(-3);
   }

   DDS_API_EXIT(bin_tag);
}
