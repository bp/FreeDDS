static char rcsid[]="$Id: symlookupmem.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, lookup member name in symbol table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

#if defined(_WIN32) || defined(MINGW)
DDS_MEMBER * ddsSymLookupMem(
   const DDS_TABLE *table,
   const char *name,
   DDS_BLOCK *block,
   size_t *bias)
#else
DDS_MEMBER * ddsSymLookupMem(
   const DDS_TABLE *table,
   const char *name,
   DDS_BLOCK *block,
   long *bias)
#endif
{
   static DDS_STRING alias = NULL_STRING;
   DDS_MEMBER *member;
   DDS_TYPE *type;
   const char *dot, *subname;
   size_t size;

   /* find the member "name{.sub1{.sub2{...}}}" */
   *bias = 0;
   subname = name;
   while(DDS_TRUE) {
      dot = strchr(subname, '.');
      if(dot) {
         size = dot - subname;
      } else {
         size = strlen(subname);
      }
      ddsStrCpyN(&alias, subname, size);
      member = UNIQUE_MEMBER(table, alias.ptr, block);
      if(! member) break;
      *bias += member->offset;
      type = member->type;
      if(! dot) break;
      if(type->ptype != DDS_STYPE_STRUCT &&
         type->ptype != DDS_STYPE_UNION) {
         member = NULL;
         break;
      }
      block = type->block;
      subname = dot + 1;
   }
   return member;
}
