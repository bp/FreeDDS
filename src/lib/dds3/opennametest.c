static char rcsid[]="$Id: opennametest.c 186 2010-09-27 19:29:14Z user $";
/*
RCS-t=DDS, open binary data, test the genus of data filenames

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int ddsOpenNameTest(
   DDS_BIN *bin,
   DDS_STRING *data_name,
   int *slice_max)
{
   static DDS_STRING base_name = NULL_STRING;
   static DDS_STRING path_name = NULL_STRING;
   static DDS_STRING full_name = NULL_STRING;
   static DDS_STRING alt_name  = NULL_STRING;
   static DDS_TOKEN token_save = NULL_TOKEN;
   DDS_MEDIA_GENUS genus, alt_genus;
   DDS_STAT_ID fast_id, alt_id;
   int fd, create, exist, bogus, attach;
   char colon, ch;
   const char *value, *slash, *name, *fast_name;
   const char *dict_path = NULL;
   const char *from_name = NULL;

   /********************************************************************/
   /* If mode is read or append, "data=..." defines the filenames.     */
   /*    Note: if mode is append and data is sliced (multiple files),  */
   /*    then all files must either exist or all must be created;      */
   /*    mixing exist and create in append mode is an error.           */
   /*    If mode is read, all files must already exist.                */
   /* If bin->name is not NULL, the filenames were overridden          */
   /* and must be validated (read, write or append mode).              */
   /*    Note: the override is aliased to the nominal "data" name.     */
   /*                                                                  */
   /* Process all names defined by data.                               */
   /* Retrieve the media genus to determine if the filename is         */
   /* bogus, already exists, or must be created.                       */
   /*    Note: this does not open any files or allocate any DDS_MEDIA. */
   /*    This checks to see if the slices exist or must be created,    */
   /*    and allows read/write/append modes to behave rationally.      */
   /* Initialize data_name based upon the values defined by data.      */
   /*    Note: the path used by data_name is different, iff tranformed */
   /*    by ddsPathFast or because a relative path was replaced by     */
   /*    the dict_path (path used to access the control dictionary).   */
   /*    Mneumonic names are preserved in the data_name version.       */
   /********************************************************************/

   cdds_dict(bin->dict_open, "scan");

   /* Create dict_path (path to dictionary describing binary). */
   /* It may provide an alternate path to find input binary data. */
   fast_name = ddsPathFast(dds_scan_last->name);
#if defined(_WIN32) || defined(MINGW)
   /*
      It is trick for Windows.
      Directory may be forward slash (produced by DDS)
      and backward slash (by DDS app).
    */
   char fslash = 1;
   slash = strrchr(fast_name, '/');
   if (slash == NULL) {
      slash = strrchr(fast_name, '\\');
      if (slash) {
         fslash = 0;
      }
   }
#else
   slash = strrchr(fast_name, '/');
