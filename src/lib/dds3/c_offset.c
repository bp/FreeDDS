static char rcsid[]="$Id: c_offset.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, return offset to member within struct/union

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 15 July 1996           ***/
/**********************************************/

#include "dds_sys.h"

int cdds_offset(
   BIN_TAG bin_tag,
   SYM_TAG sym_tag)
{
   DDS_BIN *bin;
   DDS_MEMBER *member;
#if defined(_WIN32) || defined(MINGW)
   size_t *bias;
#else
   long *bias;
#endif

   DDS_API_ENTRY("offset");

   BIN_TAG_TO_BIN(bin, bin_tag, DDS_API_EXIT(-1));

   SYM_TAG_TO_MEMBER(member, bin, sym_tag, DDS_API_EXIT(-1));

   SYM_TAG_TO_BIAS(bias, bin, sym_tag, DDS_API_EXIT(-1))

   DDS_API_EXIT( *bias );
}
