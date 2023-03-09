static char rcsid[]="$Id: gdopen.c 199 2010-11-04 17:14:10Z user $";
/*
OPEN

*/

#include "gio_sys.h"

GFILE * gdopen(         /* GIO stream (error == NULL) */
   int fd,              /* file descriptor */
   const char *mode)    /* mode string */
{
   int open_flags, open_mode;
   int dflags;
   GIO_STATE state;
   GFILE *fp = NULL;
   int saved_errno;

   gio_init();

   if( *mode == 'c' ) {
      /* just ignore it */
      mode++;
   }
   
   if (gio_mpi_node >= 0 && fd >= 0 && fd <= 1) {
      if (gio_mpi_node > 0) {
         fprintf(gio.std_debug, 
           "GIO_ERROR(NODE%d): Cannot use pipes when using MPI (fd %d)\n",
           gio_mpi_node, fd);
      } else {
         fprintf(gio.std_debug, "%s\n*\n%s\n*\n%s\n",
           "*******************************************************************",
           "*   !!!GIO_ERROR: Cannot use pipes when using MPI!!!",
           "*******************************************************************");
      }
      return NULL;
   }

   state =  GIO_gopen_mode(&open_flags, &open_mode, mode);
   if(! state) return NULL;

#if defined(_WIN32) || defined(MINGW)
   dflags = dup(fd);
#else
   dflags = XFCNTL(fd, F_GETFL, 0);
#endif
   if (dflags == -1) return NULL; /* errno set by fcntl */
#if !defined(_WIN32) && !defined(MINGW)
   switch (dflags & O_ACCMODE) {
   case O_RDONLY:
      if(! (state & GIO_STATE_OPENR)) {errno = EBADF; return NULL;}
      break;
   case O_WRONLY:
      if(! (state & GIO_STATE_OPENW)) {errno = EBADF; return NULL;}
      break;
   }
#endif
   saved_errno=errno; 
   fp = (GFILE*)malloc(sizeof(GFILE));
   if(! fp) {
     errno = ENOMEM; 
     return NULL;
   } else {
     errno = saved_errno;
   } 

   fp->fd = fd;
   fp->state = state;

   GIO_cfd_init(fp);
   fp->cfd.really_open = GIO_TRUE;
   fp->cfd.tmp_close_ok = GIO_FALSE;

   if(state & GIO_STATE_OPENA) {
      fp->offset = GIO_SEEK(fp, 0, SEEK_END);
      if(fp->offset < 0 && errno != ESPIPE) {
         XCLOSE(fp->fd);
         if(gio.trc_file) {
            fprintf(gio.trc_file, "CLOSE: fd=%d\n", fp->fd);
         }
         free(fp);
         return NULL;
      }
   }

   GIO_chain(fp);

   /* finish open processing */
   return GIO_gopen_init(fp);
}
