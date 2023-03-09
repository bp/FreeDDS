static char rcsid[]="$Id: pathfast.c 16 2009-01-06 21:52:47Z user $";
/*
  RCS-t=DDS, morph to the fastest (clean) path name

  Examples:
  GIVEN PATH                   HOST    RETURN PATH
  1)  bar                        *       bar
  2)  foo/bar                    *       foo/bar
  3)  ./bar                      *       bar
  4)  ./foo/bar                  *       foo/bar
  5)  /foo/bar                   *       /foo/bar
  6)  /data/host/data1/foo/bar  host     /data1/foo/bar
                                host     /export/data1/foo/bar
  7)  /hpc/foo/bar             exists    /foo/bar
  8)  /foo/bar                 exists    /hpc/foo/bar
 
  - Assume scan dictionary set
  - Try opening file as is (/data1/foo/var)
  - Try /data/host/data1/foo/bar if /data...

*/

/***************************************/
/***   Copyright (C) 2005            ***/
/***   BP America, Houston, TX       ***/
/***   Jerry Ehlers, 16 March 2005   ***/
/***************************************/

#include "dds_sys.h"

const char * ddsPathFast(
   const char *path)
{
   static DDS_STRING new_path = NULL_STRING;
   static DDS_STRING hold_path = NULL_STRING;
   static int warning = 0;
   DDS_MEDIA_GENUS path_genus;
   size_t cmd_host_size;
   char *path_slash, *env;
   const char *defn, *new_cwd;
   static char *mountpath=(char*)0;

   if (!mountpath) {
      env = getenv("DDS_MNT");
      if (env) {
         mountpath = (char*)malloc(2+strlen(env));
         strcpy(mountpath, env);
         if (mountpath[strlen(mountpath)-1] != '/') {
            strcat(mountpath, "/");
         }
      } else {
         mountpath = (char*)malloc(6);
         strcpy(mountpath, "/hpc/");
      }
   }

   ddsStrCpyMin(&new_path, "", PATHNAME_MAX+1);

   /* cleanup the given path name */
   path = ddsPathClean(path);

   cmd_host_size = strlen(dds_cmd_host);

#if defined(_WIN32) || defined(MINGW)
   if (!strncmp(path, "\\data\\", 6)) {
      if (!strncmp(path + 6, dds_cmd_host, cmd_host_size) &&
         path[6 + cmd_host_size] == '\\') {
         /* GIVEN "/data/host/path" ON "host" */
         ddsStrCpy(&new_path, path + 6 + cmd_host_size);
         path_slash = strrchr(new_path.ptr, '\\');
         if (path_slash) {
            *path_slash = '\0';
            path_genus = ddsStatGenus(new_path.ptr);
            if (path_genus == DDS_MEDIA_DIR) {
               /* RETURN "/path" */
               *path_slash = '\\';
               return new_path.ptr;
            }
         }

         ddsStrCpyCat2(&new_path, "\\export", path + 6 + cmd_host_size);
         path_slash = strrchr(new_path.ptr, '\\');
         if (path_slash) {
            *path_slash = '\0';
            path_genus = ddsStatGenus(new_path.ptr);
            if (path_genus == DDS_MEDIA_DIR) {
               /* RETURN "/export/path", */
	       *path_slash = '\\';
               return new_path.ptr;
            }
         }
      }
   } else if (!strncmp(path, mountpath, strlen(mountpath))) {
      /* GIVEN "/hpc/path" try "/path" */
      ddsStrCpy(&new_path, path + strlen(mountpath) - 1);
      path_slash = strrchr(new_path.ptr, '\\');
      if (path_slash) {
         *path_slash = '\0';
         path_genus = ddsStatGenus(new_path.ptr);
         if (path_genus == DDS_MEDIA_DIR) {
            /* RETURN "/path" */
            *path_slash = '\\';
            return new_path.ptr;
         }
      }
   } else if (path[0] == '\\') {
      ddsStrCpy(&new_path, path);
      path_slash = strrchr(new_path.ptr, '\\');
      if (path_slash == path) {
         ddsStrCpy(&new_path, path);
         return new_path.ptr;
      } else if (path_slash) {
         *path_slash = '\0';
         path_genus = ddsStatGenus(new_path.ptr);
         if (path_genus != DDS_MEDIA_DIR) {
            /* GIVEN "/path" try "/hpc/path" */
            ddsStrCpyN(&new_path, mountpath, strlen(mountpath) - 1);
            ddsStrCat(&new_path, path);
            path_slash = strrchr(new_path.ptr, '/');
            *path_slash = '\0';
            path_genus = ddsStatGenus(new_path.ptr);
            if (path_genus == DDS_MEDIA_DIR) {
               /* RETURN "/hpc/path" */
               *path_slash = '\\';
               if (!getenv("DDS_MNT") && !warning) {
                  warning = 1;
                  ddsWarn("Needing to add '%s' onto data paths\n",
                          mountpath);
               }
               return new_path.ptr;
            }
         }
      }
#else
   if(! strncmp(path, "/data/", 6)) {
      if(! strncmp(path + 6, dds_cmd_host, cmd_host_size) &&
         path[6 + cmd_host_size] == '/') {
         /* GIVEN "/data/host/path" ON "host" */
         ddsStrCpy(&new_path, path + 6 + cmd_host_size);
         path_slash = strrchr(new_path.ptr, '/');
         *path_slash = '\0';
         path_genus = ddsStatGenus(new_path.ptr);
         if(path_genus == DDS_MEDIA_DIR) {
            /* RETURN "/path" */
            *path_slash = '/';
            return new_path.ptr;
         }

         ddsStrCpyCat2(&new_path, "/export", path + 6 + cmd_host_size);
         path_slash = strrchr(new_path.ptr, '/');
         *path_slash = '\0';
         path_genus = ddsStatGenus(new_path.ptr);
         if(path_genus == DDS_MEDIA_DIR) {
            /* RETURN "/export/path", */
            *path_slash = '/';
            return new_path.ptr;
         }
      }
   } else if(! strncmp(path, mountpath, strlen(mountpath))) {
      /* GIVEN "/hpc/path" try "/path" */
      ddsStrCpy(&new_path, path + strlen(mountpath)-1);
      path_slash = strrchr(new_path.ptr, '/');
      *path_slash = '\0';
      path_genus = ddsStatGenus(new_path.ptr);
      if(path_genus == DDS_MEDIA_DIR) {
         /* RETURN "/path" */
         *path_slash = '/';
         return new_path.ptr;
      }
   } else if (path[0]=='/') {
      ddsStrCpy(&new_path, path);
      path_slash=strrchr(new_path.ptr, '/');
      if (path_slash == path) {
         ddsStrCpy(&new_path, path);
         return new_path.ptr;
      }
      *path_slash = '\0';
      path_genus = ddsStatGenus(new_path.ptr);
      if (path_genus != DDS_MEDIA_DIR) {
         /* GIVEN "/path" try "/hpc/path" */
         ddsStrCpyN(&new_path, mountpath, strlen(mountpath)-1);
         ddsStrCat(&new_path, path);
         path_slash = strrchr(new_path.ptr, '/');
         *path_slash = '\0';
         path_genus = ddsStatGenus(new_path.ptr);
         if(path_genus == DDS_MEDIA_DIR) {
            /* RETURN "/hpc/path" */
            *path_slash = '/';
            if (!getenv("DDS_MNT") && !warning) {
               warning = 1;
               ddsWarn("Needing to add '%s' onto data paths\n",
                       mountpath);
            }
            return new_path.ptr;
         }
      }
#endif
   } else if (path[strlen(path)-1] != ':') {
      /* IFF the cwd has changed, put the original cwd
         on the beginning of files with relative pathname
         & are not special files (eg. Landmark changes cwd) */
      /* decouple path so we can recall PathClean a 2nd time */
      ddsStrCpy(&hold_path, path);
      path = hold_path.ptr;
      ddsStrCpy(&new_path, "<cwd>");
      getcwd(new_path.ptr, (size_t)PATHNAME_MAX);
      new_cwd = ddsPathClean(new_path.ptr);
      if (strcmp(new_cwd, dds_cmd_cwd)) {
         ddsStrCpyCat3(&new_path, dds_cmd_cwd, "/", path);
         return new_path.ptr;
      }
   }

   /* Just return the path */
   ddsStrCpy(&new_path, path);
   return new_path.ptr;
}
