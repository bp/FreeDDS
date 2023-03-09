static char rcsid[]="$Id: dumpobject.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, dump DDS_OBJECT

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

void ddsDumpObject(
   int indent,
   DDS_OBJECT *object)
{
   if(! dds_initialized) return; /* no-op */

   if(! object) {
      fprintf(dds_print_fp, "ddsDumpObject: NULL OBJECT ptr\n");
      return;
   }

   fprintf(dds_print_fp, "OBJECT ");
   ddsDumpSym(indent, (DDS_SYM*)object);

   fprintf(dds_print_fp, "%*sptr= %p; reloc= %p; track= %p\n",
      indent, "", (void*)object->ptr,
                  (void*)object->reloc,
                  (void*)object->track);

   ddsDumpReloc(indent, (DDS_VALUE*)object);

   return;
}
