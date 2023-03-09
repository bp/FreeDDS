static char rcsid[]="$Id: gseek.c 13 2009-01-06 16:56:43Z user $";
/*
long SEEK offset

*/

#include "gio_sys.h"
 
int gseek(              /* seek status (ok == 0, error < 0) */
   GFILE *fp,           /* i/o stream */
   long offset,         /* long seek offset */
   int whence)          /* SEEK_SET, SEEK_CUR, SEEK_END */
{
   gpos_t n_huge;
   struct GIO_STAT stat_buf;

   /* initialize gstdin, gstdout, or gstderr, prior to access */
   if(! (fp->state & GIO_STATE_OPENED) &&
      ! GIO_gopen_init(fp)) return EOF;

   switch(whence) {
   case SEEK_SET:
      n_huge = offset;
      break;
   case SEEK_CUR:
      if(fp->state & GIO_STATE_UNREAD) {
         /*** 8/2/96 rls, bug fix
            "For a binary stream, its file position indicator is
            decremented by each successful call to the ungetc function;"
         ***/
         n_huge = fp->offset + (HOST_INT6_T)(fp->buf_u - fp->start)
            - fp->count_r + offset;
      } else {
         n_huge = fp->offset + (HOST_INT6_T)(fp->buf - fp->start) + offset;
      }
      break;
   case SEEK_END:
      if(fp->state & GIO_STATE_WRITE) {
         /* flush write buffer before seek, to update eof_size */
         if(fp->gflushw(fp)) return EOF;
      }
      n_huge = -1;
#if defined(_WIN32) || defined(MINGW)
      if (!XFSTAT(fp->cfd.name, &stat_buf)) {
#else
      if (!XFSTAT(fp->fd, &stat_buf)) {
#endif
         if(0 <= stat_buf.st_size) {
            n_huge = stat_buf.st_size + offset;
         }
      }
      break;
   default:
      return EOF;
   }

   return gsetpos(fp, &n_huge);
}
