static char rcsid[]="$Id: f_copy.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, copy one scan definition to print dictionary

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

#define ALIAS_NOM_LEN 4 * DEFNNAME_MAX


HOST_FTN_INT
HOST_FTN_FUNC(fdds_copy, FDDS_COPY)(
   HOST_FTN_STR alias_ftn,
   HOST_FTN_LEN alias_len)
{
   static DDS_STRING alias = NULL_STRING;

   DDS_API_INIT;

   FTN_STR_TO_STR(alias, alias_ftn, alias_len);

   return cdds_copy(alias.ptr);
}
