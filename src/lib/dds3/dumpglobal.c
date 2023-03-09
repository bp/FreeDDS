static char rcsid[]="$Id: dumpglobal.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, dump selected values from global variables

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

void ddsDumpGlobal(void)
{
   if(! dds_initialized) return; /* no-op */

   fprintf(dds_print_fp, "dds_warn_count= %i; _error_= %i; _debug_= %i;\n",
      (int)dds_warn_count,
      (int)dds_error_count,
      (int)dds_debug_count);

   fprintf(dds_print_fp, "dds_heap ");
   ddsDumpStack(DDS_DENT, &dds_heap);

   fprintf(dds_print_fp, "dds_dict ");
   ddsDumpTable(DDS_DENT, &dds_dict);

   fprintf(dds_print_fp, "dds_scan_first= %p; _last= %p; _dict= %p;\n",
      (void*)dds_scan_first,
      (void*)dds_scan_last,
      (void*)dds_scan_dict);

   fprintf(dds_print_fp, "dds_scan_defn= %p; _name= %s;\n",
      (void*)dds_scan_defn,
      dds_scan_name);

   fprintf(dds_print_fp, "dds_tmp ");
   ddsDumpStack(DDS_DENT, &dds_tmp);

   fprintf(dds_print_fp, "dds_keyword ");
   ddsDumpTable(DDS_DENT, &dds_keyword);

   fprintf(dds_print_fp, "dds_prime_table ");
   ddsDumpTable(DDS_DENT, &dds_prime_table);

   fprintf(dds_print_fp, "dds_prime_tag ");
   ddsDumpStack(DDS_DENT, &dds_prime_tag);

   fprintf(dds_print_fp, "dds_error_block= %p;   _type= %p; _object= %p; _expr= %p;\n",
      (void*)dds_error_block,
      (void*)dds_error_type,
      (void*)dds_error_object,
      (void*)dds_error_member);

   fprintf(dds_print_fp, "      _expr= %p; _thread= %p;   _value= %p;\n",
      (void*)dds_error_expr,
      (void*)dds_error_thread,
      (void*)dds_error_value);

   fprintf(dds_print_fp, "dds_prime_block= %p;\n",
      (void*)dds_prime_block);

   fprintf(dds_print_fp, "dds_compile= %p; _symbol= %p; _block= %p;\n",
      (void*)dds_compile,
      (void*)dds_symbol,
      (void*)dds_block);

   fprintf(dds_print_fp, "dds_symbol ");
   ddsDumpTable(DDS_DENT, dds_symbol);

   fprintf(dds_print_fp, "dds_thread= %p; _needle= %p; _map_value= %p;\n",
      (void*)dds_thread,
      (void*)dds_needle,
      (void*)dds_map_value);

   if(dds_prime_table.chain) {
      fprintf(dds_print_fp, "%*s", (int)DDS_DENT, "");
      ddsDumpBlockR(DDS_TRUE, 2 * DDS_DENT, (DDS_BLOCK*)dds_prime_table.oldest);
   }

   ddsDumpDeclare(0, dds_declare);

   return;
}
