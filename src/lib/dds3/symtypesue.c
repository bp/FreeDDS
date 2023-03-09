static char rcsid[]="$Id: symtypesue.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, create and initialize Struct/Union/Enum DDS_TYPE in symbol table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include <limits.h>
#include "dds_sys.h"

DDS_TYPE * ddsSymTypeSUE(
   DDS_BLOCK *sue_block)
{
   DDS_HASH hash;
   HOST_LLONG_T object_value, min_value, max_value;
   DDS_PTYPE ptype;
   DDS_BLOCK *block;
   DDS_MEMBER *member, *member2;
   DDS_OBJECT *object;
   DDS_TYPE *type, *member_type;
   DDS_CON_STATUS status;
   const char *name, *name2;
   void *value_ptr;
   size_t stroke_sum, stroke_mem, stroke_max, align, part;

   type = dds_declare->type;
   /* lex context, want IDENTIFIER_TOKEN (not existing object) */
   dds_declare->flag |= DDS_SYM_IDENT;

   if(type->flag & DDS_SYM_ERROR) return type; /* sorry */
   
   if(type->flag & DDS_SYM_INCOMPLETE) {
      /* forward reference (incomplete until back patched).  */
      return type;
   }

   /* define a new struct/union/enum and back patch as needed. */
   ptype = type->ptype;

   switch(ptype) {
   case DDS_STYPE_STRUCT:
      stroke_sum = 0;
      align = 1;
      member_type = NULL; /* empty struct flag */
      for(member = (DDS_MEMBER*)sue_block->first;
         member;
         member = (DDS_MEMBER*)member->newer) {
         if(! (member->flag & DDS_SYM_MEMBER)) continue;
         hash = member->hash;
         name = member->name;
         for(member2 = (DDS_MEMBER*)sue_block->first;
            member2 != member;
            member2 = (DDS_MEMBER*)member2->newer) {
            if(! (member2->flag & DDS_SYM_MEMBER)) continue;
            name2 = member2->name;
            if(member2->hash == hash && ! strcmp(name2, name)) {
               ddsYYError("'%s' struct member is defined more than once\n",
                  name);
               break;
            }
         }
         member_type = member->type;
         if(align < member_type->align) align = member_type->align;
         part = stroke_sum % member_type->align;
         if(part) stroke_sum += member_type->align - part;
         member->offset = stroke_sum;
         stroke_sum += member_type->stroke;
      }
      if(! member_type) {
         ddsYYWarn("empty struct\n");
      } else {
         if (type->flag & DDS_SYM_ALIGN) {
            align = type->align;
         }
         part = stroke_sum % align;
         if(part) stroke_sum += align - part;
         if(! (type->flag & DDS_SYM_ALIGN)) type->align = align;
         type->prec = stroke_sum;
         type->stroke = stroke_sum;
      }
      break;
   case DDS_STYPE_UNION:
      align = 1;
      stroke_max = 0;
      member_type = NULL; /* empty union flag */
      for(member = (DDS_MEMBER*)sue_block->first;
         member;
         member = (DDS_MEMBER*)member->newer) {
         if(! (member->flag & DDS_SYM_MEMBER)) continue;
         hash = member->hash;
         name = member->name;
         for(member2 = (DDS_MEMBER*)sue_block->first;
            member2 != member;
            member2 = (DDS_MEMBER*)member2->newer) {
            if(! (member2->flag & DDS_SYM_MEMBER)) continue;
            name2 = member2->name;
            if(member2->hash == hash && ! strcmp(name2, name)) {
               ddsYYError("'%s' union member is defined more than once\n",
                  name);
               break;
            }
         }
         member_type = member->type;
         if(align < member_type->align) align = member_type->align;
         stroke_mem = member_type->stroke;
         if(stroke_max < stroke_mem) stroke_max = stroke_mem;
      }
      if(! member_type) {
         ddsYYWarn("empty union\n");
      } else {
         if (type->flag & DDS_SYM_ALIGN) {
            align = type->align;
         }
         part = stroke_max % align;
         if(part) stroke_max += align - part;
         if(! (type->flag & DDS_SYM_ALIGN)) type->align = align;
         type->prec = stroke_max;
         type->stroke = stroke_max;
      }
      break;
   default:
      max_value = ((~ (HOST_ULLONG_T)0) >>
         (1 + CHAR_BIT * (HOST_LLONG_P - type->prec)));
      min_value = (-max_value) - 1;
      object_value = 0;
      value_ptr = NULL; /* empty enum flag */
      for(object = (DDS_OBJECT*)sue_block->newer;
         object;
         object = (DDS_OBJECT*)object->newer) {
         if(! (object->flag & DDS_SYM_OBJECT) ||
            ! (object->flag & DDS_SYM_ENUM) ||
            object->block != dds_block) continue;
         status = DDS_CON_OK;
         if(object->ptr) {
            /* retrieve explicit value */
            value_ptr = object->ptr;
            status = CON_AASS(HOST_LLONG_DDS, ptype,
               &object_value, value_ptr);
            if(status != DDS_CON_OK)
               ddsYYWarn("constant conversion error to enum type '%s'\n",
                  dds_prime[HOST_LLONG_DDS].name);
         } else {
            /* preserve implicit value */
            value_ptr = ddsTablePush(dds_symbol, type->prec, type->align);
            object->ptr = value_ptr;
            status = CON_AASS(ptype, HOST_LLONG_DDS,
               value_ptr, &object_value);
            if(object_value > max_value)
               ddsYYWarn("integral constant overflow, enum type '%s'\n",
                  dds_prime[ptype].name);
            else if(object_value < min_value)
               ddsYYWarn("integral constant underflow, enum type '%s'\n",
                  dds_prime[ptype].name);
            else if(status != DDS_CON_OK)
               ddsYYWarn("constant conversion error to enum type '%s'\n",
                  dds_prime[ptype].name);
         }
         object_value++;
      }
      if(! value_ptr) {
         ddsYYWarn("empty enum\n");
      }
      break;
   }

   if(sue_block->name) {
      /* back patch as needed. */
      /* no doubt this back patching is broken... :-( */
      name = sue_block->name;
      hash = sue_block->hash;
      for(block = (DDS_BLOCK*)sue_block->chain;
         block;
         block = (DDS_BLOCK*)block->chain) {
         if(hash == block->hash &&
            block->type &&
            (block->flag & DDS_SYM_BLOCK) &&
            ! strcmp(name, block->name)) {
            if(! (member_type->flag & DDS_SYM_INCOMPLETE)) break; /* done */
            if(member_type->ptype != ptype ||
               (member_type->flag    & (DDS_SYM_STORE | DDS_SYM_PREP | DDS_SYM_QUAL)) !=
               (type->flag & (DDS_SYM_STORE | DDS_SYM_PREP | DDS_SYM_QUAL))) {
               ddsYYError("type specification inconsistant with forward reference\n");
               type->flag |= DDS_SYM_ERROR;
               break;
            }
            member_type->flag |= (DDS_SYM_FLAG)(type->flag &
               (DDS_SYM_STORE | DDS_SYM_PREP | DDS_SYM_QUAL));
            member_type->flag &= (DDS_SYM_FLAG)(~ DDS_SYM_INCOMPLETE);
            if(ptype == DDS_STYPE_STRUCT || ptype == DDS_STYPE_UNION) {
               member_type->align = type->align;
               member_type->prec = type->prec;
               member_type->stroke = type->stroke;
            }
            member_type->block = sue_block;
         }
      }
   }

   sue_block->type = type;
   sue_block->flag |= (DDS_SYM_FLAG)(type->flag & DDS_SYM_SUE);

   return type;
}
