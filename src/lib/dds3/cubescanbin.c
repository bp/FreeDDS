static char rcsid[]="$Id: cubescanbin.c 67 2009-03-06 23:03:49Z user $";
/*
RCS-t=DDS, retrieve and default cube description

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

/********************************************************************/
/***  OVERVIEW:                                                   ***/
/***                                                              ***/
/***  ddsCubeScanBin... default cube description                  ***/
/***     ddsCubeScanAxis...     "size, origin, delta, ..."        ***/
/***     ScanFmtFlags...        "format= ... flags"               ***/
/***     ScanEditFlags...       "edit:...= flags"                 ***/
/***  ddsCubeScanEnd... final cube description                    ***/
/***     ddsCubeScanAxis...     "size, origin, delta, ..."        ***/
/***     ddsReformName...       "sort:...= <hdr_name>"            ***/
/********************************************************************/

static void ScanFmtFlags(
   int rank,
   const char *name,
   DDS_AXIS_FLAGS *flags,
   DDS_STRING *alias)
{
   /***************************************************************/
   /* The format may contain modifier for SPARSE and ASYNC flags. */
   /*    Example: "format= usp dense.2 sync.x" implies            */
   /*       1) format is "usp"                                    */
   /*       2) axis "2" (if it exists) is dense.                  */
   /*       3) axis "x" (if it exists) is synchronous.            */
   /***************************************************************/
   char axis_num[32];

   sprintf(axis_num, "%i", rank);

   ddsStrCpyCat4(alias, "dense.", name, " dense.", axis_num);
   if(0 < cdds_scank("format", alias->ptr))
      *flags &= (DDS_AXIS_FLAGS) ~ DDS_AXIS_SPARSE;

   ddsStrCpyCat4(alias, "sync.", name, " sync.", axis_num);
   if(0 < cdds_scank("format", alias->ptr))
      *flags &= (DDS_AXIS_FLAGS) ~ DDS_AXIS_ASYNC;

   ddsStrCpyCat4(alias, "slice.", name, " slice.", axis_num);
   if(0 < cdds_scank("format", alias->ptr))
      *flags |= DDS_AXIS_SLICE;

   return;
}

static void ScanEditFlags(
   DDS_BIN *bin,
   const char *name)
{
   static DDS_STRING key = NULL_STRING;
   DDS_AXIS *axis;
   char axis_num[32];
   int n;

   if(cdds_scank(name, "no_edit")) {
      for(n = bin->cube.io_rank; n <= bin->cube.rank; n++) {
         axis = bin->cube.axis + n;
         axis->flag |= (DDS_AXIS_FLAGS)(DDS_AXIS_SPARSE | DDS_AXIS_ASYNC);
      }
      return;
   }

   if(cdds_scank(name, "zero_dead")) {
      bin->state |= DDS_zero_dead;
   }

   if(cdds_scank(name, "drop_dead")) {
      bin->state |= DDS_drop_dead;
   }

   if(cdds_scank(name, "drop_live")) {
      bin->state |= DDS_drop_live;
   }

   if(cdds_scank(name, "drop_aux")) {
      bin->state |= DDS_drop_aux;
   }

   for(n = bin->cube.io_rank; n <= bin->cube.rank; n++) {
      axis = bin->cube.axis + n;

      sprintf(axis_num, "%i", n);

      ddsStrCpyCat4(&key, "dense.", axis->name, " dense.", axis_num);
      if(cdds_scank(name, key.ptr)) {
         axis->flag &= (DDS_AXIS_FLAGS) ~ DDS_AXIS_SPARSE;
      }

      ddsStrCpyCat4(&key, "sparse.", axis->name, " sparse.", axis_num);
      if(cdds_scank(name, key.ptr)) {
         axis->flag |= DDS_AXIS_SPARSE;
      }

      ddsStrCpyCat4(&key, "sync.", axis->name, " sync.", axis_num);
      if(cdds_scank(name, key.ptr)) {
         axis->flag &= (DDS_AXIS_FLAGS) ~ DDS_AXIS_ASYNC;
      }

      ddsStrCpyCat4(&key, "async.", axis->name, " async.", axis_num);
      if(cdds_scank(name, key.ptr)) {
         axis->flag |= DDS_AXIS_ASYNC;
      }

      ddsStrCpyCat4(&key, "upper.", axis->name, " upper.", axis_num);
      if(cdds_scank(name, key.ptr)) {
         axis->flag &= (DDS_AXIS_FLAGS) ~ DDS_AXIS_UPPER;
      }

      ddsStrCpyCat4(&key, "window.", axis->name, " window.", axis_num);
      if(cdds_scank(name, key.ptr)) {
         axis->flag &= (DDS_AXIS_FLAGS) ~ DDS_AXIS_WINDOW;
      }

      ddsStrCpyCat4(&key, "lower.", axis->name, " lower.", axis_num);
      if(cdds_scank(name, key.ptr)) {
         axis->flag &= (DDS_AXIS_FLAGS) ~ DDS_AXIS_LOWER;
      }

      ddsStrCpyCat4(&key, "error.", axis->name, " error.", axis_num);
      if(cdds_scank(name, key.ptr)) {
         axis->flag |= DDS_AXIS_ERROR;
      }

      ddsStrCpyCat4(&key, "warn.", axis->name, " warn.", axis_num);
      if(cdds_scank(name, key.ptr)) {
         axis->flag |= DDS_AXIS_WARN;
      }

      ddsStrCpyCat4(&key, "force.", axis->name, " force.", axis_num);
      if(cdds_scank(name, key.ptr)) {
         axis->flag |= DDS_AXIS_FORCE;
      }

      ddsStrCpyCat4(&key, "reject.", axis->name, " reject.", axis_num);
      if(cdds_scank(name, key.ptr)) {
         axis->flag |= DDS_AXIS_REJECT;
      }

      ddsStrCpyCat4(&key, "pad.", axis->name, " pad.", axis_num);
      if(cdds_scank(name, key.ptr)) {
         axis->flag |= DDS_AXIS_PAD;
         axis->flag &= (DDS_AXIS_FLAGS) ~ DDS_AXIS_ASYNC;
      }

      ddsStrCpyCat4(&key, "apad.", axis->name, " apad.", axis_num);
      if(cdds_scank(name, key.ptr)) {
         axis->flag |= DDS_AXIS_PAD;
      }
   }

   if(strncmp(bin->dict_open, dds_scan_dict->name,
      strlen(dds_scan_dict->name))) {
      /* copy the edit definition to the output history. */
      cdds_dict(bin->dict_open, "print");
      cdds_copy(name);
   }

   return;
}

