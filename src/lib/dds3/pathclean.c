static char rcsid[]="$Id: pathclean.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, cleanup pathname

   Examples:
           GIVEN PATH                     RETURN PATH
       1)  bar                            bar
       2)  foo/bar                        foo/bar
       2)  ./bar                          bar
       2)  ./foo/./bar                    foo/bar
       3)  //foo/././bar                  /foo/bar
       4)  /foo///bar                     /foo/bar

*/

/***************************************/
/***   Copyright (C) 2005            ***/
/***   BP America, Inc               ***/
/***   Jerry Ehlers, 16 March 2005   ***/
/***************************************/

#include "dds_sys.h"

const char * ddsPathClean(
   const char *path)
{
   static DDS_STRING new_path = NULL_STRING;
   size_t len;
   const char *path_slash;
   const char *bpath_slash;

   ddsStrCpyMin(&new_path, "", PATHNAME_MAX);

   /* skip leading white space */
   while(isspace(*path)) path++;
#if defined(_WIN32) || defined(MINGW)
   while (*path) {
      if (path[0] == '\\' && path[1] == '\\') {
         /* discard extraneous '/' */
         path++;
      } else if (path[0] == '.' && path[1] == '\\') {
         /* discard extraneous leading './' */
         path += 2;
      } else if (path[0] == '\\' && path[1] == '.'  && path[2] == '\\') {
         /* discard extraneous './' */
         path += 2;
      } else {
         path_slash = strchr(&path[1], '/');
         bpath_slash = strchr(&path[1], '\\');
         if (path_slash) {
            len = path_slash - path;
            ddsStrCatN(&new_path, path, len);
            path = path_slash;
         } else if (bpath_slash) {
            len = bpath_slash - path;
            ddsStrCatN(&new_path, path, len);
            path = bpath_slash;
         } else {
            ddsStrCat(&new_path, path);
            break;
         }
      }
   }
#else
   while(*path) {
      if (path[0] == '/' && path[1] == '/') {
         /* discard extraneous '/' */
         path++;
      } else if (path[0] == '.' && path[1] == '/') {
         /* discard extraneous leading './' */
         path += 2;
      } else if (path[0] == '/' && path[1] == '.'  && path[2] == '/') {
         /* discard extraneous './' */
         path += 2;
      } else {
         path_slash = strchr(&path[1], '/');
         if(path_slash) {
            len = path_slash - path;
            ddsStrCatN(&new_path, path, len);
            path = path_slash;
         } else {
            ddsStrCat(&new_path, path);
            break;
         }
      }
   }
#endif
   return new_path.ptr;
}
