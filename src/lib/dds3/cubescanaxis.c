static char rcsid[]="$Id: cubescanaxis.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, retrieve and default cube description

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

void ddsCubeScanAxis(
   DDS_AXIS *axis,
   DDS_STRING *alias)
{
   long i;
   float f;
   double del;
   const char *name;

   /************************************************/
   /* Initialize flag, size, origin, delta, units, */
   /* base, step, sort_name and grid.              */
   /************************************************/

   ddsStrCpyCat2(alias, "size.", axis->name);
   if(1 == cdds_scanf(alias->ptr, "%ld",  &axis->size)) {
      axis->flag |= DDS_AXIS_size;
   }
 
   ddsStrCpyCat2(alias, "origin.", axis->name);
   if(1 == cdds_scanf(alias->ptr, "%lf",  &axis->origin)) {
      axis->flag |= DDS_AXIS_origin;
   }
 
   ddsStrCpyCat2(alias, "delta.", axis->name);
   if(1 == cdds_scanf(alias->ptr, "%lf",  &axis->delta) &&
      axis->delta) {
      axis->flag |= DDS_AXIS_delta;
   }
 
   ddsStrCpyCat2(alias, "units.", axis->name);
   if(0 <= cdds_scant(alias->ptr, &name)) {
      axis->units = ddsString(name);
   }
 
   ddsStrCpyCat2(alias, "base.", axis->name);
   if(1 == cdds_scanf(alias->ptr, "%lf",  &axis->base)) {
      axis->flag |= DDS_AXIS_base;
   }
 
   ddsStrCpyCat2(alias, "step.", axis->name);
   if(1 == cdds_scanf(alias->ptr, "%lf",  &axis->step) &&
      axis->step) {
      axis->flag |= DDS_AXIS_step;
   }
 
   ddsStrCpyCat2(alias, "sort.", axis->name);
   if (0 <= cdds_scant(alias->ptr, &name)) {
      axis->sort_name = ddsString(name);
   }
 
   ddsStrCpyCat2(alias, "grid.", axis->name);
   axis->ngrid = (long)cdds_scank(alias->ptr, "");
   if (axis->ngrid > 1) {
      axis->grid = (double*)ddsMalloc(axis->ngrid*sizeof(double));
      cdds_scanf(alias->ptr, "%f", &f);
      axis->grid[0] = f;
      for(i=1;i<axis->ngrid;i++) {
         cdds_scanf("", "%f", &f);
         axis->grid[i] = f;
         del = (axis->grid[i]-axis->grid[i-1])*(axis->grid[1]-axis->grid[0]);
         if (del <= 0.0 && ! (axis->flag & DDS_AXIS_SPARSE)) {
            ddsError("%s= not monotonically increasing or decreasing\n", 
                     alias->ptr);
            ddsFree((void*)axis->grid);
            axis->ngrid = 0;
            axis->grid = (double*)0;
            break;
         }
      }
   } else {
       if (axis->ngrid >= 0 && ! (axis->flag & DDS_AXIS_SPARSE)) {
         ddsError("need at least 2 values if using %s= with 'dense.%s'\n", 
                  alias->ptr, axis->name);
      }
      axis->ngrid = 0;
      axis->grid = (double*)0;
   }
 
   return;
}
