static char rcsid[]="$Id: dictsegy1fmt.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, open and initialize "segy1.fmt:" dictionary.

*/

/***********************************/
/***   Copyright (C) 2004        ***/
/***   BP America, Houston, TX   ***/
/***   Jerry Ehlers, Nov. 2004   ***/
/***********************************/

#include "dds_sys.h"

static char dict_buf[] =
#include "segy1.fmt.h"

DDS_DICT * ddsDictSegy1Fmt(
   const char *mode,
   DDS_DICT_STATE dict_state,
   DDS_MEDIA_STATE media_state)
{
   DDS_DICT *dict;
   size_t n_size = strlen(dict_buf);

   dict_state |= (DDS_DICT_STATE)(DDS_DICT_READ_ONLY | DDS_DICT_STR_BUF);

   dict = ddsDictNew("segy1.fmt:", dict_state, media_state,
      NULL, NULL, dict_buf, n_size + 1);

   /* load string constant into dictionary */
   ddsDictLoad(dict, &dict->load, NULL, dict_buf, n_size);

   return dict;
}
