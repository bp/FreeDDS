static char rcsid[]="$Id: c_check.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, check debug flags

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_DEBUG (cdds_check)(
   DDS_DEBUG new_flag)
{
   DDS_DEBUG old_flag;

   DDS_API_ENTRY("check")

   new_flag = (DDS_DEBUG)((~ DDS_DEBUG_INIT) & (new_flag | dds_debug_user));
   old_flag = dds_debug_api;

   if(new_flag & DDS_DBG_HEAP) {
      /* one-time only check of heap integrity */
      ddsHeapCheck(NULL);
   } else {
      if(new_flag & DDS_DBG_SCANNER) {
         dds_yy_flex_debug = 1;
      } else {
         dds_yy_flex_debug = 0;
      }
      if(new_flag & (DDS_DBG_ALLOC | DDS_DBG_FREE | DDS_DBG_CALL)) {
         ddsHeapCheck(NULL);
      }
      dds_debug_api = new_flag;
   }

   DDS_API_EXIT(old_flag)
}
