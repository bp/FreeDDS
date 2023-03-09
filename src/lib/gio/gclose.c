static char rcsid[]="$Id: gclose.c 13 2009-01-06 16:56:43Z user $";
/*
RCS-t=GIO, CLOSE

*/

#include "gio_sys.h"

/* close file descriptor, but fp remains logically open */
int GIO_close(          /* status (error == EOF) */
    GFILE *fp)          /* GIO stream */
{
   int rc = 0;

   if(fp->state & GIO_STATE_WRITE) {
      if(fp->gflushw(fp)) rc = EOF;
   } else if(fp->state & GIO_STATE_READ) {
      if(fp->gflushr(fp)) rc = EOF;
   }

   XCLOSE(fp->fd);
   if(gio.trc_file) {
      fprintf(gio.trc_file, "CLOSE: fd=%d filename=%s access_count=%lu\n", 
         fp->fd, fp->cfd.name, fp->cfd.last_access_count);
   }
   gio.num_fd_open--;
   fp->cfd.really_open = GIO_FALSE;

   return rc;
}

/* CLOSE */
static int gclose2(     /* status (error == EOF) */
   GFILE *fp)           /* GIO stream */
{
   int rc = 0;
   GFILE *prev_fp;

   if(fp->state & GIO_STATE_CLOSED) return EOF;

   if(fp == gio.open_chain) {
      /* GFILE structure is at the head of the chain */
      prev_fp = NULL;
   } else {
      /* find specified GFILE structure on open chain */
      for(prev_fp = gio.open_chain; prev_fp; prev_fp = prev_fp->chain) {
         if(prev_fp->chain == fp) break;
      }
      if(! prev_fp) return EOF; /* not found, bogus GFILE */
   }

   fp->state |= GIO_STATE_CLOSED;

   if(!fp->cfd.really_open) {
      return fp->cfd.last_close_status;
   }

   rc = GIO_close(fp);

   if(fp->base_p) free(fp->base_p);
   if(fp->base_s) free(fp->base_s);
   if(fp->base_u) free(fp->base_u);
   #ifdef GIO_MMAP_IS_SUPPORTED
      if(fp->base_m && munmap(fp->base_m, fp->size_m)) {
         fp->error = errno ? errno : EIO;
         rc = EOF;
      }
   #endif /* GIO_MMAP_IS_SUPPORTED */

   if(prev_fp) {
      prev_fp->chain = fp->chain;
   } else {
      gio.open_chain = fp->chain;
   }

   #ifdef GIO_DEBUG
      fp->chain = gio.closed_chain;
      gio.closed_chain = fp;
   #endif /* GIO_DEBUG */

   return rc;
}

/* CLOSE */
int gclose(             /* status (error == EOF) */
   GFILE *fp)           /* GIO stream */
{
   int rc = 0;
 
   if(fp) {
      if(! (fp->state & GIO_STATE_OPENED)) {
         /* pathology: pre-connected file (gstdin, out, err) */
         /* was never accessed, except for *this* close. */
         if(! GIO_gopen_init(fp)) return EOF;
      }
      if(gclose2(fp)) rc = EOF;
   } else {
      rc = EOF;
   }
   return rc;
}
