static char rcsid[]="$Id: devstat.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, examine tape device status

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include <fcntl.h>

#include "dds_mtio.h"

#if !defined(_WIN32) && !defined(MINGW)
static struct mtop mt_command;
static struct mtget mtiocget;
#endif
static int rc_MTIOCTOP, errno_MTIOCTOP, rc_MTIOCGET, errno_MTIOCGET;
static STRUCTSTAT stat_buf;
static int rc_FSTAT, errno_FSTAT;

#if (defined(HOST_Sun) && ! defined(HOST_sun4))
   static struct mtdrivetype getdrivetype;
   static struct mtdrivetype_request request =
      {sizeof(struct mtdrivetype), &getdrivetype};
   static int rc_GETDRIVETYPE, errno_GETDRIVETYPE;
#elif defined(HOST_SGI)
   static struct mt_capablity capability;
   static int rc_CAPABILITY, errno_CAPABILITY;
   static struct mtblkinfo blkinfo;
   static int rc_GETBLKINFO, errno_GETBLKINFO;
#endif

static int SleepPrompt(
   int sleep_sec,
   const char *prompt)
{
   int flag = 1;  /* assume affirmative */
   char reply[8];

   if(sleep_sec > 0) {
      sleep(sleep_sec);
   } else if(sleep_sec < 0) {
      fprintf(stderr, "devstat: %s [y|n] y ? ", prompt);
      /* white space after 's' is required to eat newline. */
      fscanf(stdin, "%7s", reply);

      if(*reply == 'n' || *reply == 'N') flag = 0;
   }

   return flag;
}
 
