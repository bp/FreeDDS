static char rcsid[]="$Id: binmessage.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, print pending "bin" message and begin another

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

void ddsBinMessage(
   DDS_BIN *bin,
   DDS_BIN_STATE setup)
{
   const char *mode;

   if(bin->state & DDS_BIN_MSG_READ) {
      mode = "read";
   } else if(bin->state & DDS_BIN_MSG_WRITE) {
      mode = "write";
   } else if(bin->state & DDS_BIN_MSG_SEEK) {
      mode = "seek";
   } else {
      mode = NULL;
   }

   if(mode) {
      if(bin->count_gross || bin->count_warn ||
         bin->count_force || bin->count_reject ||
         bin->count_dead || bin->count_fail) {
         ddsWarn("%s summary, LineTrc= %lu thru %lu, ReelTrc= %lu, file= %i, FileTrc= %lu\n\tgross= %i, warn= %i, force= %i, reject= %i, dead= %i, fail= %i\n\t%s\n",
            mode,
            (unsigned long)bin->MsgTrc,
            (unsigned long)(bin->LineTrc - 1),
            (unsigned long)(bin->ReelTrc - 1),
            (int)bin->media->file_count,
            (unsigned long)(bin->FileTrc - 1),
            bin->count_gross, bin->count_warn,
            bin->count_force, bin->count_reject,
            bin->count_dead, bin->count_fail,
            bin->media->title.ptr);
      } else if(bin->FileTrc > 1) {
         ddsVerbose("%s summary, LineTrc= %lu, ReelTrc= %lu, file= %i, FileTrc= %lu\n\t%s\n",
            mode,
            (unsigned long)(bin->LineTrc - 1),
            (unsigned long)(bin->ReelTrc - 1),
            (int)bin->media->file_count,
            (unsigned long)(bin->FileTrc - 1),
            bin->media->title.ptr);
      }
   }

   bin->state = (DDS_BIN_STATE)(setup | (bin->state &
      ~ (DDS_BIN_MSG_READ | DDS_BIN_MSG_WRITE | DDS_BIN_MSG_SEEK)));

   bin->MsgTrc = bin->LineTrc;

   bin->count_gross = 0;
   bin->count_warn = 0;
   bin->count_force = 0;
   bin->count_reject = 0;
   bin->count_dead = 0;
   bin->count_fail = 0;

   return;
}
