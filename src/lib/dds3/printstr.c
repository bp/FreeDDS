static char rcsid[]="$Id: printstr.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS, "C" print string for errors, warnings, etc.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

void ddsPrintStr(
   const char *msg)
{
#ifdef DDS_ALLOW_FORTRAN_PRINT
   const char *msg_next, *newline;
   static char line_buf[DDS_FTN_LINE];
   static size_t line_size = 0;
   size_t more, want;

   if(dds_print_lu == -1) {
      /* natural C i/o */
      fputs(msg, dds_print_fp);
   } else {
      /* fortran print, line-by-line (DDS_FTN_LINE max size) */
      for(msg_next = msg;
         msg_next[0];
         msg_next += more + 1) {
         newline = strchr(msg_next, '\n');
         if(newline) {
            more = newline - msg_next;
         } else {
            more = strlen(msg_next);
            if(! more) break;
         }
         want = line_size + more;
         if(! want) {
            /* blank line ? (one blank character) */
            line_buf[0] = ' ';
            line_size = 1;
         } else if(want <= DDS_FTN_LINE) {
            /* nominal print line */
            memcpy(line_buf + line_size, msg_next, more);
            line_size = want;
            if(! newline) {
               if(line_size < DDS_FTN_LINE) break;
               --more;
            }
         } else {
            /* line overflow */
            more = DDS_FTN_LINE - line_size;
            memcpy(line_buf + line_size, msg_next, more);
            line_size = DDS_FTN_LINE;
            --more;
         }
         HOST_FTN_FUNC(ddsftnprt, DDSFTNPRT)(
            &dds_print_lu, line_buf, &line_size);
         line_size = 0;
      }
   }
#else
   fputs(msg, dds_print_fp);
#endif

   return;
}
