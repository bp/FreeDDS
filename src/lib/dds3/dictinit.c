static char rcsid[]="$Id: dictinit.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, open and initialize one dictionary.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_DICT * ddsDictInit(
   const char *name,
   const char *mode)
{
   DDS_MEDIA_STATE media_state;
   DDS_DICT_STATE dict_state;
   DDS_DICT *dict;
   const char *s;
   size_t name_size;
   int name_fd;
   char name_colon, name_null;

   for(s = name; *s; s++) {
      /* to avoid chaos, characters in dictionary names must be...
         1) printable, 2) excluding "=".
      */
      if(! isgraph(*s) || '=' == *s) {
         ddsError("dict: dictionary name contains bad characters\n\tname= %s\n",
            ddsHint(name));
         return NULL;
      }
   }

   dict_state = (DDS_DICT_STATE)0;
   media_state = (DDS_MEDIA_STATE)0;

   if(! strncmp("scan", mode, 4)) {
      dict_state |= DDS_DICT_LOAD;
      if(strstr(mode, "only")) {
         dict_state |= DDS_DICT_READ_ONLY;
         media_state |= DDS_MEDIA_OPENR;
      } else {
         media_state |= (DDS_MEDIA_STATE)(DDS_MEDIA_OPENR | DDS_MEDIA_OPENW);
      }
   } else if(! strncmp("print", mode, 5)) {
      media_state |= (DDS_MEDIA_STATE)(DDS_MEDIA_OPENR | DDS_MEDIA_OPENW);
      if(strstr(mode, "keep")) {
         dict_state |= DDS_DICT_LOAD;
      } else {
         dict_state |= DDS_DICT_TRUNCATE;
      }
   } else {
      dict_state |= DDS_DICT_LOAD;
      media_state |= (DDS_MEDIA_STATE)(DDS_MEDIA_OPENR | DDS_MEDIA_OPENW);
   }

   if(strstr(mode, "truncate")) {
      if(strstr(mode, "keep") ||
         (dict_state & DDS_DICT_READ_ONLY)) {
         ddsDebug("dict: conflicting open keywords, mode=%s\n\tdictionary= %s\n",
            mode, name);
         return NULL;
      }
      dict_state |= DDS_DICT_LOAD; /* fail-safe */
      dict_state &= (DDS_DICT_STATE) ~ (DDS_DICT_TRUNCATE); /* fail-safe */
   }

   name_size = strlen(name);

   if(! strcmp("stdin:", name)) {
      dict = ddsDictStdin(mode, dict_state, media_state);
   } else if(! strcmp("stdout:", name)) {
      dict = ddsDictStdout(mode, dict_state, media_state);
   } else if(! strcmp("stderr:", name)) {
      dict = ddsDictStderr(mode, dict_state, media_state);

   } else if(! strcmp("cmd:", name)) {
      dict = ddsDictCmd(mode, dict_state, media_state);
   } else if(! strcmp("par:", name)) {
      dict = ddsDictPar(mode, dict_state, media_state);
   } else if(! strcmp("env:", name)) {
      dict = ddsDictEnv(mode, dict_state, media_state);
   } else if(! strcmp("log:", name)) {
      dict = ddsDictLog(mode, dict_state, media_state);

   } else if(! strcmp("device:", name)) {
      dict = ddsDictDevice(mode, dict_state, media_state);

   } else if(! strcmp("asp.fmt:", name)) {
      dict = ddsDictAspFmt(mode, dict_state, media_state);
   } else if(! strcmp("asp.map:", name)) {
      dict = ddsDictAspMap(mode, dict_state, media_state);

   } else if(! strcmp("usp.fmt:", name)) {
      dict = ddsDictUspFmt(mode, dict_state, media_state);
   } else if(! strcmp("usp.map:", name)) {
      dict = ddsDictUspMap(mode, dict_state, media_state);

   } else if(! strcmp("segy.fmt:", name)) {
      dict = ddsDictSegyFmt(mode, dict_state, media_state);
   } else if(! strcmp("segy.map:", name)) {
      dict = ddsDictSegyMap(mode, dict_state, media_state);

   } else if(! strcmp("segy1.fmt:", name)) {
      dict = ddsDictSegy1Fmt(mode, dict_state, media_state);
   } else if(! strcmp("segy1.map:", name)) {
      dict = ddsDictSegy1Map(mode, dict_state, media_state);

   } else if(! strcmp("default:", name)) {
      dict = ddsDictTmp("default:", mode,
         (DDS_DICT_STATE)(dict_state | DDS_DICT_MAP));
   } else if(! strcmp("override:", name)) {
      dict = ddsDictTmp("override:", mode,
         (DDS_DICT_STATE)(dict_state | DDS_DICT_MAP));

   } else if(! strncmp("tmp", name, 3) &&
      ':' == name[name_size - 1]) {
      dict = ddsDictTmp(name, mode, dict_state);
   } else if(2 == sscanf(name, "fd%d%c%c",
         &name_fd, &name_colon, &name_null) &&
      name_colon == ':') {
      dict = ddsDictFd(name, mode, dict_state, media_state);
   } else {
      dict = ddsDictFile(name, mode, dict_state, media_state);
   }

   return dict;
}
