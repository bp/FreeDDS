static char rcsid[]="$Id: c_history.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, copy all scan dictionaries to print dictionary

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int cdds_history(
   void)
{
   int status = DDS_OK;
   DDS_DICT *dict_old;

   DDS_API_ENTRY("history")

   if(dds_print_first) {
      for(dict_old = dds_scan_last;
         dict_old;
         dict_old = dict_old->left) {

         /* start of scan dictionary contents */
         cdds_printv("dds_history{", " ");
         cdds_printv("", dict_old->name);
         cdds_printv("", "\n");

         if(ddsHistory(dds_print_first, dict_old)) status = DDS_BAD;

         /* end of scan dictionary contents */
         cdds_printv("dds_history}", " ");
         cdds_printv("", dict_old->name);
         cdds_printv("", "\n");
      }
   } else {
      status = EOF;
   }

   DDS_API_EXIT(status)
}
