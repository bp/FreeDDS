static char rcsid[]="$Id: mediafirst.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS, mount first media onto drive

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_magtape.h"

int ddsMediaFirst(
   DDS_MEDIA *media,
   const char *mode,
   const char *dict)
{
   static DDS_STRING alias = NULL_STRING;
   static DDS_TOKEN list = NULL_TOKEN;
   DDS_MEDIA *chain, *chain2;
   char scratch[128];
   int n_label;
   const char *filename, *value, *label, *device_genus;

   /***********************************************/
   /* Initialize device label information.        */
   /* These are local parameters only, i.e.       */
   /*    /dev/rmt/1hn= label "tape1 tape2" fsf 1  */
   /***********************************************/

   cdds_dict(dict, "scan");
   ddsStrCpyCat3(&alias, media->name, " ", DDS_SENTRY);

   n_label = 0;

   if(0 < cdds_scank(alias.ptr, "path slot label volser dsn") &&
      strcmp(dds_scan_name, DDS_SENTRY)) {
      cdds_scant("", &value);
      switch(*value) {
      case 'p': media->mount = DDS_MEDIA_PATH; break;
      case 's': media->mount = DDS_MEDIA_SLOT; break;
      case 'l': media->mount = DDS_MEDIA_LABEL; break;
      case 'v': media->mount = DDS_MEDIA_VOLSER; break;
      case 'd': media->mount = DDS_MEDIA_DSN; break;
      }
      if(0 < cdds_scant("", &value)) {
         /* Create media for each defined label. */
         /* Only the "label" and "next" are initialized. */
         for(label = TOKEN_INIT(list, value);
            label;
            label = TOKEN_NEXT(list)) {
            sprintf(scratch, " (%i) ", ++n_label);

            if(n_label == 1) {
               /* label and title for the first media */
               media->label = ddsString(label);
               ddsStrCat2(&media->title, scratch, label);
               chain = media;
            } else {
               if(media->state & DDS_MEDIA_NOLOAD) {
                  ddsError("multiple labels not allowed\n\tbinary= %s\n",
                     media->title.ptr);
                  break;
               }
               /* label and title for subsequent media */
               chain2 = chain;
               chain = (DDS_MEDIA*)ddsZalloc(sizeof(DDS_MEDIA));
               chain2->next = chain;
               chain->prev = chain2;

               chain->label = ddsString(label);
               ddsStrCpyCat5(&chain->title, dds_media_genus[media->genus],
                  " ", media->name, scratch, label);
            }
         }

         if(! media->next && ! strncmp(media->label, "'\"", 2)) {
            ddsError("media label syntax error (%s)\n\t%s\n",
               ddsHint(value), media->name);
         }
      }
   } 

   if(! n_label) {
      n_label++;
      media->label = ddsString("<no label>");
      ddsStrCat(&media->title, " (1) <no label>");
   }

   if(0 < cdds_scank(alias.ptr, "fsf") &&
      strcmp(dds_scan_name, DDS_SENTRY))
      cdds_scanf("", " %*s %d", &media->file_skip);

   if(0 < cdds_scank(alias.ptr, "unload") &&
      strcmp(dds_scan_name, DDS_SENTRY))
      media->state |= DDS_MEDIA_UNLOAD;

   /****************************************/
   /* mount media, starting from the first */
   /****************************************/

   if(ddsMediaLoad(media, mode)) {
      /* This is a fatal error for future mounts because */
      /* media and device parameters are not initialized. */
      return DDS_BAD;
   }

   /************************************************/
   /* Initialize media and device parameters, i.e. */
   /*    tape_P5-90_H= length 366 feet             */
   /*    /dev/rmt/1hn= length 366 feet             */
   /************************************************/

   device_genus = ddsDeviceCode(gfileno(media->fp));

   if(device_genus) {
      /* default device specifications, via ddsDeviceGenus */

      cdds_dict("device:", "scan");

      if(! cdds_scanf(device_genus, "")) {
         ddsMediaScan(device_genus, media);
      }

      filename = ddsFindDDSFile("device.dds", "par:", "");

      if(*filename) {
         cdds_dict(filename, "scan");

         if(! cdds_scanf(device_genus, "")) {
            ddsMediaScan(device_genus, media);
         }
      }
   }

   cdds_dict(dict, "scan");
   ddsStrCpyCat3(&alias, media->name, " ", DDS_SENTRY);

   if(! cdds_scanf(alias.ptr, "") &&
      strcmp(dds_scan_name, DDS_SENTRY)) {
      ddsMediaScan(media->name, media);
   }

   media->toll = 1.0 / media->density;

   /* finish initialization of chained media */
   for(chain = media->next; chain; chain = chain->next) {
      ddsMediaChain(media, chain);
   }

   return DDS_OK;
}
