static char rcsid[]="$Id: dumpdatavalue.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, dump data buffer value

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

void ddsDumpDataValue(
   int min,
   int max,
   DDS_TYPE *type,
   const char *buf)
{
   DDS_BLOCK *block;
   DDS_SYM *sym;
   DDS_OBJECT *object;
   size_t reset;
   int str_max;
   const char *name, *str;
   long long_value, long_test;
   unsigned long ulong_value;
   float float_value, complex_value[2];
   const void *value_ptr;
   void *ptr_value;

   if(! dds_initialized) return; /* no-op */

   if(dds_dump_que.top) {
      /* flush previous contents of line queue */
      fwrite(dds_dump_que.ptr, 1, dds_dump_que.top, dds_print_fp);
      dds_dump_que.top = 0;
   }

   if(! type) {
      fprintf(dds_print_fp, " %*s", min, "");
      return;
   }

   assert(type->flag & DDS_SYM_TYPE);

   switch(dds_prime[type->ptype].genus) {
   case DDS_TYPE_VOIDS:
      fprintf(dds_print_fp, " %*s", min, "void");
      break;
   case DDS_TYPE_CHARS:
      reset = dds_tmp.top;
      str_max = max ? max : 64;

      str = ddsStackTmp(str_max, 1);

      CON_AAVV(DDS_CTYPE_CHAR, type->ptype, str, buf, str_max);
     
      fprintf(dds_print_fp, " \"%*.*s\"", min, str_max, str);

      ddsStackSet(&dds_tmp, reset);
      break;
   case DDS_TYPE_INTEGERS:
      CON_ANSS(DDS_CTYPE_LONG, type->ptype, &long_value, buf);

      fprintf(dds_print_fp, " %*li", min, long_value);

      if(! max && (type->flag & DDS_SYM_ENUM)) {
         block = type->block;

         for(sym = block->first; sym; sym = (DDS_SYM*)sym->newer) {
            if((sym->flag & (DDS_SYM_OBJECT | DDS_SYM_ENUM)) !=
               (DDS_SYM_OBJECT | DDS_SYM_ENUM) ||
               sym->block != block) continue;
            object = (DDS_OBJECT*)sym;
            value_ptr = object->ptr;

            CON_ANSS(DDS_CTYPE_LONG, type->ptype, &long_test, value_ptr);

            if(long_test == long_value) {
               name = sym->name;
               fprintf(dds_print_fp, "=%s", name);
            }
         }
      }
      break;
   case DDS_TYPE_UNSIGNEDS:
      CON_ANSS(DDS_CTYPE_ULONG, type->ptype, &ulong_value, buf);

      fprintf(dds_print_fp, " %*lu", min, ulong_value);
      break;
   case DDS_TYPE_FLOATS:
      CON_ANSS(DDS_CTYPE_FLOAT, type->ptype, &float_value, buf);

      fprintf(dds_print_fp, " %*g", min, float_value);
      break;
   case DDS_TYPE_COMPLEXS:
      CON_ANSS(DDS_CTYPE_CPX, type->ptype, complex_value, buf);

      fprintf(dds_print_fp, " %*g %*g",
         min, complex_value[0],
         min, complex_value[1]);
      break;
   case DDS_TYPE_PTRS:
      CON_ANSS(DDS_CTYPE_PTR, type->ptype, &ptr_value, buf);

      fprintf(dds_print_fp, " %*p", min, ptr_value);
      break;
   case DDS_TYPE_STRUCTS:
      fprintf(dds_print_fp, " %*s", min, "struct");
      break;
   case DDS_TYPE_UNIONS:
      fprintf(dds_print_fp, " %*s", min, "union");
      break;
   default:
      ddsPanic("ddsDumpValue: bad ptype\n");
   }
   return;
}
