static char rcsid[]="$Id: openslice.c 227 2011-05-04 20:08:56Z user $";
/*
RCS-t=DDS, open binary data, create sliced binary

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

#define TEST_FLAG (DDS_AXIS_SPARSE | DDS_AXIS_ASYNC | DDS_AXIS_local)

int ddsOpenSlice(
   DDS_BIN *bin,
   const char *data_name,
   const char *open_format,
   DDS_CUBE *old_cube)
{
   BIN_TAG slice_tag;
   DDS_BIN *slice_bin = NULL, *first_slice;
   DDS_TOKEN token_save = {{0, NULL}, NULL};
   DDS_AXIS *axis, *slice_axis;
   int rank, slice_rank, old_error_count;
   int slice_num;
   char *tmp;
   const char *slice_mode;
   const char *name, *value, *title = NULL;
   const char *slice_dict = NULL, *slice_fmt;

   /* Open the master binary (for the slices) as memory/buffer. */
   old_error_count = dds_error_count;
   slice_mode = bin->mode;
   bin->mode = ddsString("m");
   ddsOpenNew(bin, data_name, old_cube);

   if(dds_error_count > old_error_count) {
      /* The binary is really messed up bad. */
      goto cleanup;
   }

   /* increment slice level to disable print info for slice binaries */
   ddsSetSliceLevel(slice_mode, 1);

   bin->slice_tic = bin->trace.type->stroke;

   for(rank = bin->cube.io_rank; rank < bin->cube.rank; rank++) {
      axis = bin->cube.axis + rank;
      if(axis->flag & DDS_AXIS_SLICE) break;
      bin->slice_tic *= axis->size;
   }

   bin->slice_toc = bin->slice_tic;

   cdds_dict(bin->dict_open, "scan");

   /* Retrieve the "title" for this processing step. */
   /* It's needed when initializing multiple slice dictionaries. */
   value = "ddsOpenNewSlice: default title";
   cdds_scant("cmd_title", &value);
   title = ddsString(value);

   bin->slice = (DDS_BIN**)ddsZalloc(bin->slice_max * sizeof(DDS_BIN*));

   /* Open each slice of data... recursive ! */
   slice_num = 0;
   for(name = TOKEN_INIT(token_save, data_name);
      name;
      name = TOKEN_NEXT(token_save)) {

      if(*slice_mode == 'r') {
         slice_dict = cdds_in("", name, title);
      } else if(*slice_mode == 'w') {
         slice_dict = cdds_out("", name, bin->dict_open);
      } else {
         slice_dict = cdds_mod("", name, title, bin->dict_open);
      }

      if(! slice_dict) {
         ddsError("open: can't access slice dict= %s\n\tdict= %s\n",
            name, bin->dict_open);
         goto cleanup;
      }

      cdds_dict(bin->dict_open, "scan");

      cdds_dict(slice_dict, "print");

      if(bin->state & DDS_BIN_OPEN_CREATE) {
         /* Eliminate sliced axes from the list of axis names. */
         cdds_printf("axis", "");
         for(rank = 1; rank <= bin->cube.rank; rank++) {
            axis = bin->cube.axis + rank;
            if(rank < bin->cube.io_rank || ! (axis->flag & DDS_AXIS_SLICE)) {
               cdds_printf("", "  %s", axis->name);
            }
         }
         cdds_printf("", "\n");
      }

      if(bin->state & DDS_BIN_FORCE_FMT && !cdds_scanf(open_format,"")) {
         /* Explicit override for the binary format. */
         /* (strip off all "slice.xxx" tokens)       */
         cdds_printf(open_format,"");
         while(cdds_scant("",&value)>0) {
            if (strncmp(value,"slice.",6)) {
               cdds_printf("","%s ",value);
            }
         }
         cdds_printf("","\n");

         /* Reset sliced sizes to 1 */
         tmp = ddsMalloc(16);
         cdds_scanf(open_format,"");
         while(cdds_scant("",&value)>0) {
            if (!strncmp(value,"slice.",6)) {
               tmp = ddsRealloc(tmp,sizeof(value));
               value += 6;
               sprintf(tmp,"size.%s",value);
               cdds_printf(tmp,"1\n");               
            }
         }
         ddsFree(tmp);
         slice_fmt = open_format;
      } else {
         slice_fmt = "";
      }

      slice_tag = cdds_open(slice_dict, slice_fmt, "", slice_mode);

      if(slice_tag < 0) {
         ddsError("open: can't access slice data, dict= %s\n\tdict= %s\n",
            name, bin->dict_open);
         goto cleanup;
      }

      slice_bin = dds_bin[slice_tag];

      if((bin->state ^ slice_bin->state) & ~
        (DDS_BIN_OPEN_CREATE | DDS_BIN_CRAM |
         DDS_BIN_UNBLOCK | DDS_BIN_FILES |
         DDS_BIN_EOF)) {
         ddsWarn("open, inconsistent state, slice dict= %s\n\tdict= %s\n",
            slice_dict, bin->dict_open);
      }

      if(strcmp(bin->fmt_name, slice_bin->fmt_name)) {
         ddsWarn("open, inconsistent format, slice dict= %s\n\tdict= %s\n",
            slice_dict, bin->dict_open);
       /*  goto cleanup; */
      }

      if(slice_num) {
         /* This is NOT the first slice. */
         if(first_slice->media->genus != slice_bin->media->genus) {
            ddsError("open, inconsistent media genus, slice dict= %s\n\tdict= %s\n",
               slice_dict, bin->dict_open);
            goto cleanup;
         }
      } else {
         /* This IS the first slice, remember it. */
         first_slice = slice_bin;
      }

      /* Validate cube consistency for each shared axis. */
      slice_rank = slice_bin->cube.io_rank - 1;
      for(rank = bin->cube.io_rank; rank <= bin->cube.rank; rank++) {
         axis = bin->cube.axis + rank;

         if(axis->flag & DDS_AXIS_SLICE) {
            /* This rank is sliced (only in the master binary). */
            continue;
         }

         if(++slice_rank > slice_bin->cube.rank) {
            ddsError("open, slice cube rank too small, slice dict= %s\n\tdict= %s\n",
               slice_dict, bin->dict_open);
            goto cleanup;
         }

         slice_axis = slice_bin->cube.axis + slice_rank;

         if(strcmp(axis->name, slice_axis->name)) {
            ddsWarn("open, axis[%i] name= %s, slice axis[%i] name= %s\n\tslice dict= %s\n\tdict= %s\n",
               rank, axis->name, slice_rank, slice_axis->name,
               slice_dict, bin->dict_open);
         }

         if((slice_axis->flag & TEST_FLAG) != (axis->flag & TEST_FLAG)) {
            ddsError("open, inconsistent axis %s flags, slice dict= %s\n\tdict= %s\n",
               axis->name, slice_dict, bin->dict_open);
            goto cleanup;
         }

         if(axis->size != slice_axis->size) {
            ddsError("open, inconsistent axis %s size, slice dict= %s\n\tdict= %s\n",
               axis->name, slice_dict, bin->dict_open);
         }

         if(axis->origin != slice_axis->origin) {
            ddsWarn("open, inconsistent axis %s origin, slice dict= %s\n\tdict= %s\n",
               axis->name, slice_dict, bin->dict_open);
         }

         if(axis->delta != slice_axis->delta) {
            ddsWarn("open, inconsistent axis %s delta, slice dict= %s\n\tdict= %s\n",
               axis->name, slice_dict, bin->dict_open);
         }

         if(strcmp(axis->units, slice_axis->units)) {
            ddsWarn("open, inconsistent axis %s units, slice dict= %s\n\tdict= %s\n",
               axis->name, slice_dict, bin->dict_open);
         }

         if(axis->base != slice_axis->base) {
            ddsWarn("open, inconsistent axis %s base, slice dict= %s\n\tdict= %s\n",
               axis->name, slice_dict, bin->dict_open);
         }

         if(axis->step != slice_axis->step) {
            ddsWarn("open, inconsistent axis %s step, slice dict= %s\n\tdict= %s\n",
               axis->name, slice_dict, bin->dict_open);
         }

         if(strcmp(axis->sort_name, slice_axis->sort_name)) {
            ddsWarn("open, inconsistent axis %s sort_name, slice dict= %s\n\tdict= %s\n",
               axis->name, slice_dict, bin->dict_open);
         }
      }

      if(slice_rank < slice_bin->cube.rank && slice_rank >= 3) {
         ddsWarn("open, slice cube rank too big, slice dict= %s\n\tdict= %s\n",
            slice_dict, bin->dict_open);
      }

      if(! ddsSymSameType(bin->trace.type, slice_bin->trace.type)) {
         ddsDebug("\talign : %d %d\n\tprec  : %ld %ld\n\tcount : %ld %ld\n\tstroke: %ld %ld\n",
                  bin->trace.type->align, slice_bin->trace.type->align,
                  bin->trace.type->prec, slice_bin->trace.type->prec,
                  bin->trace.type->count, slice_bin->trace.type->count,
                  bin->trace.type->stroke, slice_bin->trace.type->stroke);
         ddsError("open, inconsistent trace type, slice dict= %s\n\tdict= %s\n",
            slice_dict, bin->dict_open);
         goto cleanup;
      }

      bin->slice[slice_num] = slice_bin;
      slice_num++;
      slice_bin->master = bin;
      slice_bin = NULL;
   }

   /* normal return */
   slice_dict = NULL;

   cleanup:
   /* decrement slice level to possibly enable print info for slice binaries */
   ddsSetSliceLevel(slice_mode, -1);

   if(slice_bin) cdds_close(slice_tag);
   if(slice_dict) cdds_dict(slice_dict, "reset");
   ddsStrFree(&token_save.str);
   ddsFree((void*)slice_mode);
   if(title) ddsFree((void*)title);

   return DDS_OK;
}
