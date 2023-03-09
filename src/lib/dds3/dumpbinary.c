static char rcsid[]="$Id: dumpbinary.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, dump all DDS_BIN and associated symbol tables

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

void ddsDumpBinary(void)
{
   BIN_TAG bin_tag;
   FILE *save_fp;

   if(! dds_initialized) return; /* no-op */

   save_fp = dds_print_fp;
   remove("dds_dump.out");
   dds_print_fp = fopen("dds_dump.out", "w");

   ddsDumpGlobal();

   for(bin_tag = 0; bin_tag < dds_bin_max; bin_tag++) {
      if(dds_bin[bin_tag]) ddsDumpBinSym(DDS_DENT, bin_tag);
   }

   ddsDumpScanner(dds_lexer);

   fclose(dds_print_fp);
   dds_print_fp = save_fp;

   return;
}
