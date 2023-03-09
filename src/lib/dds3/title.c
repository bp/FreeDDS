static char rcsid[]="$Id: title.c 42 2009-01-19 18:31:06Z user $";
/*
RCS-t=DDS, append title definitions to print dictionary

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int ddsTitle(
   const char *title)
{
   const char *gaudy =
      "********************************************\n"
      "******************* TITLE ******************\n"
      "********************************************";

   cdds_printf(DDS_SENTRY, " %s %s\n%s\n", dds_cmd_ddsrev, dds_cmd_ddsdat, gaudy);
   cdds_printf("cmd_title", " ");
   cdds_printt("", title ? title : "<none>");
   if(dds_print_first && (dds_print_first->state & DDS_DICT_LOAD)) {
      cdds_printf("", " modify\n");
   } else {
      cdds_printf("", "\n");
   }
   cdds_printf("cmd_name", " %s\n", dds_cmd_name);
   cdds_printf("cmd_rev", " %s\n", dds_cmd_rev);
   cdds_printf("cmd_ddsrev", " %s\n", dds_cmd_ddsrev);
   cdds_printf("cmd_user", " %s\n", dds_cmd_user);
   cdds_printf("cmd_date", " %s\n", dds_cmd_date);
   cdds_printf("cmd_host", " %s\n", dds_cmd_host);
   cdds_printf("cmd_cwd", " %s\n", dds_cmd_cwd);
   cdds_printf("cmd_pid", " %i\n", dds_cmd_pid);

   return DDS_OK;
}
