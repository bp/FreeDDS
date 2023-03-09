static char rcsid[]="$Id: c_prime.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, return symbol tag for prime type

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 11 July 1996           ***/
/**********************************************/

#include "dds_sys.h"

SYM_TAG (cdds_prime)(
   BIN_TAG bin_tag,
   SYM_TAG sym_tag)
{
   DDS_BIN *bin;
   DDS_TYPE *type;
   SYM_TAG new_tag;

   DDS_API_ENTRY("prime")

   BIN_TAG_TO_BIN(bin, bin_tag, DDS_API_EXIT(0));

   SYM_TAG_TO_TYPE(type, bin, sym_tag, DDS_API_EXIT(0));

   while(type->type) type = type->type;

   SYM_TO_SYM_TAG(new_tag, &bin->sym_tag, dds_prime[type->ptype].type)

   DDS_API_EXIT(new_tag)
}
