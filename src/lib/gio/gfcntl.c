static char rcsid[]="$Id: gfcntl.c 38 2009-01-12 18:39:42Z user $";
/*
FILE CONTROL

*/

#include <stdarg.h>
#include "gio_sys.h"

#ifdef __linux__
#define	numArg_t  long
#else
#define	numArg_t  int
#endif

typedef enum
{
   NO_ARG,
   NUM_ARG,
   PTR_ARG
}
argType;

int gfcntl(	        /* maniplate file descriptor */
   GFILE *fp,           /* file pointer */
   int cmd,		/* command */
   /* int arg */ ... )	/* command argument */
{
   va_list ap;
   numArg_t numArg = 0;
   void *ptrArg = NULL;
   argType aType = NO_ARG;
   int result = -1;
#if defined(_WIN32) || defined(MINGW)
   result = dup(fp->fd);
#else
   if(GIO_cfd_check(fp) < 0) return result;

   switch( cmd )
   {

   case F_DUPFD:
   case F_SETFD:
   case F_SETFL:
   case F_SETOWN:
      va_start( ap, cmd );
      numArg = va_arg( ap, numArg_t );
      va_end( ap );
      aType = NUM_ARG;
      break;
   case F_GETLK:
   case F_SETLK:
   case F_SETLKW:
      va_start( ap, cmd );
      ptrArg = va_arg( ap, void* );
      va_end( ap );
      aType = PTR_ARG;
      break;
   case F_GETFD:
   case F_GETFL:
   case F_GETOWN:
      aType = NO_ARG;
      break;
   default:
      return result;
   }

   switch( aType )
   {
   case NO_ARG:
      result = XFCNTL( fp->fd, cmd, 0 );
      break;
   case NUM_ARG:
      result = XFCNTL( fp->fd, cmd, numArg );
      break;
   case PTR_ARG:
      result = XFCNTL( fp->fd, cmd, ptrArg );
      break;
   }
#endif
   return result;
}
