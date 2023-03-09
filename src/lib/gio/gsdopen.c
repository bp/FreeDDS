static char rcsid[]="$Id: gsdopen.c 13 2009-01-06 16:56:43Z user $";
/*
Swap file Descriptor OPEN

*/

#include "gio_sys.h"

GFILE * gsdopen(        /* GIO stream (error == NULL) */
   const char *name,    /* path and file name */
   const char *mode,    /* mode string */
   GFILE *fp)           /* GIO stream */
{
   int fd;
   GIO_BOOLEAN cached_fd = GIO_FALSE;
   GIO_STATE state;
   struct GIO_STAT filestat;

   gio_init();

   if( *mode == 'c' ) {
      cached_fd = GIO_TRUE;
      mode++;
   }
   
   if(strlen(name) > GIO_FILENAME_MAX) {
       errno = ENAMETOOLONG;
       return NULL;
   }

   fp->state =  GIO_gopen_mode(&fp->cfd.open_flags, &fp->cfd.open_mode, mode);
   if(! fp->state) return NULL;

   GIO_cfd_init(fp);
   strcpy( fp->cfd.name, name );
   fp->cfd.tmp_close_ok = cached_fd;

   if(GIO_open(fp) < 0) {
      return NULL;
   }

   if(fp->cfd.tmp_close_ok) {
#if defined(_WIN32) || defined(MINGW)
      XFSTAT(fp->cfd.name, &filestat);
#else
      XFSTAT(fp->fd, &filestat);
#endif
      if (!S_ISREG(filestat.st_mode)) {
         fp->cfd.tmp_close_ok = GIO_FALSE;
      }
   }

   GIO_chain(fp);

   return fp;
}
