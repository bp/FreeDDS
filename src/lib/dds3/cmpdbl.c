static char rcsid[]="$Id: cmpdbl.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, compare double floats, using epsilon precision
 
*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/
 
#include "dds_sys.h"
 

#include <math.h>
#ifdef sun
/*** <float.h> yields bad fabs values on the sun4 acc ??? ***/
#define FLT_EPSILON 0.00001
#else
#include <float.h>
#endif

int ddsCmpDbl(
   double x,
   double y)
{
   double percent, epsilon = 24 * FLT_EPSILON;

   if(x == y) return 0;   /* x == y */
   percent = (x - y) / (fabs(x) + fabs(y));

   if(percent > +epsilon) return +1;	/* x >>> y */
   if(percent < -epsilon) return -1;	/* x <<< y */
   return 0;   /* x === y */
}
