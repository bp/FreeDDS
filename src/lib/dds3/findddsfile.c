static char rcsid[]="$Id: findddsfile.c 3 2008-12-08 20:35:59Z user $";
/*
RCS-t=DDS, find "DDS" file among "dds_path" directories

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

const char * ddsFindDDSFile(
   const char *name,
   const char *hist,
   const char *default_name)
{
   static DDS_STRING filename = NULL_STRING;
   static DDS_TOKEN path_save = NULL_TOKEN;
   const char *value, *path;

   if(! filename.max) {
      ddsStrSetMin(&filename, "", PATHNAME_MAX);
      ddsStrSetMin(&path_save.str, "", PATHNAME_MAX);
   }

   cdds_dict(dds_base_dict, "scan");

   if(0 < cdds_scanv("DDS_PATH DDS_PATH3 dds_path dds_path3", &value)) {
      ddsStrCpy(&dds_path, value);

      for(path = TOKEN_INIT(path_save, dds_path.ptr);
         path;
         path = TOKEN_NEXT(path_save)) {
         ddsStrCpy(&filename, path);
#if defined(_WIN32) || defined(MINGW)
         if (path[strlen(path) - 1] != '\\') ddsStrCat(&filename, "\\");
#else
         if(path[strlen(path) - 1] != '/') ddsStrCat(&filename, "/");
#endif
         ddsStrCat(&filename, name);
         if(DDS_MEDIA_REG == ddsStatGenus(filename.ptr)) {
            /* found a regular file */
            return filename.ptr;
         }
      }
   }

   if(default_name) {
      /* return default name */
      ddsStrCpy(&filename, default_name);
      return filename.ptr;
   }

   return NULL;
}
