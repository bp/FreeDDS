static char rcsid[]="$Id: revealname.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, return name of file in current directory given a file descriptor.
Shamelessly stolen from SEPlib 11/9/95 by R. Selzler.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

/*
NAME
ddsRevealName \- return name of file in current directory given a file descriptor

SYNOPSIS

int ddsRevealName(fd,string,maxlen)
char string[];
int fd, maxlen;

DESCRIPTION

Findnm is a utility function that looks in the current directory for
the name of a file attached to the file descriptor, fd.  The resulting
filename, if found, is placed in, string, and its length is returned.
Up to maxlen characters will be copied.  A count of zero is returned
if no name could be found.

SEE ALSO
csh(1), pwd, sh(1), find(1), basename(1), cd(1), chdir(2), getwd(3)

DIAGNOSTICS
Findnm returns the value -1 in case of string overflow.

BUGS
If the file has more than one link in the current directory,
only one name will be returned.

KEYWORDS: filename find
*/
/*
 * ddsRevealName(fd,string,maxlen) - generate unqualified name associated with fd
 *			   char string[maxlen]; int fd;
 *			   returns actual number of characters
 *
 * Author: stew     - only looks in current directory
 * Revised: stew  10-18-84  check if "." is in same file system before scanning.
 * Revised: stew   2-7-85   return -1 on string overflow.
 * Revised: Dave Nichols   9/16/90  add posix compatibility
 * Revised: stew  10-14-92 force _SUN for RS6000 for non-posix compatibility
 * Revised: stew  2-25-95  Solaris mods
 * Revised: martin  9-1-95  Linux mods
 */
/*#if defined(RS6000)*/
/*#undef  _SUN*/
/*#define _SUN*/
/*#endif*/
/*#include <sepcube.h>*/
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#if defined _POSIX_SOURCE || defined SOLARIS 
#include <dirent.h>
#define DIRENT 	 struct dirent
#elif defined(_WIN32) || defined(MINGW)
#include <windows.h>
#define DIRENT 	 struct dirent
#else
#include <sys/dir.h>
#define DIRENT 	 struct direct
#endif

#if defined(SGI) || defined(SOLARIS) || defined(GNU)
#define d_fileno d_ino
#endif

#if defined(SOLARIS) ||defined(linux)
#define SIZE(dir) strlen( (dir)->d_name) /* not stored in dirent */
#else
#define SIZE(dir) (dir)->d_namlen
#endif

#ifdef ANSI_DECL
int ddsRevealName(int fd, char* string, int maxlen)
#else
int ddsRevealName(fd,string,maxlen)
int fd; char *string; int maxlen;
#endif
{
#if defined(_WIN32) || defined(MINGW)
   WIN32_FIND_DATA 		*dirp;
   HANDLE hFind = INVALID_HANDLE_VALUE;
#else
   DIR *dirp;
   DIRENT*thisdir;
#endif
   STRUCTSTAT stbuf, ptbuf;
   int nchar;

   string[0] = '\0';

   /* fetch file & current directory status structures */

   if ((ddsFSTAT(fd,&stbuf)==-1) || (ddsSTAT(".",&ptbuf)==-1)) {
      return(-1); 
   }

#if defined(_WIN32) || defined(MINGW)
   /* see if file descriptor refers to same file system */

   if (stbuf.st_dev != ptbuf.st_dev) return(0);

   /* scan files in current directory */

   if ((dirp=opendir(".")) == (WIN32_FIND_DATA*)NULL) return(-1);
   nchar = 0;
   do {
      if (dirp->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
         _tprintf(TEXT("  %s   <DIR>\n"), dirp->cFileName);
      }	else {
         nchar = sizeof(dirp->cFileName);
         strncpy(string, dirp->cFileName, maxlen);
      }
   } while (FindNextFile(hFind, dirp) != 0);
   closedir(dirp);
#else
   /* see if file descriptor refers to same file system */

   if (stbuf.st_dev != ptbuf.st_dev) return(0);

   /* scan files in current directory */

   if ((dirp=opendir(".")) == (DIR*)NULL) return(-1);

   while ((thisdir = readdir(dirp)) != (DIRENT*)NULL) {
      if (((int)(stbuf.st_ino)) == ((int)(thisdir->d_fileno)))
         break;
   }

   if (thisdir != (DIRENT*)NULL) {
      nchar = SIZE(thisdir);
      strncpy(string, thisdir->d_name, maxlen);
   } else
      nchar = 0;

      closedir(dirp);
#endif

   if (nchar > maxlen) return(-1);
   return(nchar);
}
