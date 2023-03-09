static char rcsid[]="$Id: c_dict.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, select (scan/print/suspend/reset/close) dictionary(s).

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int (cdds_dict)(
   const char *name,
   const char *mode)
{
   /* "cdds_dict" is indirectly recursive through ddsDictInit. */
   /* The dictionary state is saved and restored after calling it. */
   /*    via ddsDictInit->ddsDictPar->cdds_dict */
   /*    via ddsDictInit->...->ddsDictNew->ddsMediaNew->cdds_dict */
   static DDS_TOKEN dict_save = NULL_TOKEN;
   int status = DDS_OK;
   size_t tmp_stack, scan_stack, print_stack, hist_stack;
   const char *dict_name;
   char tmp_save, *tmp_next, *tmp_name;
   DDS_DICT *dict, *dict_older, **dict_ptr;

   DDS_API_ENTRY("dict")

   while(isspace(*mode)) mode++;

   if(strcmp("scan", mode) &&
      strcmp("print", mode) &&
      strcmp("reset", mode) &&
      strcmp("close", mode) && /* deprecate */
      strcmp("suspend", mode) &&
      strcmp("freeze", mode)) {
      ddsDebug("bad 'mode' argument (%s), dictionary= %s\n",
         ddsHint(mode), ddsHint2(name));
      DDS_API_EXIT(DDS_BAD)
   }

   /* copy dictionary names onto temporary stack */
   hist_stack = dds_tmp.top;
   for(dict_name = TOKEN_INIT(dict_save, name);
      dict_name;
      dict_name = TOKEN_NEXT(dict_save)) {
      if(! strcmp(dict_name, "hist:")) {
         /* substitute dds_hist for all occurrances of "hist:" */
         ddsStackStr(&dds_tmp, dds_hist);
      } else {
         /* copy nominal name onto stack */
         ddsStackStr(&dds_tmp, dict_name);
      }
      /* replace nul terminator with blank delimiter */
      dds_tmp.ptr[dds_tmp.top - 1] = ' ';
   }

   ddsStackPushChar(&dds_tmp, '\0');

   if(dds_scan_first) {
      /* terminate current scan definition */
      ddsScanDone();
      if(! strcmp("scan", mode)) {
         /* new names will replace current scan selection */
         scan_stack = 0;
         for(dict = dds_scan_first; dict; dict = dict_older) {
            /* suspend dictionary from "scan" mode */
            dict->state &= (DDS_DICT_STATE) ~ DDS_DICT_SCAN;
            dict_older = dict->right;
            dict->left = dict->right = NULL;
         }
      } else {
         scan_stack = ddsStackPush(&dds_tmp, sizeof(DDS_DICT*), HOST_PTR_A);
         tmp_stack = scan_stack;
         for(dict = dds_scan_first; dict; dict = dict_older) {
            /* save pointers to all dictionaries on scan stack */
            *(DDS_DICT**)(dds_tmp.ptr + tmp_stack) = dict;
            tmp_stack = ddsStackPush(&dds_tmp, sizeof(DDS_DICT*), HOST_PTR_A);
            /* suspend dictionary from "scan" mode */
            dict->state &= (DDS_DICT_STATE) ~ DDS_DICT_SCAN;
            dict_older = dict->right;
            dict->left = dict->right = NULL;
         }
         *(DDS_DICT**)(dds_tmp.ptr + tmp_stack) = NULL; /* terminator */
      }
      dds_scan_first = dds_scan_last = NULL;
   } else {
      /* nothing to restore */
      scan_stack = 0;
   }

   if(dds_print_first) {
      /* terminate current print definition */
      if(ddsPrintDone()) status = DDS_BAD;
      if(! strcmp("print", mode)) {
         /* new names will replace current print selection */
         print_stack = 0;
         for(dict = dds_print_first; dict; dict = dict_older) {
            /* suspend dictionary from "print" mode */
            dict->state &= (DDS_DICT_STATE) ~ DDS_DICT_PRINT;
            dict_older = dict->right;
            dict->left = dict->right = NULL;
         }
      } else {
         print_stack = ddsStackPush(&dds_tmp, sizeof(DDS_DICT*), HOST_PTR_A);
         tmp_stack = print_stack;
         for(dict = dds_print_first; dict; dict = dict_older) {
            /* save pointers to all dictionaries on print stack */
            *(DDS_DICT**)(dds_tmp.ptr + tmp_stack) = dict;
            tmp_stack = ddsStackPush(&dds_tmp, sizeof(DDS_DICT*), HOST_PTR_A);
            /* suspend dictionary from "print" mode */
            dict->state &= (DDS_DICT_STATE) ~ DDS_DICT_PRINT;
            dict_older = dict->right;
            dict->left = dict->right = NULL;
         }
         *(DDS_DICT**)(dds_tmp.ptr + tmp_stack) = NULL; /* terminator */
      }
      dds_print_first = dds_print_last = NULL;
   } else {
      /* nothing to restore */
      print_stack = 0;
   }

   if(mode[0] != 'r' && mode[0] != 'c' && mode[0] != 'f') {
      /* Initialize all dictionaries that are new. */
      /* RECURSIVE !!!  cdds_dict may be called again. */
      for(tmp_name = dds_tmp.ptr + hist_stack;
         DDS_TRUE;
         tmp_name = tmp_next) {

         while(*tmp_name && isspace(*tmp_name)) tmp_name++;
         if(! *tmp_name) break;

         tmp_next = tmp_name;
         while(*tmp_next && ! isspace(*tmp_next)) tmp_next++;
         tmp_save = *tmp_next;
         *tmp_next = '\0';  /* nul terminator for name */

         dict = (DDS_DICT*)ddsTableLookup(&dds_dict, tmp_name);
         if(! dict) {
            /* initial access to this dictionary */
            dict = ddsDictInit(tmp_name, mode);
            if(! dict) status = DDS_BAD;
         }
         *tmp_next = tmp_save; /* restore white space delimiter */
      }
   }

   if(dds_scan_first) {
      /* cleanup after recursive initialization. */
      /* terminate ddsDictInit scan definition */
      ddsScanDone();
      for(dict = dds_scan_first; dict; dict = dict_older) {
         /* suspend dictionary from "scan" mode */
         dict->state &= (DDS_DICT_STATE) ~ DDS_DICT_SCAN;
         dict_older = dict->right;
         dict->left = dict->right = NULL;
      }
      dds_scan_first = dds_scan_last = NULL;
   }

   if(dds_print_first) {
      /* cleanup after recursive initialization. */
      /* terminate ddsDictInit print definition */
      if(ddsPrintDone()) status = DDS_BAD;
      for(dict = dds_print_first; dict; dict = dict_older) {
         /* suspend dictionary from "print" mode */
         dict->state &= (DDS_DICT_STATE) ~ DDS_DICT_PRINT;
         dict_older = dict->right;
         dict->left = dict->right = NULL;
      }
      dds_print_first = dds_print_last = NULL;
   }

   if(scan_stack) {
      /* restore scan dictionary state */
      for(dict_ptr = (DDS_DICT**)(dds_tmp.ptr + scan_stack);
         *dict_ptr;
         dict_ptr++) {
         (*dict_ptr)->state |= DDS_DICT_SCAN;
         if(dds_scan_last) {
            /* append to end of precedence chain */
            dds_scan_last->right = *dict_ptr;
            (*dict_ptr)->left = dds_scan_last;
            assert((*dict_ptr)->right == NULL);
            dds_scan_last = *dict_ptr;
         } else {
            /* first entry on precedence chain */
            dds_scan_first = dds_scan_last = *dict_ptr;
            assert((*dict_ptr)->left == NULL &&
               (*dict_ptr)->right == NULL);
         }
      }
   }

   if(print_stack) {
      /* restore print dictionary state */
      for(dict_ptr = (DDS_DICT**)(dds_tmp.ptr + print_stack);
         *dict_ptr;
         dict_ptr++) {
         (*dict_ptr)->state |= DDS_DICT_PRINT;
         if(dds_print_last) {
            /* append to end of precedence chain */
            dds_print_last->right = *dict_ptr;
            (*dict_ptr)->left = dds_print_last;
            assert((*dict_ptr)->right == NULL);
            dds_print_last = *dict_ptr;
         } else {
            /* first entry on precedence chain */
            dds_print_first = dds_print_last = *dict_ptr;
            assert((*dict_ptr)->left == NULL &&
               (*dict_ptr)->right == NULL);
         }
      }
   }

   /* process all dictionary names */
   for(dict_name = TOKEN_INIT(dict_save, dds_tmp.ptr + hist_stack);
      dict_name;
      dict_name = TOKEN_NEXT(dict_save)) {

      dict = (DDS_DICT*)ddsTableLookup(&dds_dict, dict_name);
      if(! dict) {
         /* pathology: the mode is "reset", "close", or "freeze". */
         /* This dictionary does NOT already exist. */
         continue;
      }

      if(dict->state & DDS_DICT_PRINT) {
         if(mode[0] == 'p' /* "print" mode */ ) {
            /* keep print dictionary at higher precedence */
            continue;
         }
         /* terminate print mode */
         if(dict == dds_print_first) ddsPrintDone();
         dict->state &= (DDS_DICT_STATE) ~ DDS_DICT_PRINT;
         if(dict == dds_print_first) dds_print_first = dict->right;
         if(dict == dds_print_last) dds_print_last = dict->left;
         if(dict->left) dict->left->right = dict->right;
         if(dict->right) dict->right->left = dict->left;
         dict->left = dict->right = NULL;
      } else if(dict->state & DDS_DICT_SCAN) {
         if(mode[1] == 'c' /* "scan" mode */ ) {
            /* keep scan dictionary at higher precedence */
            continue;
         }
         /* terminate scan mode */
         if(dict == dds_scan_dict) ddsScanDone();
         dict->state &= (DDS_DICT_STATE) ~ DDS_DICT_SCAN;
         if(dict == dds_scan_first) dds_scan_first = dict->right;
         if(dict == dds_scan_last) dds_scan_last = dict->left;
         if(dict->left) dict->left->right = dict->right;
         if(dict->right) dict->right->left = dict->left;
         dict->left = dict->right = NULL;
      }

      if(mode[1] == 'c' /* "scan" mode */ ) {
         /* scan dictionary */
         if(dds_scan_last) {
            /* append to end of precedence chain */
            dds_scan_last->right = dict;
            dict->left = dds_scan_last;
            assert(dict->right == NULL);
            dds_scan_last = dict;
         } else {
            /* first entry on precedence chain */
            dds_scan_first = dds_scan_last = dict;
            assert(dict->left == NULL && dict->right == NULL);
         }
         dict->state |= DDS_DICT_SCAN;
      } else if(mode[0] == 'p') {
         /* print dictionary */
         if(dds_print_last) {
            /* append to end of precedence chain */
            dds_print_last->right = dict;
            dict->left = dds_print_last;
            assert(dict->right == NULL);
            dds_print_last = dict;
         } else {
            /* first entry on precedence chain */
            if(dict->state & DDS_DICT_READ_ONLY) {
               ddsError("dict: print not allowed on read-only dictionary\n\tname= %s\n",
                  dict_name);
                  status = DDS_BAD;
                  break;
            } else if(dict->state & DDS_DICT_DATA) {
               ddsDebug("dict: print on frozen dictionary (binary data attached)\n\tname= %s\n",
                  dict_name);
                  status = DDS_BAD;
                  break;
            } else if(dict->state & DDS_DICT_FROZEN) {
               ddsDebug("dict: print on frozen dictionary (binary data opened)\n\tname= %s\n",
                  dict_name);
                  status = DDS_BAD;
                  break;
            }
            dds_print_first = dds_print_last = dict;
            assert(dict->left == NULL && dict->right == NULL);
         }
         dict->state |= DDS_DICT_PRINT;
      } else if(mode[0] == 'r' || mode[0] == 'c') {
         /* reset dictionary */
         if(ddsDictDelete(dict)) status = DDS_BAD;
      } else if(mode[0] == 'f') {
         /* freeze dictionary contents */
         dict->state |= DDS_DICT_FROZEN;
      } else if(mode[0] == 's' && strstr(name, "log:")) {
         /* suspend log: dictionary */
         ddsReadLog();
      }
   }

   ddsStackSet(&dds_tmp, hist_stack);

   DDS_API_EXIT(status)
}
