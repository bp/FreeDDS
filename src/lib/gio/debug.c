static char rcsid[]="$Id: debug.c 13 2009-01-06 16:56:43Z user $";
/*
DEBUG stream Buffer pointer

*/

#include "gio_sys.h"

/* DEBUG stream Buffer pointer */
int GIO_debug(        /* status, non-zero = error */
   GFILE *fp,           /* i/o stream */
   const char *msg)	/* debug message */
{
   fprintf(gio.std_debug, "gio:debug:%s\n", msg);

   fprintf(gio.std_debug, "\tfp= %p, magic= 0x%x, fd= %i, state= 0x%x\n",
      (void*)fp, (int)fp->magic, (int)fp->fd, (int)fp->state);

   fprintf(gio.std_debug, "\tchain= %p, error= %i\n",
      (void*)fp->chain, (int)fp->error);

   fprintf(gio.std_debug, "\tcount_r= " HOST_LLONG_QP
                          ", count_w= " HOST_LLONG_QP "\n",
      (HOST_LLONG_T)fp->count_r, (HOST_LLONG_T)fp->count_w);

   fprintf(gio.std_debug, "\tbuf= %p, start= %p\n",
      (void*)fp->buf, (void*)fp->start);

   fprintf(gio.std_debug, "\tbase_p= %p, size_p= " HOST_LLONG_QP
                          ", base_m= %p, size_m= " HOST_LLONG_QP "\n",
      (void*)fp->base_p, (HOST_LLONG_T)fp->size_p,
      (void*)fp->base_m, (HOST_LLONG_T)fp->size_m);

   fprintf(gio.std_debug, "\toffset= " HOST_INT6_QP
                        ", eof_size= " HOST_INT6_QP "\n",
                        fp->offset, fp->eof_size);

   fprintf(gio.std_debug, "\tcount_s= " HOST_LLONG_QP
                          ", buf_s= %p, buf_p= %p\n",
      (HOST_LLONG_T)fp->count_s, (void*)fp->buf_s, (void*)fp->buf_w);

   fprintf(gio.std_debug, "\tbase_s= %p, size_s= " HOST_LLONG_QP "\n",
      (void*)fp->base_s, (HOST_LLONG_T)fp->size_s);

   fprintf(gio.std_debug, "\tcache_r= " HOST_LLONG_QP
                          ", cache_w= " HOST_LLONG_QP "\n",
      (HOST_LLONG_T)fp->cache_r, (HOST_LLONG_T)fp->cache_w);

   fprintf(gio.std_debug, "\tbase_u= %p, size_u= " HOST_LLONG_QP "\n",
      (void*)fp->base_u, (HOST_LLONG_T)fp->size_u);

   fprintf(gio.std_debug, "\tbuf_u= %p, count_u= " HOST_LLONG_QP "\n",
      (void*)fp->buf_u, (HOST_LLONG_T)fp->count_u);

   fflush(gio.std_debug);

   return 0;
}