void ddsCubeScanBin(
   DDS_BIN *bin,
   DDS_CUBE *old_cube)
{
   static DDS_STRING alias = NULL_STRING;
   DDS_AXIS *axis, *old_axis, *def_axis;
   DDS_CUBE *def_cube;
   int n, j;
   const char *s, *str, *name;
   char tmp[AXISNAME_MAX];

   /***************************************************/
   /* Scan the history for existing cube description. */
   /* Its used to minimize the dictionary update.     */
   /***************************************************/

   if(bin->cube.axis) return; /* already done */

   def_cube = bin->def_cube;
   cdds_dict(bin->dict_open, "scan");

   /* Count the number of axes in the existing cube. */
   old_cube->rank = cdds_scank("axis", "");
   if(old_cube->rank < 0) old_cube->rank = 0;

   /* Malloc axis descriptor for existing cube. */
   old_cube->axis = (DDS_AXIS*)ddsZalloc((
      (old_cube->rank + 1) * sizeof(DDS_AXIS)));

   /* Retrieve existing io_rank (first axis counted by i/o). */
   old_cube->io_rank = -1;
   cdds_scanf("io_rank", "%d", &old_cube->io_rank);

   /* Retrieve axis names and validate name syntax. */
   cdds_scanf("axis", "");

   for(n=1;n<=old_cube->rank;n++) {
      old_axis = old_cube->axis + n;
      cdds_scant("", &name);
      for(s=name;*s;s++) {
         if(! isgraph(*s) || *s == '=') {
            sprintf(tmp, "?%i", n);
            name = tmp;
            break;
         }
      }
      old_axis->name = ddsString(name);
   }

   /* Retrieve old cube description */
   /* (set SPARSE flag to not check any grid defn) */
   for(n=1;n<=old_cube->rank;n++) {
      axis = old_cube->axis + n;
      axis->flag |= (DDS_AXIS_FLAGS)(DDS_AXIS_SPARSE);
      ddsCubeScanAxis(axis, &alias);
   }

   /********************************************************/
   /* Scan fmt_dict for the cube description being opened. */
   /********************************************************/

   cdds_dict(bin->dict_fmt, "scan");

   /* Compute the number of axes in the new cube. */
   if(old_cube->rank < def_cube->rank)
      bin->cube.rank = def_cube->rank;
   else
      bin->cube.rank = old_cube->rank;

   /* Malloc axis descriptor for new cubes. */
   bin->cube.axis = (DDS_AXIS*)ddsZalloc((
      (bin->cube.rank + 1) * sizeof(DDS_AXIS)));
 
   /* Determine the new io_rank */
   if(old_cube->io_rank > 0) {
      /* Default provided by old cube. */
      bin->cube.io_rank = old_cube->io_rank;
   } else if(def_cube->io_rank > 0) {
      /* Default provided by format's minimum io_rank. */
      bin->cube.io_rank = def_cube->io_rank;
   } else {
      /* Nominal default. */
      bin->cube.io_rank = 2;
   }

   cdds_scanf("io_rank", "%d", &bin->cube.io_rank);

   if(bin->cube.io_rank < def_cube->io_rank) {
      ddsError("cube io rank (%i) is less than minimum (%i)\n",
         bin->cube.io_rank, def_cube->io_rank);
      bin->cube.io_rank = def_cube->io_rank;
   }

   /* Retrieve axis names and validate name syntax. */
   cdds_scanf("axis", "");

   for(n=1;n<=bin->cube.rank;n++) {
      axis = bin->cube.axis + n;

      if(n <= old_cube->rank &&
         old_cube->axis[n].name) {
         name = old_cube->axis[n].name;
      } else if(n <= def_cube->rank &&
         def_cube->axis[n].name) {
         name = def_cube->axis[n].name;
      } else {
         name = "";
      }

      if(*name) {
         axis->name = ddsString(name);
      } else {
         sprintf(tmp, "?%i", n);
         axis->name = ddsString(tmp);
      }

      for(j=1;j<n;j++) {
         if(! strcmp(bin->cube.axis[j].name, axis->name)) {
            ddsWarn("axis %i name is the same as %i (%s)\nrenaming it to (?%i)\n",
                    n, j, axis->name, n);
            ddsFree((void*)axis->name);
            sprintf(tmp, "?%i", n);
            axis->name = ddsString(tmp);
            break;
         }
      }
   }

   /**********************************/
   /* Retrieve new cube description. */
   /**********************************/

   /* Default number of data slices (multiple files). */
   bin->slice_max = 1;

   for(n=1;n<=bin->cube.rank;n++) {
      axis = bin->cube.axis + n;

      /* apply defaults provided by format */
      if(n <= def_cube->rank) {
         /* unique default for axis N */
         def_axis = def_cube->axis + n;
      } else {
         /* generic default uses axis 0 */
         def_axis = def_cube->axis;
      }

      axis->flag = def_axis->flag;
      axis->size = def_axis->size;
      axis->origin = def_axis->origin;
      axis->delta = def_axis->delta;

      axis->base = def_axis->base;
      axis->step = def_axis->step;

      ScanFmtFlags(n, axis->name, &axis->flag, &alias);

      ddsCubeScanAxis(axis, &alias);

      if(n >= bin->cube.io_rank && (axis->flag & DDS_AXIS_SLICE)) {
         /* accumulate the total number of sliced files. */
         bin->slice_max *= axis->size;
      }

      if(! axis->units && def_axis->units)
         axis->units = ddsString(def_axis->units);

      if(! axis->sort_name && def_axis->sort_name)
        axis->sort_name = ddsString(def_axis->sort_name);

      if(! axis->ngrid && def_axis->ngrid) {
         axis->ngrid = def_axis->ngrid;
         axis->grid = (double*)ddsMalloc(axis->ngrid*sizeof(double));
         for(j=0;j<axis->ngrid;j++) axis->grid[j] = def_axis->grid[j];
      }

      if(axis->flag & DDS_AXIS_FORCE) {
         axis->flag &= (DDS_AXIS_FLAGS) ~ (DDS_AXIS_REJECT | DDS_AXIS_PAD);
      }

      
      if((bin->state & DDS_BIN_OPEN_CREATE) &&
         axis->sort_name &&
         bin->old_format[0]) {
         /**********************************************************/
         /* Mutate the sort field name from old to new.            */
         /* Example: if the format changes from segy to usp,       */
         /* change SEQNO and SHOT sort names to TrcNum and PrRcNm. */
         /**********************************************************/
         str = ddsReformName(n, bin->fmt_name, axis->name,
            axis->sort_name, bin->old_format);

         if(strchr(str, '?')) {
            /* nominal sort field name is bogus, use default instead */
            str = def_axis->sort_name;
         }

         if(strcmp(str, axis->sort_name)) {
            /* Name of sort field DOES change. */
            ddsFree((void*)axis->sort_name);
            axis->sort_name = ddsString(str);
         }
      }
   }

   ddsStrCpyCat3(&alias, DDS_SENTRY, " edit:* edit:", bin->alias_name);

   if(! cdds_scanf(alias.ptr, "") && strcmp(dds_scan_name, DDS_SENTRY)) {
      ScanEditFlags(bin, alias.ptr);
      cdds_dict(bin->dict_fmt, "scan");
   }

   cdds_dict(bin->dict_open, "scan");

   return;
}
