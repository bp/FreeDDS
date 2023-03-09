static char rcsid[]="$Id: c_count.c 262 2013-09-23 16:14:59Z user $";
/*
RCS-t=DDS API, return element count, if type is an array

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 15 July 1996           ***/
/**********************************************/

#include "dds_sys.h"

int cdds_count(
   BIN_TAG bin_tag,
   SYM_TAG sym_tag)
{
	int count;
   DDS_BIN *bin;
   DDS_TYPE *type;

   DDS_API_ENTRY("count");

   BIN_TAG_TO_BIN(bin, bin_tag, DDS_API_EXIT(DDS_BAD));

   SYM_TAG_TO_TYPE(type, bin, sym_tag, DDS_API_EXIT(DDS_BAD));

   count = 1;
   while(type->flag & DDS_SYM_ARRAY) {
      count *= type->count;
      type = type->type;
   }		

   DDS_API_EXIT(count);
}
