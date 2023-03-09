static char rcsid[]="$Id: c_member.c 196 2010-10-25 23:23:21Z user $";
/*
RCS-t=DDS API, return member tag, given its name.

*/

/****************************************/
/***   Copyright (C) 2004             ***/
/***   BP America, Houston, TX        ***/
/***   Jerry Ehlers, 30 March, 2004   ***/
/****************************************/

#include "dds_sys.h"

SYM_TAG (cdds_member)(
   BIN_TAG bin_tag,
   SYM_TAG sym_tag,
   const char *field_name)
{
   DDS_BIN *bin;
   DDS_SYM *sym;
   DDS_BLOCK *block;
   DDS_MEMBER *member;
   DDS_TYPE *type;
   SYM_TAG new_tag, new2_tag, tag;
   int stype, stype2;
   int index = 0;
#if defined(_WIN32) || defined(MINGW)
   size_t *bias_ptr, bias, bias2;
#else
   long *bias_ptr, bias, bias2;
#endif
   char *name;
   const char *cname;

   DDS_API_ENTRY("member");

   BIN_TAG_TO_BIN(bin, bin_tag, DDS_API_EXIT(DDS_BAD));

   SYM_TAG_TO_SYM(sym, bin, sym_tag, DDS_API_EXIT(DDS_BAD))

   if ((sym->flag & DDS_SYM_STRUCT) ||
       (sym->flag & DDS_SYM_UNION)  ||
       (sym->flag & DDS_SYM_OBJECT)) {
      block = sym->type->block;
   } else {
      block = NULL;
   }

   new_tag = (SYM_TAG)-1;
   if (block) {
      if (field_name) while (isspace(*field_name)) field_name++;
      if (field_name && *field_name) {
         member = ddsSymLookupMem(&bin->symbol, field_name, block, 
                                  &bias);
         if (!member && !strncmp(field_name, "Horz", 4)) {
            /* Kludge to overload TVPTxx/TVPVxx Horzxxx names for usp */
            name = (char*)malloc(1+strlen(field_name));

            /* get TVPTxx */
            type = NULL;
            bias = 0;
            stype= 0;
            sprintf(name, "TVPT%s", &field_name[4]);
            member = ddsSymLookupMem(&bin->symbol, name, block, &bias);
            SYM_TO_SYM_TAG(new_tag, &bin->sym_tag, member);
            if (new_tag < 0) {
               ddsDebug("Unable to find '%s' for '%s'\n",name,field_name);
               free(name);
               DDS_API_EXIT(new_tag);
            }
            SYM_TAG_TO_BIAS(bias_ptr, bin, new_tag, DDS_API_EXIT(DDS_BAD));
            *bias_ptr = bias;
            type = member->type;
            while(type && type->type) type = type->type;
            if (type) stype = dds_prime[type->ptype].stype;

            /* get TVPVxx */
            type  = NULL;
            bias2 = 0;
            stype2= 0;
            sprintf(name, "TVPV%s", &field_name[4]);
            member = ddsSymLookupMem(&bin->symbol, name, block, &bias2);
            SYM_TO_SYM_TAG(new2_tag, &bin->sym_tag, member);
            if (new2_tag < 0) {
               ddsDebug("Unable to find '%s' for '%s'\n",name,field_name); 
               free(name);
               DDS_API_EXIT(-1);
            }
            SYM_TAG_TO_BIAS(bias_ptr, bin, new2_tag, DDS_API_EXIT(DDS_BAD));
            *bias_ptr = bias2;
            type = member->type;
            while(type && type->type) type = type->type;
            if (type) stype2 = dds_prime[type->ptype].stype;

            free(name);

            /* Check for consistencty */
            if (stype == DDS_INTEGER2 && stype2 == DDS_INTEGER2 && bias2 == bias + 2) {
               /* flag as fake float by setting tag to negative value (except -1) */
               new_tag = -new_tag;
            } else if (stype == DDS_INTEGER2X && stype2 == DDS_INTEGER2X && bias2 == bias + 2) {
               /* flag as byte-swapped fake float by setting tag to neg & adding 10000 */
               new_tag = -(10000 + new_tag);
            } else if (new_tag >= 0 && new2_tag >= 0) {
               /* flag for both TVPTxx & TVPVxx headers w/general formats */
               new_tag = -(10000 * new2_tag + new_tag);
            } else {
               ddsDebug("Incompatible TVPT pair \n\tstype= %d %d\n\tbias=%ld %ld\n",
                        stype, stype2, bias, bias2);
               new_tag = -1;
            }

            DDS_API_EXIT(new_tag);
         } else if (!member && strchr(field_name, '[')) {
            /* Kludge to allow vector indexing */
            name = strchr(field_name, '[');
            name[0] = '\0';
            sscanf(&name[1],"%d]",&index);
            member = ddsSymLookupMem(&bin->symbol, field_name, block, 
                                     &bias);
            name[0] = '[';
            if (member) { 
               new_tag = ddsStackPush((&bin->sym_tag), sizeof(DDS_SYM_TAG), 1) /
                  sizeof(DDS_SYM_TAG);
               ((DDS_SYM_TAG*)((&bin->sym_tag)->ptr))[new_tag].sym = (DDS_SYM*)(member); 
               ((DDS_SYM_TAG*)((&bin->sym_tag)->ptr))[new_tag].offset = (long)-1; 
               SYM_TAG_TO_BIAS(bias_ptr, bin, new_tag, DDS_API_EXIT(DDS_BAD));
               SYM_TAG_TO_TYPE(type, bin, new_tag, DDS_API_EXIT(DDS_BAD));
               *bias_ptr = bias + index * type->prec;
            } 
            DDS_API_EXIT(new_tag);
         }
         SYM_TO_SYM_TAG(new_tag, &bin->sym_tag, member);
         if (new_tag < 0) {
            DDS_API_EXIT(new_tag);
         }
         SYM_TAG_TO_BIAS(bias_ptr, bin, new_tag, DDS_API_EXIT(DDS_BAD));
         *bias_ptr = bias;
      } else {
         for(member = (DDS_MEMBER*)block->first; 
            member; 
            member=(DDS_MEMBER*)member->newer) {
            if (member->flag & DDS_SYM_MEMBER) {
               SYM_TO_SYM_TAG(new_tag, &bin->sym_tag, member)
               SYM_TAG_TO_BIAS(bias_ptr, bin, new_tag, DDS_API_EXIT(DDS_BAD))
               *bias_ptr = 0;
               /* This is needed to setup the bias */
               tag = new_tag;
               while (tag >= 0) {
                  SYM_TAG_TO_SYM(sym, bin, tag, DDS_API_EXIT(DDS_BAD));
                  cname = sym->name;
                  if (!cname) cname = sym->newer->name;
                  if (!cname) break;
            
                  member = UNIQUE_MEMBER(&bin->symbol, cname, sym->block);
                  if (!member) break;

                  *bias_ptr += member->offset;
                  tag = cdds_parent(bin_tag, tag);
               }
               break;
            }
         }
      }
   }

   DDS_API_EXIT(new_tag)
}
