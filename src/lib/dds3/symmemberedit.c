static char rcsid[]="$Id: symmemberedit.c 223 2011-04-06 21:25:42Z user $";
/*
RCS-t=DDS, append and edit struct/union member

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

/************************************************************************/
/*  SUMMARY OF EDIT LOGIC                                               */
/*     exists... "test" member already exists in struct/union list.     */
/*     unique... "test" member does NOT already exist in list.          */
/*                                                                      */
/*     delete  => delete symbol from existing or "test" member list.    */
/*     append  => append "test" symbol and type to member list.         */
/*     del-app => delete symbol and append "test" member to list.       */
/*     force   => force type of existing member to "test" type.         */
/*     keep    => keep member without change.                           */
/*     ---     => no member and no change.                              */
/************************************************************************/
/*         {...; op tdef a, b, c;}  {...; op struct;}  {...; op tdef;}  */
/*          ---------------------    ---------------    -------------   */
/*               members    a,b,c         members           members     */
/*               -------    -----         -------           -------     */
/*  DELETE  "-"                                                         */
/*     exists    delete     delete        delete            delete      */
/*     unique    ---        delete        ---               ---         */
/*  REPLACE "*"                                                         */
/*     exists    force      delete        force             ---         */
/*     unique    ---        delete        ---               ---         */
/*  APPEND  "+"                                                         */
/*     exists    keep       delete        keep              ---         */
/*     unique    ---        keep          append            ---         */
/*  DAPPEND "-+"                                                        */
/*     exists    delete     keep          del-app           del-app     */
/*     unique    ---        keep          append            ---         */
/*  RAPPEND "*+"                                                        */
/*     exists    force      delete        force             ---         */
/*     unique    ---        keep          append            ---         */
/***********************************************************************/

#include "dds_sys.h"

static const char *EditName(
   const char *old_name)
{
   const char *new_name;
   static DDS_STRING rename = NULL_STRING;
#if defined(_WIN32) || defined(MINGW)
   size_t bias;
#else
   long bias;
#endif

   /* check if this name needs to be changed */
   cdds_dict(dds_compile->dict_fmt, "scan");
   ddsStrCpyCat3(&rename, "rename:", dds_compile->old_format, ".");
   ddsStrCat3(&rename, old_name, ":", dds_compile->fmt_name);
   if (cdds_scant(rename.ptr, &new_name) < 1) {
      return old_name;
   }

   /* check if new name already exists */
   if(ddsSymLookupMem(dds_symbol, new_name, dds_block, &bias)) {
      ddsWarn("renaming '%s' to '%s' (exists)!\ndict=%s\ndefn=%s\n", 
         old_name, new_name, dds_compile->dict_fmt, rename.ptr);
   }

   /* add new name to table */
   new_name = ddsTablePushStr(dds_symbol, new_name);

   return new_name;
}


static DDS_TYPE* ChangeType(
   DDS_TYPE *type,
   DDS_PTYPE ptype,
   int prec)
{
   if (!type) return type;
 
   if (!type->type) return dds_prime[ptype].type;

   type->ptype = ptype;
   type->prec = prec;
   type->stroke = type->count * type->prec;
 
   type->type = ChangeType(type->type, ptype, prec);
 
   if (type->trail) {
      if (type->trail == (DDS_OBJECT*)dds_prime_block || 
          type->trail == (DDS_OBJECT*)dds_error_block) {
         type->trail = 0;
      } else {
         type->trail->type = ChangeType(type->trail->type, ptype, prec);
      }
   }
   return type;
}   


