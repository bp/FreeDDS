static char rcsid[]="$Id: mediascan.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, retrieve specs for (tape) drive/media 
 
*/
 
/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/
 
#include <limits.h>
#include "dds_sys.h"

void ddsMediaScan(
   const char *alias,
   DDS_MEDIA *media)
{
   const char *value;

   if(0 < cdds_scank(alias, "gap"))
      cdds_scanf("", " %*s %f", &media->gap);
   if(0 < cdds_scank(alias, "duty"))
      cdds_scanf("", " %*s %f", &media->duty);

   if(0 < cdds_scank(alias, "density")) {
      cdds_scanf("", " %*s %d", &media->density);
      if(! media->density) media->density = -1;
   }
   if(0 < cdds_scank(alias, "sectors"))
      cdds_scanf("", " %*s %d", &media->sectors);
   if(0 < cdds_scank(alias, "tracks"))
      cdds_scanf("", " %*s %d", &media->tracks);
   if(0 < cdds_scank(alias, "cylinders"))
      cdds_scanf("", " %*s %d", &media->cylinders);
   if(0 < cdds_scank(alias, "passes"))
      cdds_scanf("", " %*s %d", &media->passes);
   if(0 < cdds_scank(alias, "length")) {
      value = "";
      if(1 == cdds_scanf("", " %*s %d", &media->length))
         cdds_scant("", &value);
      if(! strcmp("meter", value) || ! strcmp("meters", value))
         media->length *= 3.2808;
   }

   if(0 < cdds_scank(alias, "min_block")) {
      value = "";
      if(1 == cdds_scanf("", " %*s %d", &media->min_block))
         cdds_scant("", &value);
      if(! strcmp("k", value) || ! strcmp("K", value))
         media->min_block *= DDS_KB;
      else if(! strcmp("m", value) || ! strcmp("M", value))
         media->min_block *= DDS_MB;
   }
   if(0 < cdds_scank(alias, "max_block")) {
      value = "";
      if(1 == cdds_scanf("", " %*s %d", &media->max_block))
         cdds_scant("", &value);
      if(! strcmp("k", value) || ! strcmp("K", value))
         media->max_block *= DDS_KB;
      else if(! strcmp("m", value) || ! strcmp("M", value))
         media->max_block *= DDS_MB;
   }
   if(0 < cdds_scank(alias, "opt_block")) {
      value = "";
      if(1 == cdds_scanf("", " %*s %d", &media->opt_block))
         cdds_scant("", &value);
      if(! strcmp("k", value) || ! strcmp("K", value))
         media->opt_block *= DDS_KB;
      else if(! strcmp("m", value) || ! strcmp("M", value))
         media->opt_block *= DDS_MB;
   }
   if(0 < cdds_scank(alias, "rate")) {
      value = "";
      if(1 == cdds_scanf("", " %*s %f", &media->rate))
         cdds_scant("", &value);
      if(! strcmp("k", value) || ! strcmp("K", value))
         media->rate *= DDS_KB;
      else if(! strcmp("m", value) || ! strcmp("M", value))
         media->rate *= DDS_MB;
      if(media->rate == 0.0) media->rate = 0.1e+6;
   }
   if(0 < cdds_scank(alias, "capacity")) {
      value = "";
      if(1 == cdds_scanf("", " %*s %f", &media->capacity))
         cdds_scant("", &value);
      if(! strcmp("k", value) || ! strcmp("K", value))
         media->capacity *= DDS_KB;
      else if(! strcmp("m", value) || ! strcmp("M", value))
         media->capacity *= DDS_MB;
      else if(! strcmp("g", value) || ! strcmp("G", value))
         media->capacity *= DDS_GB;
      if(media->capacity == 0.0) media->capacity = DDS_MB;
   }

   return;
}
