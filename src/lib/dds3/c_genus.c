static char rcsid[]="$Id: c_genus.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, return genus name associated with type

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 15 July 1996           ***/
/**********************************************/

#include "dds_sys.h"

const char* cdds_genus(
   BIN_TAG bin_tag,
   SYM_TAG sym_tag)
{
   DDS_BIN *bin;
   DDS_TYPE *type;
   DDS_TYPE_GENUS genus;

   DDS_API_ENTRY("genus");

   BIN_TAG_TO_BIN(bin, bin_tag, DDS_API_EXIT(NULL));

   SYM_TAG_TO_TYPE(type, bin, sym_tag, DDS_API_EXIT(NULL));

   genus = dds_prime[type->ptype].genus;

   DDS_API_EXIT(dds_prime_genus[genus].note);
}
