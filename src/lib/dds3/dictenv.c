static char rcsid[]="$Id: dictenv.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, open and initialize "env:" dictionary.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_DICT * ddsDictEnv(
   const char *mode,
   DDS_DICT_STATE dict_state,
   DDS_MEDIA_STATE media_state)
{
   DDS_DICT *dict;
   size_t n_size;
   int env_n;
   const char *env_v;

   dict = ddsDictNew("env:", dict_state, media_state,
      NULL, NULL, NULL, 0);

   if(! (dict_state & DDS_DICT_LOAD)) return dict;

   /* load environment variables into dictionary */
   env_n = -1;
   for(env_v = "DDS_ENVIRONMENT= xxx";
      env_v;
      env_v = GetArgEnvV(++env_n)) {
      n_size = strlen(env_v);
      if(! n_size) continue;
      ddsDictLoad(dict, &dict->load,
         NULL, env_v, n_size);
      ddsDictLoad(dict, &dict->load,
         NULL, "\n", 1);
   }

   return dict;
}
