static char rcsid[]="$Id: scandone.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, de-select current scan dict and defn

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int ddsScanDone(
   void)
{
   const DDS_DEFN *newer_defn;

   if(dds_scan_dict) {
      /* restore current scan definition */
      newer_defn = (DDS_DEFN*)dds_scan_defn->newer;
      dds_scan_dict->buf.ptr[newer_defn->start] =
         dds_scan_value_save;
      dds_scan_dict = NULL;
   }

   return DDS_OK;
}
