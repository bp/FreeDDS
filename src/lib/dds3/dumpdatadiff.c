static char rcsid[]="$Id: dumpdatadiff.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, dump data buffer, difference level

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

int ddsDumpDataDiff(
   int indent,
   DDS_TYPE *type,
   size_t offset,
   const char *buf,
   const char *buf1,
   const char *buf2)
{
   static int diff, status;
   size_t size;
   char str[DDS_DUMP_MAX + 1];

   if(! dds_initialized) return DDS_BAD; /* no-op */

   if(! type) {
      fprintf(dds_print_fp, "ddsDumpDataDiff: NULL TYPE ptr\n");
      return DDS_BAD;
   }

   /* return DDS_OK  if dump data should be displayed. */ 
   /* return DDS_BAD if dump data should be muted. */

   if(! (dds_state & DDS_DUMP_DEFAULT)) {
      /* can't difference anything (non-default type) */
      diff = 0;
      status = DDS_OK;
   } else if(type->ptype == DDS_STYPE_STRUCT ||
             type->ptype == DDS_STYPE_UNION) {
      /* processing struct or union (always display, never difference) */
      diff = 0;
      status = DDS_OK;
   } else if(buf) {
      /* Beginning of array (not the interior a printing array) */
      if(dds_state & DDS_DUMP_DIFF0) {
         /* dump everything */
         status = DDS_OK;
      } else {
         /* Assume the dump will be muted. */
         /* Dump iff buffer differs at the first or second level */
         status = DDS_BAD;
      }

      if(type->stroke) {
         buf  += offset;
         buf1 += offset;
         buf2 += offset;

         if(memcmp(buf, buf1, type->stroke)) {
            if(dds_state & DDS_DUMP_DIFF1) status = DDS_OK;
            if(memcmp(buf1, buf2, type->stroke)) {
               diff = 1;
            } else {
               diff = 3;
               if(dds_state & DDS_DUMP_DIFF2) status = DDS_OK;
            }
         } else {
            if(memcmp(buf1, buf2, type->stroke)) {
               diff = 2;
               if(dds_state & DDS_DUMP_DIFF2) status = DDS_OK;
            } else {
               diff = 0;
            }
         }
      }
   }

   sprintf(str, "    %*sDump(%i) %4lu: ",
      indent, "", diff, (unsigned long)offset);

   size = strlen(str);
   if(size > DDS_DUMP_MAX) { 
      ddsPanic("ddsDumpDataDiff: string overflow\n");
   }

   ddsStackChar(&dds_dump_que, str, size);

   return status;
}
