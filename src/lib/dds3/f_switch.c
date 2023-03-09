static char rcsid[]="$Id: f_switch.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS, standard method to fetch a command line switch

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

HOST_FTN_INT
HOST_FTN_FUNC(fdds_switch, FDDS_SWITCH)(
   HOST_FTN_STR swtch_ftn,
   HOST_FTN_INT *dflt,
   HOST_FTN_LEN swtch_len)
{
   static DDS_STRING swtch = NULL_STRING;

   DDS_API_INIT;

   FTN_STR_TO_STR(swtch, swtch_ftn, swtch_len);

   return cdds_switch(swtch.ptr, (int)*dflt);
}
