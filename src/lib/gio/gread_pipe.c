/*
 * WARNING: Do NOT include gio_sys.h - it'll produce a recursive
 * definition of the read call below
 */

#include <sys/types.h>

#include <stdio.h>

#include "gtime.h"
#if defined(_WIN32) || defined(MINGW)
#include <io.h>
#else
#include <unistd.h>
#endif

int gread_pipe(int fd, void *buffer, unsigned int nbyte)
{
   int    bytes_read, total_bytes;
   double time;
   struct tms tbuf;

   total_bytes = 0;
   while ( total_bytes < nbyte )
   {
#if defined(_WIN32) || defined(MINGW)
      bytes_read = XREAD(fd, (char *)buffer + total_bytes, (unsigned)(nbyte - total_bytes));
#else
      if (gio.trc_file) {
         time = times(&tbuf);
         bytes_read = XREAD(fd, (char *)buffer + total_bytes, (unsigned)(nbyte - total_bytes));
         time = (times(&tbuf) - time) / gio.clk_tck;
         fprintf(gio.trc_file, "READ: pipe fd=%d nybtes=%d time(sec)=%g\n",
                 fd, bytes_read, time);
      } else {
         bytes_read = XREAD(fd, (char *)buffer + total_bytes, (unsigned)(nbyte - total_bytes));
      }
#endif
      if ( bytes_read < 0  ) return( bytes_read);
      if ( bytes_read == 0 ) return( total_bytes );
      total_bytes += bytes_read;
   }
   return( total_bytes );
}