static void EditType(
   DDS_MEMBER *member)
{
   DDS_TYPE *type;
   DDS_PTYPE ptype;
   DDS_MEDIA_GENUS media_genus;
   int prec;
 
   type = member->type;
 
   if (!strcmp(dds_compile->fmt_name, "asp")
      || !strcmp(dds_compile->fmt_name, "sep")
      || strstr(dds_compile->fmt_name, "cube")) {
     /*******************************/
      /* edit cube, sep & asp types */
     /*******************************/
 
      /* edit only if outputing to tape or disk */
      media_genus = dds_compile->media->genus;

      if (  media_genus != DDS_MEDIA_REG
         && media_genus != DDS_MEDIA_CHR) return;
 
      if (!strcmp(dds_compile->fmt_name, "fcube")) {
         ptype = DDS_STYPE_FLOAT4;
         prec  = 4;
      } else if (!strcmp(dds_compile->fmt_name, "fcubex")) {
         ptype = DDS_STYPE_FLOAT4X;
         prec  = 4;
      } else if (!strcmp(dds_compile->fmt_name, "dcube")) {
         ptype = DDS_STYPE_FLOAT8;
         prec  = 8;
      } else if (!strcmp(dds_compile->fmt_name, "dcubex")) {
         ptype = DDS_STYPE_FLOAT8X;
         prec  = 8;
      } else if (!strcmp(dds_compile->fmt_name, "ccube")) {
         ptype = DDS_STYPE_COMPLEX8;
         prec  = 8;
      } else if (!strcmp(dds_compile->fmt_name, "ccubex")) {
         ptype = DDS_STYPE_COMPLEX8X;
         prec  = 8;
      } else if (!strcmp(dds_compile->fmt_name, "icube")) {
         ptype = DDS_STYPE_INTEGER4;
         prec  = 4;
      } else if (!strcmp(dds_compile->fmt_name, "icubex")) {
         ptype = DDS_STYPE_INTEGER4X;
         prec  = 4;
      } else if (!strcmp(dds_compile->fmt_name, "i2cube")) {
         ptype = DDS_STYPE_INTEGER2;
         prec  = 2;
      } else if (!strcmp(dds_compile->fmt_name, "i2cubex")) {
         ptype = DDS_STYPE_INTEGER2X;
         prec  = 2;
      } else if (!strcmp(dds_compile->fmt_name, "i1cube")) {
         ptype = DDS_STYPE_INTEGER1;
         prec  = 1;
      } else if (!strcmp(dds_compile->fmt_name, "sep")) {
         ptype = DDS_STYPE_FLOAT4;
         prec  = 4;
      } else return;
 
      if (type->ptype == ptype) return;
 
      member->type = ChangeType(member->type, ptype, prec);
   }
 
   return;
}


