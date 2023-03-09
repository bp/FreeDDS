static char rcsid[]="$Id: tstsystem.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=GIO, tstSystem: test system initialization.

*/

#if defined(_WIN32) || defined(MINGW)
#include "utsname.h"
#else
#include <sys/utsname.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include "tstsys.h"

/***************************/
/* define static variables */
/***************************/

static char hostname[64] = "<host>";
static char filename[GIO_FILENAME_MAX] = "/export/data2/user/";
static char logfile[GIO_FILENAME_MAX+1];

const char *form_name[] = {"verify", "speed"};
const char *pack_name[] =
   {"stdio", "usp", "vds", "buf_ptr", "buf_cpy",
      "map_ptr", "map_cpy", "raw_ptr", "raw_cpy"};
const char *mode_name[] = {"nop",
   "read_forward", "read_backward", "read_random",
   "write_forward", "write_backward", "write_random", "create_forward"
   };
const char *size_name[] = {"size_fixed", "size_random"};
const char *align_name[] = {"align_fixed", "align_random"};
const char *bias_name[] = {"bias_fixed", "bias_random"};
 
void tstSystem(
   TST_SETUP *setup,
   int argc,
   const char** argv)
{
   FILE *stdlog;
   TST_MASK TM_form, TM_pack, TM_mode, TM_size, TM_align, TM_bias;
   struct utsname name;
   struct stat stat_buf;
   const char *base_name;
   int i;

   setargcv(argc, argv);

   gio_init();

   /**********************************/
   /* default basic setup parameters */
   /**********************************/

   memset((void*)setup, '\0', sizeof(TST_SETUP));

   setup->board = (TST_BOARD*)safemalloc(sizeof(TST_BOARD));
   memset((void*)setup->board, '\0', sizeof(TST_BOARD));

   setup->hostname = hostname;
   if(0 <= uname(&name)) strcpy(hostname, name.nodename);
   setup->filename = filename;

   setup->read_mbs = 4.;
   setup->write_mbs = 3.;

   setup->title = "<untitled>";
   setup->smart = 0;
   setup->watch = 0;
   setup->verbose = 0;
   setup->seed = 0x29e3fb6d;
   setup->time_limit = 90.;
   setup->file_limit = 50 * GIO_MB;
   setup->cache_limit = 50;
   setup->cache_flags = 0;

   setup->pass_max = 1;

   TM_form = (TST_MASK)(TM_verify | TM_speed);

   TM_pack = (TST_MASK)(TM_stdio | TM_usp | TM_vds |
         TM_buf_ptr | TM_buf_cpy |
         TM_map_ptr | TM_map_cpy |
         TM_raw_ptr | TM_raw_cpy);

   TM_mode = (TST_MASK)(TM_nop |
         TM_read_forward | TM_read_backward | TM_read_random |
         TM_write_forward | TM_write_backward | TM_write_random |
         TM_create_forward);

   TM_size = (TST_MASK)(TM_size_fixed | TM_size_random);

   TM_align = (TST_MASK)(TM_align_fixed | TM_align_random);

   TM_bias = (TST_MASK)(TM_bias_fixed | TM_bias_random);

   setup->size_max = 3;
   setup->size_list[0] = 48;       /* bytes, nominal ascii text */
   setup->size_list[1] = 4800;     /* bytes, nominal seismic trace */
   setup->size_list[2] = 480000;   /* bytes, nominal seismic record */

   setup->align_max = 1;
   setup->align_list[0] = 8;       /* cray frverify fails unless % 8 */
   setup->align_list[1] = 16;
   setup->align_list[2] = 256;
   setup->align_list[3] = 4096;
   setup->align_list[4] = 65536;

   setup->bias_max = 1;
   setup->bias_list[0] = 0;

   setup->block_max = 2;
   setup->block_list[0] = -1;            /* non-buffered */
   setup->block_list[1] = 0;             /* default buffer size */
   setup->block_list[2] = 8   * GIO_KB;  /* small buffer size */
   setup->block_list[3] = 64  * GIO_KB;  /* large buffer size */
   setup->block_list[4] = 512 * GIO_KB;  /* huge buffer size */
   setup->block_list[5] = 0;

   setup->loop_max = 1000000;

   /***********************************************/
   /* tweek setup parameters based upon host name */
   /***********************************************/

   /**********************************************************/
   /* "filename" is the absolute path to the test directory. */
   /* It should be on the local file system.                 */
   /*                                                        */
   /* "cache_limit" is the size of the test and flush file.  */
   /* It should be approximately the file system cache size. */
   /* If unknown, 95% of the real memory size is reasonable. */
   /* On Sun, "top" reports the real memory size.            */
   /* On SGI, "osview" reports memory usage in real time.    */
   /*                                                        */
   /* "read_mbs" and "write_mbs" are nominal stdio rates.    */
   /* These values are used to predict total test duration.  */
   /* Pre and post processing of test and flush files use    */
   /* stdio, unless the usp i/o package is being tested.     */
   /* Processing time for the main test loop has a fixed     */
   /* upper bound, because its speed is unpredictable.       */
   /**********************************************************/

   if(! strcmp("hosjwe06", setup->hostname)) {
      strcpy(filename, "/data2/");
      setup->cache_limit = 128;
      setup->read_mbs = 32.;
      setup->write_mbs = 32.;
   } else if(! strcmp("gssgi11", setup->hostname)) {
      strcpy(filename, "/data17/user/");
      setup->cache_limit = 375 / 4;
      setup->read_mbs = 32.;
      setup->write_mbs = 32.;
   } else if(! strcmp("stimpy", setup->hostname)) {
      strcpy(filename, "/dat01/user/");
      setup->cache_limit = 7.7 * 1024;
      setup->cache_limit /= 16;
      setup->read_mbs = 32.;
      setup->write_mbs = 32.;
   } else if(! strcmp("obuort", setup->hostname)) {
      strcpy(filename, "/dat02/user/");
      setup->cache_limit = 3.9 * 1024;
      setup->read_mbs = 20.;
      setup->write_mbs = 10.;
   } else if(! strcmp("gssgi5", setup->hostname)) {
      strcpy(filename, "/dat27/user/");
      setup->cache_limit = 8 * 1024;
      setup->cache_limit /= 16;
      setup->read_mbs = 20.;
      setup->write_mbs = 10.;
   } else if(! strcmp("harpo", setup->hostname)) {
      strcpy(filename, "/tmp/user/");
      setup->cache_limit = 128;
      setup->read_mbs = 5.0;
      setup->write_mbs = 3.;
   } else if(! strcmp("lurch", setup->hostname)) {
      strcpy(filename, "/data1/user/");
      setup->cache_limit = 128;
      setup->read_mbs = 32.;
      setup->write_mbs = 32.;
   } else if(! strcmp("arco000", setup->hostname)) {
      strcpy(filename, "/tmp/user/");
      setup->cache_limit = 128;
      setup->read_mbs = 2.6;
      setup->write_mbs = 2.;
   } else if(! strcmp("calw0229", setup->hostname)) {
      strcpy(filename, "/temp/user/");
      setup->cache_limit = 208;
      setup->read_mbs = 21.;
      setup->write_mbs = 10.;
   } else if(! strcmp("sc", setup->hostname)) {
      strcpy(filename, "/wrk/user/");
      setup->cache_limit = 512;
      setup->read_mbs = 21.;
      setup->write_mbs = 10.;
   } else if(! strcmp("sf", setup->hostname)) {
      strcpy(filename, "/wrk/user/");
      setup->cache_limit = 64;
      setup->read_mbs = 21.;
      setup->write_mbs = 10.;
   } else if(! strcmp("sk", setup->hostname)) {
      strcpy(filename, "/wrk/user/");
      setup->cache_limit = 128;
      setup->read_mbs = 21.;
      setup->write_mbs = 10.;
   } else {
      #if defined(HOST_SGI)
         #define DEFAULTS_DONE
         strcpy(filename, "/tmp/user/");
         setup->cache_limit = 375;
         setup->read_mbs = 5.0;
         setup->write_mbs = 3.;
      #elif defined(HOST_Sun)
         #define DEFAULTS_DONE
         strcpy(filename, "/tmp/user/");
         setup->cache_limit = 64;
         setup->read_mbs = 3.;
         setup->write_mbs = 2.;
      #else
         strcpy(filename, "/tmp/");
         setup->cache_limit = 50;
         setup->read_mbs = 5.;
         setup->write_mbs = 3.;
      #endif
   }

   /* command name provides the base name for the tst file. */
   if(argc > 0 && argv[0]) {
      base_name = strrchr(argv[0], '/');
      if(base_name)
         base_name++;
      else
         base_name = argv[0];
   } else {
      base_name = "tryTst";
   }

   strcat(filename, setup->hostname);
   strcat(filename, ":");
   strcat(filename, base_name);

   for(i = 1; i < argc - 1; i++) {
      if(argv[i] && argv[i + 1] && ! strcmp(argv[i], "-f")) {
         /* command line override "-f {path/}{filename}" */
         strcpy(filename, argv[i + 1]);
         if('/' == filename[strlen(filename) - 1]) {
            strcat(filename, setup->hostname);
            strcat(filename, ":");
            strcat(filename, base_name);
         }
         break;
      }
   }

   /* Print results to log file near the test file. */
   strcpy(logfile, filename);
   strcat(logfile, ".log");
   fprintf(stderr, "tstAny: log= %s\n", logfile);
   fflush(stderr);
   stdlog = freopen(logfile, "a+", stderr);
   if(! stdlog) {
      fprintf(stderr, "tstSystem: %s\n\topening log file %s\n",
         strerror(errno), logfile);
      exit(1);
   }

   if((stat)(logfile, &stat_buf)) {
      fprintf(stderr, "tstSystem: %s\n\tstat on log file %s\n",
         strerror(errno), logfile);
      exit(1);
   }
#if defined(_WIN32) || defined(MINGW)
   setup->blk_size = 4 * GIO_KB;
#else
   if(0 < stat_buf.st_blksize) {
      setup->blk_size = stat_buf.st_blksize;
   } else {
      setup->blk_size = 4 * GIO_KB;
   }
#endif
   setvbuf(stdlog, NULL, _IOLBF, 8 * GIO_KB);

   /* File limit is 120% size of system io cache. */
   setup->file_limit = 1.20 * setup->cache_limit * GIO_MB;

   /* Nominal time for one pass through entire file. */
   if(setup->write_mbs < setup->read_mbs) {
      setup->time_limit = setup->file_limit / (GIO_MB * setup->write_mbs);
   } else {
      setup->time_limit = setup->file_limit / (GIO_MB * setup->read_mbs);
   }

   /* Minimum of 1 minute, maximum of 2 minutes per trial. */
   if(setup->time_limit < 1.0 * 60) setup->time_limit = 1.0 * 60;
   if(setup->time_limit > 2.0 * 60) setup->time_limit = 2.0 * 60;

   return;
}