static void DumpStat(
   int fd,
   int sleep_sec,
   const char *msg)
{
   rc_MTIOCTOP = 0;
   errno_MTIOCTOP = ENOSYS;

   if(SleepPrompt(sleep_sec, "issue MTREW/MTNOP")) {
      #ifdef HOST_Sun
         /* If rewind returns an error, then drive is not reloaded. */
         errno = 0;
         mt_command.mt_op = MTREW;
         mt_command.mt_count = 1;
         rc_MTIOCTOP = ioctl(fd, MTIOCTOP, &mt_command);
         errno_MTIOCTOP = errno;
      #else
         /* Still looking for a kludge to solve this one... */
         /* on SGI DLT, UNLOAD or OFFL work (operate handle lights up) */
         /* but the MTIOCGET command immediately puts it back online */
         /* assuming the tape is still inserted...  making it impossible */
         /* to detect when a _new_ tape has been inserted. */
         /* MTNOP does not seem to bring it online, nor does it provide */
         /* useful info regarding the offline state. */
/***
         errno = 0;
         mt_command.mt_op = MTNOP;
         mt_command.mt_count = 1;
         rc_MTIOCTOP = ioctl(fd, MTIOCTOP, &mt_command);
         errno_MTIOCTOP = errno;
***/
      #endif
   }

   rc_FSTAT = 0;
   errno_FSTAT = ENOSYS;

   if(SleepPrompt(sleep_sec, "issue fstat")) {
      errno = 0;
      rc_FSTAT = ddsFSTAT(fd, &stat_buf);
      errno_FSTAT = errno;
   }

   rc_MTIOCGET = 0;
   errno_MTIOCGET = ENOSYS;

   if(SleepPrompt(sleep_sec, "issue MTIOCGET")) {
      errno = 0;
#if !defined(_WIN32) && !defined(MINGW)
      rc_MTIOCGET = ioctl(fd, MTIOCGET, (char *)&mtiocget);
      errno_MTIOCGET = errno;
#endif
   }

   fprintf(stdout, "\nDumpStat: %s\n", msg);

   fprintf(stdout, "   fstat rc= %i, errno= %i, %s\n",
      rc_FSTAT, errno_FSTAT,
      strerror(errno_FSTAT));

   fprintf(stdout, "      st_dev= 0x%lx\n", (long)stat_buf.st_dev);
   fprintf(stdout, "      st_ino= %li\n", (long)stat_buf.st_ino);
   fprintf(stdout, "      st_mode= 0x%lx\n", (long)stat_buf.st_mode);
   fprintf(stdout, "      st_nlink= %li\n", (long)stat_buf.st_nlink);
   fprintf(stdout, "      st_rdev= 0x%lx\n", (long)stat_buf.st_rdev);
   fprintf(stdout, "      st_size= %li\n", (long)stat_buf.st_size);
#if !defined(_WIN32) && !defined(MINGW)
   fprintf(stdout, "      st_blksize= %li\n", (long)stat_buf.st_blksize);
   fprintf(stdout, "      st_blocks= %li\n", (long)stat_buf.st_blocks);
#endif

   fprintf(stdout, "   ioctl MTREW rc= %i, errno= %i, %s\n",
      rc_MTIOCTOP, errno_MTIOCTOP,
      strerror(errno_MTIOCTOP));

   #ifdef HOST_Sun
      fprintf(stdout, "   ioctl MTIOCGET rc= %i, errno= %i, %s\n",
         rc_MTIOCGET, errno_MTIOCGET,
         strerror(errno_MTIOCGET));

      fprintf(stdout, "      mt_type= 0x%x\n", (int)mtiocget.mt_type);
      fprintf(stdout, "      mt_dsreg= 0x%x\n", (int)mtiocget.mt_dsreg);
      fprintf(stdout, "      mt_erreg= 0x%x\n", (int)mtiocget.mt_erreg);
      fprintf(stdout, "      mt_resid= %li\n", (long)mtiocget.mt_resid);
      fprintf(stdout, "      mt_fileno= %li\n", (long)mtiocget.mt_fileno);
      fprintf(stdout, "      mt_blkno= %li\n", (long)mtiocget.mt_blkno);
      fprintf(stdout, "      mt_flags= 0x%x\n", (int)mtiocget.mt_flags);
      fprintf(stdout, "      mt_bf= %i\n", (int)mtiocget.mt_bf);

      #ifndef HOST_sun4
         if(SleepPrompt(sleep_sec, "issue MTIOCGETDRIVETYPE")) {
            /*** This crashes gsul16, RLS 29 Aug 97
            errno = 0;
            rc_GETDRIVETYPE = ioctl(fd, MTIOCGETDRIVETYPE, (char *)&getdrivetype);
            errno_GETDRIVETYPE = errno;
            ***/

            errno = 0;
            rc_GETDRIVETYPE = ioctl(fd, MTIOCGETDRIVETYPE, (char *)&request);
            errno_GETDRIVETYPE = errno;

            fprintf(stdout, "   ioctl MTIOCGETDRIVETYPE rc= %i, errno= %i, %s\n",
               rc_GETDRIVETYPE, errno_GETDRIVETYPE,
               strerror(errno_GETDRIVETYPE));

            fprintf(stdout, "      name= '%.64s'\n", (char*)getdrivetype.name);
            fprintf(stdout, "      vid= '%.25s'\n", (char*)getdrivetype.vid);
            fprintf(stdout, "      type= 0x%x\n", (int)getdrivetype.type);
            fprintf(stdout, "      bsize= %i\n", (int)getdrivetype.bsize);
            fprintf(stdout, "      options= 0x%x\n", (int)getdrivetype.options);
            fprintf(stdout, "      max_rretries= %i\n", (int)getdrivetype.max_rretries);
            fprintf(stdout, "      max_wretries= %i\n", (int)getdrivetype.max_wretries);
            fprintf(stdout, "      densities= 0x%x 0x%x 0x%x 0x%x\n",
               (int)getdrivetype.densities[0], (int)getdrivetype.densities[1],
               (int)getdrivetype.densities[2], (int)getdrivetype.densities[3]);
            fprintf(stdout, "      default_density= 0x%x\n",
               (int)getdrivetype.default_density);
            fprintf(stdout, "      speeds= 0x%x 0x%x 0x%x 0x%x\n",
               (int)getdrivetype.speeds[0], (int)getdrivetype.speeds[1],
               (int)getdrivetype.speeds[2], (int)getdrivetype.speeds[3]);
         }
      #endif
   #elif defined(HOST_SGI)
      fprintf(stdout, "   ioctl STATUS rc= %i, errno= %i, %s\n",
         rc_MTIOCGET, errno_MTIOCGET, strerror(errno_MTIOCGET));

      fprintf(stdout, "      mt_type= %i\n", (int)mtiocget.mt_type);
      fprintf(stdout, "      mt_dposn= 0x%x\n", (int)mtiocget.mt_dposn);
      fprintf(stdout, "      mt_dsreg= 0x%x\n", (int)mtiocget.mt_dsreg);
      fprintf(stdout, "      mt_erreg= 0x%x\n", (int)mtiocget.mt_erreg);
      fprintf(stdout, "      mt_resid= %li\n", (long)mtiocget.mt_resid);
      fprintf(stdout, "      mt_fileno= %li\n", (long)mtiocget.mt_fileno);
      fprintf(stdout, "      mt_blkno= %li\n", (long)mtiocget.mt_blkno);

      if(SleepPrompt(sleep_sec, "issue MTCAPABILITY")) {
         errno = 0;
         rc_CAPABILITY = ioctl(fd, MTCAPABILITY, (char *)&capability);
         errno_CAPABILITY = errno;

         fprintf(stdout, "   ioctl CAPABILITY rc= %i, errno= %i, %s\n",
            rc_CAPABILITY, errno_CAPABILITY, strerror(errno_CAPABILITY));

         fprintf(stdout, "      mtsubtype= 0x%x\n", (int)capability.mtsubtype);
         fprintf(stdout, "      mtcapablity= 0x%x\n", (int)capability.mtcapablity);
      }

      if(SleepPrompt(sleep_sec, "issue MTIOCGETBLKINFO")) {
         errno = 0;
         rc_GETBLKINFO = ioctl(fd, MTIOCGETBLKINFO, (char *)&blkinfo);
         errno_GETBLKINFO = errno;

         fprintf(stdout, "   ioctl GETBLKINFO rc= %i, errno= %i, %s\n",
            rc_GETBLKINFO, errno_GETBLKINFO, strerror(errno_GETBLKINFO));

         fprintf(stdout, "      minblksz= %u\n", (unsigned)blkinfo.minblksz);
         fprintf(stdout, "      maxblksz= %u\n", (unsigned)blkinfo.maxblksz);
         fprintf(stdout, "      curblksz= %u\n", (unsigned)blkinfo.curblksz);
         fprintf(stdout, "      recblksz= %u\n", (unsigned)blkinfo.recblksz);
      }
   #else
      fprintf(stdout, "devstat not supported on this host\n");
   #endif

   return;
}

