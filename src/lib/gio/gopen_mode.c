static char rcsid[]="$Id: gopen_mode.c 3 2008-12-08 20:35:59Z user $";
/*
RCS-t=GIO, Dissect mode flags

*/

#include "gio_sys.h"

GIO_STATE GIO_gopen_mode(
   int *open_flags,
   int *open_mode,
   const char *mode)
{
   GIO_STATE state = (GIO_STATE)0;
   
   *open_mode = *open_flags = 0;

   /* parse mode flags */
#if defined(_WIN32) || defined(MINGW)
   /* Add BINARY flag for Windows */
   char binmode = 0;
   if (*mode == 'b') {
      binmode = 1;
      *open_mode = O_BINARY;
      mode++;
   }
#endif
   switch (*mode++) {
   case 'r':
      *open_mode = O_RDONLY;
      state |= GIO_STATE_OPENR;
      break;
   case 'w':
      *open_mode = O_WRONLY;
      *open_flags = O_CREAT | O_TRUNC;
      state |= GIO_STATE_OPENW;
      break;
   case 'a':
      *open_mode = O_WRONLY;
      *open_flags = O_CREAT | O_APPEND;
      state |= (GIO_STATE)(GIO_STATE_OPENW | GIO_STATE_OPENA);
      break;
   default:
      errno = EINVAL;
      return state;
   }

   if (mode[0] == '+' || (mode[0] == 'b' && mode[1] == '+')) {
      *open_mode = O_RDWR;
      state |= (GIO_STATE)(GIO_STATE_OPENR | GIO_STATE_OPENW);
   }

#if defined(_WIN32) || defined(MINGW)
   if (binmode) {
      *open_mode = *open_mode | O_BINARY;
   }
#endif
   #ifdef HOST_c2mp
      *open_flags |= O_LARGEFILE;
   #endif

   return state;
}
