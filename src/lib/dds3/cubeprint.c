static char rcsid[]="$Id: cubeprint.c 270 2014-03-13 19:28:00Z user $";
/*
RCS-t=DDS, update binary cube description

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

void ddsCubePrint(
   DDS_BIN *bin,
   DDS_CUBE *old_cube)
{
   static DDS_STRING alias = NULL_STRING;
   DDS_AXIS *axis, *old_axis;
   DDS_BLOCK *block;
   DDS_MEMBER *member;
   DDS_TYPE_GENUS genus;
   SYM_TAG tag;
   DDS_BOOLEAN non_struct = DDS_FALSE;
#if defined(_WIN32) || defined(MINGW)
   size_t *bias_ptr; 
#else
   long *bias_ptr;
#endif
   int i, j, n, rank_max;
   double scale;
   const char *name;

   cdds_dict(bin->dict_open, "print");

   rank_max = bin->cube.rank;

   for(j=1;j<=rank_max;j++) {
      if(bin->cube.rank != old_cube->rank ||
         strcmp(bin->cube.axis[j].name, old_cube->axis[j].name)) {
         cdds_printf("axis", "");
         for(n = 1; n <= rank_max; n++)
            cdds_printf("", "  %s", bin->cube.axis[n].name);
         cdds_printf("", "\n");
         break;
      }
   }

   if(bin->cube.io_rank != old_cube->io_rank) {
      cdds_printf("io_rank", " %i\n", bin->cube.io_rank);
   }

   for(n=1;n<=rank_max;n++) {
      axis = bin->cube.axis + n;

      old_axis = NULL;
      for(j=1;j<=old_cube->rank;j++) {
         if(! strcmp(axis->name, old_cube->axis[j].name)) {
            /* cross reference old and new cubes */
            old_axis = old_cube->axis + j;
            break;
         }
      }

      if(! strcmp(axis->name, "t") && axis->units) {
         /* Normalize time coordinates to seconds. */
         scale = cdds_scale("sec", axis->units);

         if(scale) {
            if(strcmp(axis->units, "sec")) {
               ddsFree((char*)axis->units);
            }
            axis->units = ddsString("sec");
         } else {
            ddsWarn("invalid units.t= %s\n\t%s\n",
               axis->units, bin->media->title.ptr);
            scale = 1.;
         }

         axis->origin *= scale;
         axis->delta *= scale;

/*** what about "grid.t= T1 T2 ... Tn" discrete values ? ***/

      }

      if(axis->flag & DDS_AXIS_size) {
         /* size may need to be defined. */
         if(! old_axis || ! (old_axis->flag & DDS_AXIS_size) ||
            old_axis->size != axis->size) {
            ddsStrCpyCat2(&alias, "size.", axis->name);
            cdds_printf(alias.ptr, " %li\n", axis->size);
         }
      }

      ddsStrCpyCat2(&alias, "origin.", axis->name);
      if(axis->flag & DDS_AXIS_origin) {
         /* origin may need to be defined. */
         if(! old_axis || ! (old_axis->flag & DDS_AXIS_origin) ||
            old_axis->origin != axis->origin) {
            cdds_printf(alias.ptr, " %.16g\n", axis->origin);
         }
      } else {
         cdds_printf(alias.ptr, " 0.\n");
      }

      ddsStrCpyCat2(&alias, "delta.", axis->name);
      if(axis->flag & DDS_AXIS_delta) {
         /* delta may need to be defined. */
         if(! old_axis || ! (old_axis->flag & DDS_AXIS_delta) ||
            old_axis->delta != axis->delta) {
            cdds_printf(alias.ptr, " %.16g\n", axis->delta);
         }
      } else {
         cdds_printf(alias.ptr, " 1.\n");
      }

      if(axis->units) {
         /* units may need to be defined. */
         if(! old_axis || ! old_axis->units ||
            strcmp(old_axis->units, axis->units)) {
            ddsStrCpyCat2(&alias, "units.", axis->name);
            cdds_printf(alias.ptr, " %s\n", axis->units);
         }
      } else {
         ddsStrCpyCat2(&alias, "units.", axis->name);
         cdds_printf(alias.ptr, " ?\n");
      } 

      if(axis->flag & DDS_AXIS_base) {
         /* base may need to be defined. */
         if(! old_axis || ! (old_axis->flag & DDS_AXIS_base) ||
            old_axis->base != axis->base) {
            ddsStrCpyCat2(&alias, "base.", axis->name);
            cdds_printf(alias.ptr, " %.16g\n", axis->base);
         }
      } else if(old_axis && (old_axis->flag & DDS_AXIS_base)) {
         /* base must be undefined. */
         ddsStrCpyCat2(&alias, "$base.", axis->name);
         cdds_printf(alias.ptr, " \n");
      }

      if(axis->flag & DDS_AXIS_step) {
         /* step may need to be defined. */
         if(! old_axis || ! (old_axis->flag & DDS_AXIS_step) ||
            old_axis->step != axis->step) {
            ddsStrCpyCat2(&alias, "step.", axis->name);
            cdds_printf(alias.ptr, " %.16g\n", axis->step);
         }
      } else if(old_axis && (old_axis->flag & DDS_AXIS_step)) {
         /* step must be undefined. */
         ddsStrCpyCat2(&alias, "$step.", axis->name);
         cdds_printf(alias.ptr, " \n");
      }

      name = axis->sort_name;

      if(name) {
         /* sort_name may need to be defined. */
         if(bin->trace.type->ptype == DDS_STYPE_STRUCT) {
            /* trace type is a structure */
            block = bin->trace.type->block;
            member = ddsSymLookupMem(dds_symbol, name, block,
               &axis->sort.offset);

            if(member) {
               genus = dds_prime[member->type->ptype].genus;
               if((1 << genus) & DDS_ARITHMETIC_GENUS) {
                  /* sort field exists within trace structure */
                  ddsLinkMember(&axis->sort, block, name);
                  axis->flag |= DDS_AXIS_SORT;
               } else {
                  ddsWarn("sort field type is bad, name= %s\n\tbinary= %s\n",
                     name, bin->name);
                  name = ddsString("?");
               }
               SYM_TO_SYM_TAG(tag, &dds_compile->sym_tag, member)
               SYM_TAG_TO_BIAS(bias_ptr, dds_compile, tag, ; )
               *bias_ptr = axis->sort.offset;
            } else if(*name != '?') {
               ddsVerbose("sort field does not exist, name= %s\n\tbinary= %s\n",
                  name, bin->name);
               name = ddsString("?");
            }
         } else {
            non_struct = DDS_TRUE;
            name = ddsString("?");
         }

         if(name != axis->sort_name) {
            ddsFree((char*)axis->sort_name);
            axis->sort_name = name;
         }

         if(! old_axis || ! old_axis->sort_name ||
            strcmp(old_axis->sort_name, name)) {
            ddsStrCpyCat2(&alias, "sort.", axis->name);
            cdds_printf(alias.ptr, " %s\n", name);
         }
      } else {
         ddsWarn("sort.%s not defined!\n", axis->name);
      }

      if(axis->ngrid) {
         /* units may need to be defined. */
         if(! old_axis || old_axis->ngrid != axis->ngrid) {
            ddsStrCpyCat2(&alias, "grid.", axis->name);
            cdds_printf(alias.ptr, "");
            for(i=0;i<axis->ngrid;i++) {
               cdds_printf("", "%.16g ", axis->grid[i]);
            }
            cdds_printf("", "\n");
         } else for(i=0;i<axis->ngrid;i++) {
            if (axis->grid[i] != old_axis->grid[i]) {
               ddsStrCpyCat2(&alias, "grid.", axis->name);
               cdds_printf(alias.ptr, "");
               for(j=0;j<axis->ngrid;j++) {
                  cdds_printf("", "%.16g ", axis->grid[j]);
               }
               cdds_printf("", "\n");
               break;
            }
         }
      } 

      /* only keep global and local flag bits (clear overloaded bits) */
      axis->flag &= (DDS_AXIS_FLAGS) (DDS_AXIS_global | DDS_AXIS_local);
   }

   if(non_struct) {
      ddsWarn("sort fields are named and for a non-struct\n\tbinary= %s\n",
         bin->name);
   }

   return;
}
