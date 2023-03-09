static char rcsid[]="$Id: dicttmp.c 112 2009-09-03 22:10:35Z user $";
/*
RCS-t=DDS, open and initialize "tmpxxx:" dictionary.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

/* 
Musa Maharramov 260809

Dropped tmpnam()
Switched to gopen_tempfile() from libgio for temp file creation
*/

#include "dds_sys.h"


DDS_DICT * ddsDictTmp(
   const char *name,
   const char *mode,
   DDS_DICT_STATE dict_state)
{
   const char * media_name;
   DDS_DICT *dict;
   DDS_MEDIA_STATE media_state;
   GFILE *media_fp;

   if(dict_state & DDS_DICT_READ_ONLY) {
      ddsDebug("dict: errno= %i, %s\n\topening '%s' dictionary, mode= %s\n",
         EACCES, strerror(EACCES), name, mode);
      return NULL;
   }

   if(dds_state & DDS_SAVE_TMP) {
      media_state = (DDS_MEDIA_STATE)(DDS_MEDIA_OPENR | DDS_MEDIA_OPENW);
    
      media_fp = gopen_tempfile(&media_name, "cw+");
    
      if(! media_fp) {
         /* panic (not error), if temporary files can't be accessed */
         ddsPanic("dict: errno= %i, %s\n\topening '%s' (%s) dictionary, mode= %s\n",
         errno, strerror(errno), name, media_name, mode);
      }
      ddsVerbose("dictionary '%s', real filename %s\n", name, media_name);
   } else {
      media_name = NULL;
      media_fp = NULL;
      media_state = (DDS_MEDIA_STATE)0;
   }

   dict = ddsDictNew(name, dict_state, media_state,
      media_name, media_fp, NULL, 0);

   return dict;
}
