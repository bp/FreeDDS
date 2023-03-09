static char rcsid[]="$Id: gflush.c 13 2009-01-06 16:56:43Z user $";
/*
RCS-t=GIO, FLUSH gio stream

*/

#include "gio_sys.h"

int gflush(             /* return 0 (error == EOF) */
   GFILE *fp)           /* GIO stream */
{
   int rc = 0;

   if(fp) {
      if(!fp->cfd.really_open)
	 return rc;
      if(fp->state & GIO_STATE_WRITE) {
         if(fp->gflushw(fp)) rc = EOF;
      } else if(fp->state & GIO_STATE_READ) {
         /* non-standard, benevolent extension  */
         if(fp->gflushr(fp)) rc = EOF;
      }
   } else {
      /* flush all *write* streams */
      for(fp = gio.open_chain; fp; fp = fp->chain) {
	 if(fp->cfd.really_open) {
	    if(fp->state & GIO_STATE_WRITE) {
	       if(fp->gflushw(fp)) rc = EOF;
	    } else if(fp->state & GIO_STATE_READ) {
	       /*** standard violation, if activated
		 if(fp->gflushr(fp)) rc = EOF;
		 ***/
	    }
	 }
      }
   }
   return rc;
}
