static char rcsid[]="$Id: cubescaninit.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, Initialize default cube description

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

static DDS_AXIS segy_axis[4] = {
   {DDS_FLAG_OSA,  NULL,  0,  0.,  1.,  "?",  1.,  1.,  "?",           0, NULL},
   {DDS_FLAG_O,     "t",  1,  0.,  1.,  "?",  1.,  1.,  "?",           0, NULL},
   {DDS_FLAG_OSA,   "x",  1,  0.,  1.,  "?",  1.,  1.,  "CdpTrcNum",   0, NULL},
   {DDS_FLAG_OSA,   "y",  1,  0.,  1.,  "?",  1.,  1.,  "FieldRecNum", 0, NULL}
};
static DDS_CUBE segy_cube = {3, 2, 0, 0, segy_axis};

static DDS_AXIS usp_axis[4] = {
   {DDS_FLAG_OSA,  NULL,  0,  0.,  1.,  "?",  1.,  1.,  "?",      0, NULL},
   {DDS_FLAG_O,     "t",  1,  0.,  1.,  "?",  1.,  1.,  "?",      0, NULL},
   {DDS_FLAG_OSA,   "x",  1,  0.,  1.,  "?",  1.,  1.,  "TrcNum", 0, NULL},
   {DDS_FLAG_OSA,   "y",  1,  0.,  1.,  "?",  1.,  1.,  "RecNum", 0, NULL}
};
static DDS_CUBE usp_cube = {3, 2, 0, 0, usp_axis};

static DDS_AXIS asp_axis[2] = {
   {DDS_FLAG_OSA,  NULL,  0,  0.,  1.,  "?",  1.,  1.,  "?", 0, NULL},
   {DDS_FLAG_O,    NULL,  1,  0.,  1.,  "?",  1.,  1.,  "?", 0, NULL}
};
static DDS_CUBE asp_cube = {1, 2, 0, 0, asp_axis};

void ddsCubeScanInit(
   DDS_BIN *bin)
{
   static DDS_STRING dict_load = NULL_STRING;
   static DDS_STRING fmt_filename = NULL_STRING;
   static DDS_STRING map_filename = NULL_STRING;
   DDS_CUBE *def_cube;
   const char *filename;
   const char *fmt_default, *map_default;
   int rank;

   /* Tweak the cube defaults based upon create/exist flags */
   /* and the format of the binary data. */
   cdds_dict(bin->dict_open, "scan");

   if(bin->state & DDS_BIN_OPEN_CREATE) {
      /* Create and open new binary data. */
      if(! strcmp(bin->fmt_name, "segy")) {
         bin->fmt = DDS_FMT_SEGY;
         rank = 3;
      } else if(! strcmp(bin->fmt_name, "segy1")) {
         bin->fmt = DDS_FMT_SEGY1;
         rank = 3;
      } else if(! strcmp(bin->fmt_name, "su")) {
         bin->fmt = DDS_FMT_SU;
         rank = 3;
      } else if(! strcmp(bin->fmt_name, "usp")) {
         bin->fmt = DDS_FMT_USP;
         rank = 3;
      } else {
         bin->fmt = DDS_FMT_GENERIC;
         rank = 1;
      }
   } else {
      /* Open existing binary data. */
      /* The existing rank may constrain the default cube. */
      rank = cdds_scank("axis", "");

      if(! strcmp(bin->fmt_name, "segy")) {
         bin->fmt = DDS_FMT_SEGY;
         if(rank < 1 || rank > 3) rank = 3;
      } else if(! strcmp(bin->fmt_name, "segy1")) {
         bin->fmt = DDS_FMT_SEGY1;
         if(rank < 1 || rank > 3) rank = 3;
      } else if(! strcmp(bin->fmt_name, "su")) {
         bin->fmt = DDS_FMT_SU;
         if(rank < 1 || rank > 3) rank = 3;
      } else if(! strcmp(bin->fmt_name, "usp")) {
         bin->fmt = DDS_FMT_USP;
         if(rank < 1 || rank > 3) rank = 3;
      } else {
         bin->fmt = DDS_FMT_GENERIC;
         rank = 1;
      }
   }

   switch(bin->fmt) {
   case DDS_FMT_SEGY:
   case DDS_FMT_SU:
      def_cube = &segy_cube;
      def_cube->rank = rank;
      def_cube->axis[3].size = 1;
      ddsStrCpy(&fmt_filename, "segy.fmt.dds");
      ddsStrCpy(&map_filename, "segy.map.dds");
      fmt_default = "segy.fmt:";
      map_default = "segy.map:";
      break;
   case DDS_FMT_SEGY1:
      def_cube = &segy_cube;
      def_cube->rank = rank;
      def_cube->axis[3].size = 1;
      ddsStrCpy(&fmt_filename, "segy1.fmt.dds");
      ddsStrCpy(&map_filename, "segy1.map.dds");
      fmt_default = "segy1.fmt:";
      map_default = "segy1.map:";
      break;
   case DDS_FMT_USP:
      def_cube = &usp_cube;
      def_cube->rank = rank;
      ddsStrCpy(&fmt_filename, "usp.fmt.dds");
      ddsStrCpy(&map_filename, "usp.map.dds");
      fmt_default = "usp.fmt:";
      map_default = "usp.map:";
      break;
   case DDS_FMT_GENERIC:
      def_cube = &asp_cube;
      def_cube->rank = rank;
      ddsStrCpyCat2(&fmt_filename, bin->fmt_name, ".fmt.dds");
      ddsStrCpyCat2(&map_filename, bin->fmt_name, ".map.dds");
      fmt_default = "asp.fmt:";
      map_default = "asp.map:";
      break;
   }

   bin->def_cube = def_cube;

   /* create dict_fmt load dictionary list */
   ddsStrCpyCat3(&dict_load, "override: ", bin->dict_open, " default: ");
   filename = ddsFindDDSFile(fmt_filename.ptr, bin->dict_open, fmt_default);
   if(filename) ddsStrCat2(&dict_load, " ", filename);
   bin->dict_fmt = ddsString(dict_load.ptr);

   /* create dict_map load dictionary list */
   ddsStrCpyCat3(&dict_load, "override: ", bin->dict_open, " default: ");
   filename =  ddsFindDDSFile(map_filename.ptr, bin->dict_open, map_default);
   if(filename) ddsStrCat2(&dict_load, " ", filename);
   bin->dict_map = ddsString(dict_load.ptr);

   return;
}
