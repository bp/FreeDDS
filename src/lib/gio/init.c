static char rcsid[]="$Id: init.c 199 2010-11-04 17:14:10Z user $";
/*
RCS-t=GIO, initialization and cleanup

*/

#define GIO_GLOBAL

#include <stdio.h>
#include "gtime.h"
#if defined(_WIN32) || defined(MINGW)
#include <io.h>
#else
#include <unistd.h>
#include <sys/resource.h>
#endif
#include "gio_sys.h"

/* ensure linkage of debug printer */
/* static int (*debugger)(GFILE *fp, const char *msg) = &GIO_debug; */

static void cleanup(void)
{
   /* automatically close all streams, when application exits */
   gclose(NULL);

   return;
}

int (gio_init)(void)
{
   int open_max = 0;
   char *tmp_name, *env;
#if !defined(_WIN32) && !defined(MINGW)
    struct rlimit rlp;
#endif
    
   if(gio.initialized == GIO_MAGIC) return 0;

   #ifdef GIO_MMAP_IS_SUPPORTED
      #undef PAGE_SIZE_SET
      #if (defined(HOST_sun4cg92) | defined(HOST_SGI))
            #define PAGE_SIZE_SET
            gio.page_size = sysconf(_SC_PAGESIZE);
      #endif
      #ifndef PAGE_SIZE_SET
         gio.page_size = getpagesize();
      #endif /* PAGE_SIZE_SET */
      assert(gio.page_size
         && "host page size == 0?");
   #else /* GIO_MMAP_IS_SUPPORTED */
      gio.page_size = GIO_MALLOC_MIN;
   #endif /* GIO_MMAP_IS_SUPPORTED */

   atexit(&cleanup);

   assert((GIO_MASK_STANDARD_MODES &
      (GIO_IO_COPY_READ | GIO_IO_COPY_WRITE |
       GIO_IO_MMAP_READ | GIO_IO_MMAP_WRITE |
       GIO_IO_RAW_READ | GIO_IO_RAW_WRITE |
       GIO_NOMINAL_BUFFERING |
       GIO_IO_NON_TUNED | GIO_IO_NON_CACHE)) == 0
         && "gsetvbuf mode flag conflict?");

   gio.std_debug = stderr;

   if ((env=getenv("GIO_TRACE")) != NULL) {
      tmp_name = (char*)malloc(32+strlen(env));
      sprintf(tmp_name, "%sgio_trace.%d", env, getpid());
      gio.trc_file = fopen(tmp_name, "w+");
#if defined(_WIN32) || defined(MINGW)
      gio.clk_tck = CLOCKS_PER_SEC;
#else
      gio.clk_tck = sysconf(_SC_CLK_TCK);
#endif
   }

   if ((env=getenv("GIO_BLKSIZE")) != NULL) {
      gio.blk_size = atoi(env);
   }

   gio.verbose = (getenv("GIO_VERBOSE") != NULL);


#ifdef GIO_SYSCONF_AVAILABLE
   open_max = sysconf(_SC_OPEN_MAX) - 4;
   if (gio.verbose) {
      fprintf(gio.std_debug, "GIO_init: sysconf(_SC_OPEN_MAX)=%d\n",
         open_max);
   }
#endif
#if defined(_WIN32) || defined(MINGW)
   open_max = _getmaxstdio();
#else
   if (!getrlimit(RLIMIT_NOFILE, &rlp)) {
      open_max = rlp.rlim_cur - 4;
      if (gio.verbose) {
         fprintf(gio.std_debug, "GIO_init: rlimit_nofile=%d\n",
            open_max);
      }
   } 
#endif
#ifdef _POSIX_OPEN_MAX
   if(_POSIX_OPEN_MAX > open_max) {
      open_max = _POSIX_OPEN_MAX;
   }
   if (gio.verbose) {
      fprintf(gio.std_debug, "GIO_init: _POSIX_OPEN_MAX=%d\n",
         _POSIX_OPEN_MAX);
   }
#endif

#ifdef OPEN_MAX
   if(OPEN_MAX > open_max) {
      open_max = OPEN_MAX;
   }
   if (gio.verbose) {
      fprintf(gio.std_debug, "GIO_init: OPEN_MAX=%d\n",
         OPEN_MAX);
   }
#endif

   if(open_max > GIO_OPEN_MAX_BASE) {
      gio.open_max = open_max - GIO_OPEN_MAX_PAD;
      if (gio.verbose) {
         fprintf(gio.std_debug, "GIO_init: GIO_OPEN_MAX_BASE=%d GIO_OPEN_MAX_PAD=%d\n",
            GIO_OPEN_MAX_BASE, GIO_OPEN_MAX_PAD);
      }
   }

   if ((env=getenv("GIO_OPEN_MAX")) != NULL) {
      sscanf(env, "%d", &open_max);
      gio.open_max = open_max;
   }

   if (gio.verbose) {
      fprintf(gio.std_debug, "GIO_init: gio.open_max=%d\n",
         gio.open_max);
   }
   gio.init_open_max = gio.open_max;

   /* initialization completed */
   gio.initialized = GIO_MAGIC;

   return 0;
}
