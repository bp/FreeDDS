static char rcsid[]="$Id: dumppage.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, dump DDS_PAGE

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

void ddsDumpPage(
   int indent,
   DDS_PAGE *page)
{
   if(! dds_initialized) return; /* no-op */

   if(! page) {
      fprintf(dds_print_fp, "ddsDumpPage: NULL PAGE ptr\n");
      return;
   }

   if(((DDS_PAGE*)page)->ptr) {
      if(((DDS_PAGE*)((DDS_PAGE*)page->ptr))->ptr) {
         ddsDumpPage(indent, (DDS_PAGE*)((DDS_PAGE*)page->ptr));
         fprintf(dds_print_fp, "%*s", indent, "");
      }
   }

   fprintf(dds_print_fp, "PAGE &= %p; top= %5li; max= %5li; ptr= %p;\n",
      (void*)page,
      (long)page->top,
      (long)page->max,
      (void*)page->ptr);

   return;
}
