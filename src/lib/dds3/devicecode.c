static char rcsid[]="$Id: devicecode.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, convert (tape) drive device code into mnemonic

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_mtio.h"

#if !defined(_WIN32) && !defined(MINGW) && !defined(Darwin)
static struct mtget mtiocget;
#endif
static int rc_MTIOCGET;
static STRUCTSTAT stat_buf;
#if (defined(HOST_Sun) && ! defined(HOST_sun4))
   static struct mtdrivetype getdrivetype;
   static struct mtdrivetype_request request =
      {sizeof(struct mtdrivetype), &getdrivetype};
   static int rc_GETDRIVETYPE, errno_GETDRIVETYPE;
#elif defined(HOST_SGI)
   static struct mt_capablity capability;
   static int rc_CAPABILITY;
#endif

static char mnemonic[64];

const char * ddsDeviceCode(
   int fd)
{
   /* magnetic media type default */
   strcpy(mnemonic, "media_unknown");
 
#if !defined(_WIN32) && !defined(MINGW) && !defined(Darwin)
   rc_MTIOCGET = ioctl(fd, MTIOCGET, (char *)&mtiocget);
#endif
   ddsFSTAT(fd, &stat_buf); 

   #ifdef HOST_Sun
      #ifndef HOST_sun4
         errno = 0;
         rc_GETDRIVETYPE = ioctl(fd, MTIOCGETDRIVETYPE, (char *)&request);
         errno_GETDRIVETYPE = errno;
      #endif

      if(! rc_MTIOCGET) {
         switch(mtiocget.mt_type) {
         case MT_ISSYSGEN11:
            strcpy(mnemonic, "tape_Q11/24_600");
            break;
         case MT_ISSYSGEN:
            strcpy(mnemonic, "tape_Q11/24_600");
            break;
         case MT_ISVIPER1:
         case MT_ISWANGTEK1:
            strcpy(mnemonic, "tape_Q150_600");
            break;
         case MT_ISCCS24: /* from iqc0 */
            strcpy(mnemonic, "tape_3480");
            break;
         case MT_ISCDC:
         case MT_ISFUJI:
         case MT_ISKENNEDY:
         case MT_ISHP:
#ifdef MT_ISXY
         case MT_ISXY:
#endif
         case 1: /* kludge from gpmqc */
            if((stat_buf.st_rdev & MT_DENSITY_MASK) == MT_DENSITY1)
               strcpy(mnemonic, "tape_NRZI_2400");
            else if((stat_buf.st_rdev & MT_DENSITY_MASK) == MT_DENSITY2)
               strcpy(mnemonic, "tape_PE_2400");
            else
               strcpy(mnemonic, "tape_GCR_2400");
            break;
         case MT_ISEXABYTE:
            strcpy(mnemonic, "tape_P5-90");
            break;
         case MT_ISEXB8500:
            if((stat_buf.st_rdev & MT_DENSITY_MASK) == MT_DENSITY1)
               strcpy(mnemonic, "tape_P5-90");
            else
               strcpy(mnemonic, "tape_P5-90_H");
            break;
         #ifndef HOST_sun4
         case 0x38:
            strcpy(mnemonic, "tape_DLT4");
            break;
         #endif
         case MT_ISDEFAULT:
            #ifndef HOST_sun4
               if(! rc_GETDRIVETYPE) {
                  if(strstr(getdrivetype.name, "DLT4")) {
                     strcpy(mnemonic, "tape_DLT4");
                  }
               }
            #endif
            break;
         }
      }
   #elif defined(HOST_SGI)
      rc_CAPABILITY = ioctl(fd, MTCAPABILITY, (char *)&capability);

      if(! rc_MTIOCGET) {
         switch(mtiocget.mt_type) {
         case MT_ISSCSI: /* 0x09 */
            if(! rc_CAPABILITY) {
               switch(capability.mtsubtype) {
               case 0x06: /* TP8MM_8500 ? */
                  strcpy(mnemonic, "tape_P5-90_H");
                  break;
               case 0x0a: /* TP8MM_8500 ? */
                  strcpy(mnemonic, "tape_DLT4");
                  break;
               case 0x0e: /* TPSTK9490 ? */
                  strcpy(mnemonic, "tape_3590");
                  break;
               }
            } else {
               /* wild guess */
               strcpy(mnemonic, "tape_ids");
            }
            break;
         }
      }
   #endif

   return mnemonic;
}
