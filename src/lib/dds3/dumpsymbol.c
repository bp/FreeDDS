static char rcsid[]="$Id: dumpsymbol.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, dump symbol DDS_BLOCK recursively

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

void ddsDumpSymbol(void)
{
   FILE *save_fp;

   if(! dds_initialized) return; /* no-op */

   save_fp = dds_print_fp;
   remove("dds_dump.out");
   dds_print_fp = fopen("dds_dump.out", "w");

   ddsDumpGlobal();

   ddsDumpTable(DDS_DENT, dds_symbol);

   ddsDumpMapOut(DDS_DENT, dds_compile->tag);

   ddsDumpBlockR(DDS_TRUE, DDS_DENT, (DDS_BLOCK*)dds_compile->symbol.oldest);

   ddsDumpScanner(dds_lexer);

   fclose(dds_print_fp);
   dds_print_fp = save_fp;

   return;
}
