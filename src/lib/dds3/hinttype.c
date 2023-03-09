static char rcsid[]="$Id: hinttype.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, reconstruct a type declaration for diagnostic message

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

static void HintType2(
   DDS_STRING *msg,
   DDS_BIN *bin,
   DDS_TYPE *type)
{
   DDS_BLOCK *block;
   DDS_OBJECT *object;
   char tmp[64];
   const char *name;

   block = type->block;

   if(type->trail) {
      object = type->trail;
      name = object->name;
      ddsStrCat(msg, name);
   } else if(block->name) {
      name = block->name;
      ddsStrCpyCat2(msg, "struct ", name);
   } else if(type->type) {
      if(type->flag & DDS_SYM_ARRAY) {
         HintType2(msg, bin, type->type);
         sprintf(tmp, "%lu", type->count);
         ddsStrCat3(msg, "[", tmp, "]");
      } else {
         ddsStrCpy(msg, "*");
         HintType2(msg, bin, type->type);
      }
   } else if(type->ptype) {
		name = dds_prime[dds_prime[type->ptype].stype].name;
		ddsStrCpy(msg, name);
   } else {
      ddsStrCpy(msg, "<type>");
   }
   return;
}

const char * ddsHintType(
   DDS_BIN *bin,
   DDS_TYPE *type)
{
   static DDS_STRING msg;

   ddsStrCpy(&msg, "");
   HintType2(&msg, bin, type);
   return msg.ptr;
}
