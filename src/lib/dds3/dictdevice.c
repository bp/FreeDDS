static char rcsid[]="$Id: dictdevice.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, open and initialize "device:" dictionary.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

static char dict_buf[] =
#include "device.h"

DDS_DICT * ddsDictDevice(
   const char *mode,
   DDS_DICT_STATE dict_state,
   DDS_MEDIA_STATE media_state)
{
   DDS_DICT *dict;
   size_t n_size = strlen(dict_buf);

   dict_state |= (DDS_DICT_STATE)(DDS_DICT_READ_ONLY | DDS_DICT_STR_BUF);

   dict = ddsDictNew("device:", dict_state, media_state,
      NULL, NULL, dict_buf, n_size + 1);

   /* load string constant into dictionary */
   ddsDictLoad(dict, &dict->load, NULL, dict_buf, n_size);

   return dict;
}
