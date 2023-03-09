static char rcsid[]="$Id: testcomp1.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS main, test compiler simplest functionality.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "parser.tab.h"

int main(
   int argc,
   char **argv)
{
   int rc;

   setargcv(argc, argv);
 
   cdds_init();
   dds_state |= DDS_VERBOSE;
   dds_state &= (DDS_STATE) ~ (DDS_YACC_CREATE | DDS_YACC_MAP);
   dds_lex_ss = DDS_LEX_TRANSLATE;

   if(! cdds_dict("stdin:", "scan") &&
      ! cdds_scanf("hello", "")) {
      dds_hist = "stdin:";
      ddsLexStr(dds_scan_dict->name, dds_scan_name, dds_scan_token.next,
         dds_scan_dict->buf.ptr + dds_scan_newer->start,
         dds_scan_defn->line);
      rc = ddsYYparse(NULL);
      ddsSymBlockEnd();
      ddsDumpSymbol();
   }

   return 0;
}
