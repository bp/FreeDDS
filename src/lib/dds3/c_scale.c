static char rcsid[]="$Id: c_scale.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, compute scale factor, given output and input units.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

double cdds_scale(
   const char *out,
   const char *in)
{
   char str[8];
   float scale_out, scale_in;
   double scale;
   /* this should really be done by the unix units command */

   DDS_API_ENTRY("scale")

   if(! strcmp(out, "sec")) {
      if(! strcmp(in, "sec")) {
         scale = 1.;
      } else if(! strcmp(in, "msec")) {
         scale = 1.e-3;
      } else if(! strcmp(in, "usec")) {
         scale = 1.e-6;
      } else if(! strcmp(in, "nsec")) {
         scale = 1.e-9;
      } else if(2 == sscanf(in, "%f%7s", &scale_in, str) &&
         ! strcmp(str, "sec")) {
         scale = scale_in;
      } else {
         scale = 0.;
      }
   } else if(! strcmp(out, "msec")) {
      if(! strcmp(in, "sec")) {
         scale = 1.e+3;
      } else if(! strcmp(in, "msec")) {
         scale = 1.;
      } else if(! strcmp(in, "usec")) {
         scale = 1.e-3;
      } else if(! strcmp(in, "nsec")) {
         scale = 1.e-6;
      } else if(2 == sscanf(in, "%f%7s", &scale_in, str) &&
         ! strcmp(str, "sec")) {
         scale = 1.e+3 * scale_in;
      } else {
         scale = 0.;
      }
   } else if(! strcmp(out, "usec")) {
      if(! strcmp(in, "sec")) {
         scale = 1.e+6;
      } else if(! strcmp(in, "msec")) {
         scale = 1.e+3;
      } else if(! strcmp(in, "usec")) {
         scale = 1.;
      } else if(! strcmp(in, "nsec")) {
         scale = 1.e-3;
      } else if(2 == sscanf(in, "%f%7s", &scale_in, str) &&
         ! strcmp(str, "sec")) {
         scale = 1.e+6 * scale_in;
      } else {
         scale = 0.;
      }
   } else if(! strcmp(out, "nsec")) {
      if(! strcmp(in, "sec")) {
         scale = 1.e+9;
      } else if(! strcmp(in, "msec")) {
         scale = 1.e+6;
      } else if(! strcmp(in, "usec")) {
         scale = 1.e+3;
      } else if(! strcmp(in, "nsec")) {
         scale = 1.;
      } else if(2 == sscanf(in, "%f%7s", &scale_in, str) &&
         ! strcmp(str, "sec")) {
         scale = 1.e+9 * scale_in;
      } else {
         scale = 0.;
      }
   } else if(2 == sscanf(out, "%f%7s", &scale_out, str) &&
         ! strcmp(str, "sec") && scale_out) {
      if(! strcmp(in, "sec")) {
         scale = 1.0 / scale_out;
      } else if(! strcmp(in, "msec")) {
         scale = 1.e+3 / scale_out;
      } else if(! strcmp(in, "usec")) {
         scale = 1.e+6 / scale_out;
      } else if(! strcmp(in, "nsec")) {
         scale = 1.e+9 / scale_out;
      } else if(2 == sscanf(in, "%f%7s", &scale_in, str) &&
         ! strcmp(str, "sec")) {
         scale = scale_in / scale_out;
      } else {
         scale = 0.;
      }
   } else if(! strcmp(out, "feet")) {
      if(! strcmp(in, "feet")) {
         scale = 1.;
      } else if(! strcmp(in, "meters")) {
         scale = FEET_PER_METER;
      } else {
         scale = 0.;
      }
   } else if(! strcmp(out, "meters")) {
      if(! strcmp(in, "feet")) {
         scale = METER_PER_FOOT;
      } else if(! strcmp(in, "meters")) {
         scale = 1.;
      } else {
         scale = 0.;
      }
   } else if(! strcmp(out, "?") && ! strcmp(in,  "usp_xy_units?")) {
      /* quietly ignore both unknowns */
      scale = 1.;
   } else if(! strcmp(in, "?")  && ! strcmp(out, "usp_xy_units?")) {
      /* quietly ignore both unknowns */
      scale = 1.;
   } else if(! strcmp(out, in)) {
      scale = 1.;
   } else if(! strcmp(out, "?") || ! strcmp(in, "?")) {
      scale = 1.;
   } else {
      scale = 0.;
   }

   DDS_API_EXIT(scale)
}