#if !defined(_WIN32) && !defined(MINGW)

#define SLEEPY 2

int main(int argc, char **argv)
{
   /* print stdin mag tape status */
   int fd, n;
   int test_unload = 1, sleep_sec = 0;
   const char *arg, *dev_name = "/dev/tape", *flag;
   char reply[8];

   if(argc == 1) {
      fprintf(stderr,
         "devstat: device status test and display\n"
         "   usage: devstat [<flag>] <file>\n"
         "   <flags>: any combination of the following\n"
         "      +/-:  enable/disable subsequent option\n"
         "      u:    unload and reload sequence\n"
         "      s:    sleepy status mode\n"
         "      p:    prompt status mode\n"
         "      Default is '+u-s'\n"
         "   Default <file> is /dev/tape\n"
         "   Example: devstat /dev/rmt/0l > log\n"
         );
      exit(1);
   }

   for(n = 1; n < argc; n++) {
      arg = argv[n];

      if(! arg) {
         /* skip nonsense */
         continue;
      } else if(*arg == '-' || *arg == '+') {
         /* process flags */
         for(flag = arg + 1; *flag; flag++) {
            if(*flag == '+') {
               /* enable subsequent flags */
               arg = flag;
            } else if(*flag == '-') {
               /* disable subsequent flags */
               arg = flag;
            } else if(*flag == 'u') {
               test_unload = (*arg == '+') ? 1 : 0;
            } else if(*flag == 's') {
               /* enable auto sleep (disable prompt) */
               sleep_sec = (*arg == '+') ? SLEEPY : 0;
            } else if(*flag == 'p') {
               /* enable user prompt (disable sleep) */
               sleep_sec = (*arg == '+') ? -1 : 0;
            }
         }
         continue;
      } else {
         /* device name selection and testing */
         dev_name = arg;
      }

      fprintf(stderr, "opening file descriptor for %s\n", dev_name);
      fd = open(dev_name, O_RDONLY, (int)0644);

      if(0 <= fd) {
         /* initial status after opening device */
         fprintf(stdout, "\n###########################################\n"
            "devstat: dump status for device= %s\n", dev_name);

         DumpStat(fd, sleep_sec,
            "initial status of device after open");

         if(test_unload) {
            #ifdef HOST_SGI
               mt_command.mt_op = MTUNLOAD;  /* i think MTOFFL works now too */
            #else
               mt_command.mt_op = MTOFFL;
            #endif

            fprintf(stderr, "calling ioctl to unload device...\n");

            mt_command.mt_count = 1;
            ioctl(fd, MTIOCTOP, &mt_command);

mt_command.mt_op = MTOFFL;
mt_command.mt_count = 1;
ioctl(fd, MTIOCTOP, &mt_command);

mt_command.mt_op = MTRST;
mt_command.mt_count = 1;
ioctl(fd, MTIOCTOP, &mt_command);

            DumpStat(fd, sleep_sec,
               "immediately after unload ioctl (while flashing)");

            fprintf(stderr, "wait for unload completion, then <return>");
            fscanf(stdin, "%7s", reply);

            DumpStat(fd, sleep_sec,
               "after ioctl unload has completed");

            fprintf(stderr, "reload device, then <return> "
               "(immediately, while flashing)");
            fscanf(stdin, "%7s", reply);

            DumpStat(fd, sleep_sec,
               "immediately after reloading device (while flashing)");

            #ifdef HOST_Sun
/***
               mt_command.mt_op = MTLOAD;
               ioctl(fd, MTIOCTOP, &mt_command);
               fprintf(stderr, "the reload SHOULD now be completed ???\n");
***/
            #endif

            fprintf(stderr, "wait for tape ready (stop flashing)"
               ", then <return>");
            fscanf(stdin, "%7s", reply);

            DumpStat(fd, sleep_sec,
               "after reload and device ready (flashing stopped)");

            fprintf(stderr, "manually initiate unload, then <return> "
               "(immediately, while flashing)");
            fscanf(stdin, "%7s", reply);

            DumpStat(fd, sleep_sec,
               "immediately after manual unload initiation (while flashing)");

            fprintf(stderr, "wait for unload completion, then <return>");
            fscanf(stdin, "%7s", reply);

            DumpStat(fd, sleep_sec,
               "after manual unload has completed");
         }

         close(fd);
      } else {
         fprintf(stdout, "\n###########################################\n"
            "devstat: %s\n   device= %s\n", strerror(errno), dev_name);
      }
   }

   exit(0);
}

#endif
