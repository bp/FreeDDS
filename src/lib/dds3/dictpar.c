static char rcsid[]="$Id: dictpar.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, open and initialize "par:" dictionary.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

#define COPY_SIZE (8 * DDS_KB)

DDS_DICT * ddsDictPar(
   const char *mode,
   DDS_DICT_STATE dict_state,
   DDS_MEDIA_STATE media_state)
{
   DDS_DICT *dict, *cmd_dict;
   DDS_TOKEN par_save = {{0, NULL}, NULL};
   const char *par_value, *recover, *par_name;
   int par_size;

   dict = ddsDictNew("par:", dict_state, media_state,
      NULL, NULL, NULL, 0);

   if(! (dict_state & DDS_DICT_LOAD)) return dict;

   cdds_dict("cmd:", "scan");

   if(0 < cdds_scank("par", "")) {
      /* process "par= filenames..." from "cmd:" dictionary */
      cdds_scanv("par", &recover);
      par_value = ddsString(recover);
      par_size = strlen(par_value);
      par_save.next = par_value;
      ddsStrSetMin(&par_save.str, "", par_size);

      par_name = "dds_par{=";
      ddsDictLoad(dict, &dict->load, NULL, par_name, strlen(par_name));
      ddsDictLoad(dict, &dict->load, NULL, par_value, par_size);

      ddsParLoad(&par_save, dict, &dict->load);

      par_name = "dds_par}=";
      ddsDictLoad(dict, &dict->load, NULL, par_name, strlen(par_name));
      ddsDictLoad(dict, &dict->load, NULL, par_value, par_size);

      ddsStrFree(&par_save.str);
      ddsFree((void*)par_value);
   }

   cdds_dict("", "scan");

   /* load command line arguments into dictionary */
   cmd_dict = (DDS_DICT*)ddsTableLookup(&dds_dict, "cmd:");

   ddsHistory(dict, cmd_dict);

   return dict;
}
