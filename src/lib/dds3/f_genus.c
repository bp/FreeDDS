static char rcsid[]="$Id: f_genus.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, fetch field genus from DDS structures

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 16 July 1996           ***/
/**********************************************/

#include "dds_sys.h"


HOST_FTN_INT
HOST_FTN_FUNC(fdds_genus, FDDS_GENUS)(
   HOST_FTN_STR genus_ftn,
   HOST_FTN_INT *bin,
   HOST_FTN_INT *field,
   HOST_FTN_LEN genus_len)
{
   const char *gen;

   DDS_API_INIT;

   gen = cdds_genus(*bin, *field);
   if (!gen) return DDS_BAD;

   STR_TO_FTN_STR(genus_ftn, genus_len, gen);

   return DDS_OK;
}