void ddsSymMemberEdit(
   DDS_SYM_FLAG flag,
   DDS_MEMBER *member)
{
   DDS_BLOCK *block2;
   DDS_MEMBER *member_stop, *member2, *member3;
   DDS_TYPE *type, *type2, *type3;
   DDS_SYM *older_sym, *newer_sym, *newest_sym;
   DDS_OBJECT *object2, *trail;
   DDS_HASH hash2;
   DDS_BOOLEAN unique;
   const char *name2;

   type = dds_declare->type;

   if(! (dds_state & DDS_YACC_CREATE)) {
      ddsYYError("member edit on *input* struct/union\n");
   }

   if(member) {
      /* explicit list of test members */
      /* walk the list of members to test */
      for(member2 = member;
         member2;
         member2 = (DDS_MEMBER*)member2->newer) {
         if(! (member2->flag & DDS_SYM_MEMBER)) continue;
         hash2 = member2->hash;
         name2 =  member2->name;
         unique = DDS_TRUE;
         /* walk the list of existing members */
         for(member3 = (DDS_MEMBER*)dds_block->newer;
            member3 && member3 != member;
            member3 = (DDS_MEMBER*)member3->newer) {
            if(! (member3->flag & DDS_SYM_MEMBER)) continue;
            if(member3->hash == hash2 &&
               ! strcmp(member3->name, name2)) {
               if(flag & (DDS_SYM_DELETE | DDS_SYM_DAPPEND)) {
                  /* delete this existing member (zombie) */
                  member3->flag &= (DDS_SYM_FLAG) ~ DDS_SYM_MEMBER;
               } else if(flag & (DDS_SYM_REPLACE | DDS_SYM_RAPPEND)) {
                  /* force type of existing member */
                  member3->type = member2->type;
                  EditType(member3);
               }
               unique = DDS_FALSE;
            }
         }
         if(flag & (DDS_SYM_DELETE | DDS_SYM_REPLACE) ||
            ((flag & (DDS_SYM_APPEND | DDS_SYM_RAPPEND)) && ! unique)) {
            /* delete test member */
            member2->flag &= (DDS_SYM_FLAG) ~ DDS_SYM_MEMBER;
         }
         assert(member3 && "ddsSymMemberEdit: explicit delete closure\n");
      }
   } else if(type->ptype == DDS_STYPE_STRUCT ||
             type->ptype == DDS_STYPE_UNION) {
      /* implicit list of test members, given by struct/union type */
      if(type->trail) {
         object2 = type->trail;
         type2 = object2->type;
         block2 = type2->block;
      } else {
         block2 = type->block;
      }
      /* walk the list of members to test */
      for(member2 = (DDS_MEMBER*)block2->first;
         member2;
         member2 = (DDS_MEMBER*)member2->newer) {
         if(! (member2->flag & DDS_SYM_MEMBER)) continue;
         hash2 = member2->hash;
         name2 = member2->name;
         unique = DDS_TRUE;
         /* walk the list of existing members */
         for(member3 = (DDS_MEMBER*)dds_block->newer;
            member3;
            member3 = (DDS_MEMBER*)member3->newer) {
            if(!(member3->flag & DDS_SYM_MEMBER)) continue;
				if ((member3->hash == hash2) &&
               ! strcmp(member3->name, name2)) {
               if(flag & (DDS_SYM_DELETE | DDS_SYM_DAPPEND)) {
                  /* delete this existing member (zombie) */
                  member3->flag &= (DDS_SYM_FLAG) ~ DDS_SYM_MEMBER;
               } else if(flag & (DDS_SYM_REPLACE | DDS_SYM_RAPPEND)) {
                  /* force type of existing member */
                  member3->type = member2->type;
                  EditType(member3);
               }
               member = member3;
               unique = DDS_FALSE;
            }
            if(! member3->newer) break;
         }
         if((flag & DDS_SYM_DAPPEND) ||
            (unique && (flag & (DDS_SYM_APPEND | DDS_SYM_RAPPEND)))) {
            /* append test member to existing members */
            if(unique) member = member2;
            dds_declare->type = member->type;
            member->name = EditName(member->name);
            member = ddsSymMember(member->name);
            EditType(member);
         }
      }
   } else if(type->trail) {
      /* no list of test members, only typedef trail available */
      /* walk the list of existing members */
      if(flag & DDS_SYM_DELETE) {
         /* delete members with same typedef trail */
         trail = type->trail;
         for(member3 = (DDS_MEMBER*)dds_block->newer;
            member3;
            member3 = (DDS_MEMBER*)member3->newer) {
            if(! (member3->flag & DDS_SYM_MEMBER)) continue;
            type3 = member3->type;
            if(type3->trail == trail) {
               /* delete this member (zombie) */
               member3->flag &= (DDS_SYM_FLAG) ~ DDS_SYM_MEMBER;
            }
         }
      } else if(flag & DDS_SYM_DAPPEND) {
         /* delete and append members with same typedef trail */
         /* i.e. move all members with this type to the end */
         trail = type->trail;
         member_stop = (DDS_MEMBER*)dds_declare->more;
         for(member3 = (DDS_MEMBER*)dds_block->newer;
            member3 && member3 != member_stop;
            member3 = (DDS_MEMBER*)member3->newer) {
            if(! (member3->flag & DDS_SYM_MEMBER)) continue;
            type3 = member3->type;
            if(type3->trail == trail) {
               /* append this member to the end */
               older_sym = (DDS_SYM*)member3->older;
               newer_sym = (DDS_SYM*)member3->newer;
               older_sym->newer = member3->newer;
               newer_sym->older = member3->older;
               newest_sym = (DDS_SYM*)dds_symbol->newest;
               newest_sym->newer = (DDS_ITEM*)member3;
               member3->older = (DDS_ITEM*)dds_symbol->newest;
               member3->newer = 0;
               dds_symbol->newest = (DDS_ITEM*)member3;
               member3 = (DDS_MEMBER*)older_sym;
               EditType(member3);
            }
         }
         assert(member3 && "ddsSymMemberEdit: move member closure\n");
      }
   }

   return;
}