#endif
   if(slash) dict_path = ddsStringN(fast_name, (slash - fast_name));

   value = "";
   cdds_scanv("data", &value);
   from_name = ddsString(value);

   *slice_max = 0;
   bogus = exist = create = attach = 0;

   for(name = TOKEN_INIT(token_save, from_name);
      name;
      name = TOKEN_NEXT(token_save)) {

      /* Change network path to fastest path. */
      fast_name = ddsPathFast(name);

#if defined(_WIN32) || defined(MINGW)
     if (fslash) {
        slash = strrchr(fast_name, '/');
      }
      if (slash == NULL) {
         slash = strrchr(fast_name, '\\');
      }
#else
      slash = strrchr(fast_name, '/');
#endif

      if(slash) {
         /* Found '/'; name contains a path and base name. */
         ddsStrCpy(&base_name, slash + 1);
         ddsStrCpyN(&path_name, fast_name, slash - fast_name);
      } else {
         /* Base name only. */
         ddsStrCpy(&base_name, fast_name);
         ddsStrCpy(&path_name, "");
      }

      if(! strcmp(fast_name, "stdin:")) {
         genus = ddsFstatGenus(fileno(stdin));
      } else if(! strcmp(fast_name, "stdout:")) {
         genus = ddsFstatGenus(fileno(stdout));
      } else if(2 == sscanf(fast_name, " fd%d%c%c", &fd, &colon, &ch) &&
         colon == ':') {
         genus = ddsFstatGenus(fd);
      } else if (!strncmp(base_name.ptr, "tmp", 3) 
         && base_name.ptr[strlen(base_name.ptr)-1] == ':') {
         genus = DDS_MEDIA_ERR;
         bin->state |= DDS_BIN_TMP;
      } else if(! strcmp(fast_name, "dict:")) {
         attach++;
         genus = DDS_MEDIA_REG;  /* this is a kludge */
      } else if(! strcmp(fast_name, "cmd:") ||
         ! strcmp(fast_name, "par:") ||
         ! strcmp(fast_name, "env:")) {
         ddsError("open, invalid data name, '%s'\n\tdict= %s\n",
            fast_name, bin->dict_open);
         genus = DDS_MEDIA_NOT;
      } else {
         /* process a real filename, not a mneumonic. */
         genus = ddsStatGenus(fast_name);
#if defined(_WIN32) || defined(MINGW)
         char schar[1];
         if (fslash) {
            schar[0] = '/';
         } else {
            schar[0] = '\\';
         }
         if(dict_path
            && (*bin->mode == 'r' || *bin->mode == 'a')
            && *name != schar[0]) {
            /* fast_name might be relative to the dictionary path. */
            /* Create alternate name and determine its genus too. */
            ddsStrCpy(&full_name, fast_name);
                      fast_name = full_name.ptr;
                      ddsStrCpyCat3(&alt_name, dict_path, "\\", name); 
#else
         if(dict_path
            && (*bin->mode == 'r' || *bin->mode == 'a')
            && *name != '/') {
            /* fast_name might be relative to the dictionary path. */
            /* Create alternate name and determine its genus too. */
            ddsStrCpy(&full_name, fast_name);
            fast_name = full_name.ptr;
            ddsStrCpyCat3(&alt_name, dict_path, "/", name);
#endif
            ddsStrCpy(&alt_name, ddsPathFast(alt_name.ptr));
            alt_genus = ddsStatGenus(alt_name.ptr);
            if(alt_genus != DDS_MEDIA_ERR
               && ! ddsStatID(&alt_id, alt_name.ptr)) {
               if(genus != DDS_MEDIA_ERR
                  && ! ddsStatID(&fast_id, full_name.ptr)
                  && memcmp((void*)&fast_id, (void*)&alt_id, sizeof(DDS_STAT_ID))) {
                  ddsWarn("open, ambiguous data filename\n\tname= %s\n\tdict path name= %s\n\tdict= %s\n\tto resolve ambiguity, specify an absolute data path\n",
                     full_name.ptr, alt_name.ptr, bin->dict_open);
                  if (! *path_name.ptr) {
                     /* if both are valid, use secondary filename (relative to dictionary path) */
                     fast_name = alt_name.ptr;
                     genus = alt_genus;
                  }
               } else {
                  /* Use secondary filename (it already exists) */
                  fast_name = alt_name.ptr;
                  genus = alt_genus;
               }
            }
         } else if(genus == DDS_MEDIA_ERR) {
            if(*path_name.ptr) {
               if(DDS_MEDIA_DIR != ddsStatGenus(path_name.ptr)) {
                  ddsWarn("open, data path not accessible, '%s'\n\tdict= %s\n",
                     path_name.ptr, bin->dict_open);
               }
            }
         }
      }

      switch(genus) {
      case DDS_MEDIA_CHR:
         if(strcmp(fast_name, "/dev/null")) {
            /* Tape devices require special consideration. */
            bin->state |= DDS_BIN_TAPE;
         }
         exist++;
         break;
      case DDS_MEDIA_BLK:
      case DDS_MEDIA_REG:
      case DDS_MEDIA_FIFO:
      case DDS_MEDIA_NULL:
         /* This genus indicates the file already exists. */
         exist++;
         break;
      case DDS_MEDIA_ERR:
         /* This genus indicates the file must be created. */
         create++;
         break;
      case DDS_MEDIA_NOT:
      case DDS_MEDIA_DIR:
      case DDS_MEDIA_TTY:
      default:
         /* This genus indicates the file is bogus. */
         bogus++;
         break;
      }

      if((*slice_max)++) {
         ddsStrCat2(data_name, " ", fast_name);
      } else {
         ddsStrCpy(data_name, fast_name);
      }
   }

   /****************************************************************/
   /* Given the open mode and genus summary, catch blatant errors. */
   /****************************************************************/

   if(attach && *slice_max > 1) {
      ddsError("open, sliced data attached to dictionary\n\tdict= %s\n",
         bin->dict_open);
   } else if(bogus && *bin->mode != 'm') {
      ddsError("open, bogus data filename(s) %s\n\tdict= %s\n",
         from_name, bin->dict_open);
   } else if(*bin->mode == 'r') {
      /* open existing data */
      if(create) return 1;
   } else if(*bin->mode == 'w') {
      /* write mode, create the files as needed. */
      bin->state |= DDS_BIN_OPEN_CREATE;
   } else if(*bin->mode == 'a') {
      /* Append to data, either existing or created files. */
      if(create && exist) {
         /* error, some files exist, but not all of them */
         ddsError("open, some data files do not exist (append mode)\n\tdict= %s\n",
            bin->dict_open);
      } else if(! exist) {
         /* create new files and append data to them */
         bin->state |= DDS_BIN_OPEN_CREATE;
      }
   }

   if(dict_path) ddsFree((void*)dict_path);
   if(from_name) ddsFree((void*)from_name);

   return DDS_OK;
}
