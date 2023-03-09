static char rcsid[]="$Id: dictcmd.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, open and initialize "cmd:" dictionary.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_DICT * ddsDictCmd(
   const char *mode,
   DDS_DICT_STATE dict_state,
   DDS_MEDIA_STATE media_state)
{
   DDS_DICT *dict;
   size_t n_size;
   int arg_n;
   const char *arg_v;

   dict = ddsDictNew("cmd:", dict_state, media_state,
      NULL, NULL, NULL, 0);

   if(! (dict_state & DDS_DICT_LOAD)) return dict;

   /* load command line arguments into dictionary */
   for(arg_n = 0, arg_v = "cmd_args= "; arg_v; arg_v = GetArgCmdV(++arg_n)) {
      n_size = strlen(arg_v);
      if(! n_size) continue;
      if(arg_n) {
         if(1 < n_size && strchr(arg_v, '='))
            ddsDictLoad(dict, &dict->load,
               NULL, "\n", 1);
         else
            ddsDictLoad(dict, &dict->load,
               NULL, " ", 1);
      }
      ddsDictLoad(dict, &dict->load,
         NULL, arg_v, n_size);
   }

   /* append final new line */
   ddsDictLoad(dict, &dict->load,
      NULL, "\n", 1);

   return dict;
}
