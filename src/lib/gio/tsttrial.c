static char rcsid[]="$Id: tsttrial.c 13 2009-01-06 16:56:43Z user $";
/*
RCS-t=GIO, tstTrial: conduction one test/tuneup trial.
   tstSetup:    print setup, initialize statistics, validate parameters.
   tstInit:     remove file or initialize its contents.
   tstOpen:     open test file, in preparation for trial.
   tstExecute:  execute one test trial.
   tstClose:    close test file and cleanup after trial.
   tstScore:    print the raw score for one trial.
   tstverify:   verify contents of file, if trial was writing.

*/

#include "tstsys.h"

int tstTrial(           /* status (non-zero == error). */
   TST_SETUP *setup)   /* setup for one particular trial. */
{
   FILE *fp_clear, *fp_stdio;
   GFILE *fp_gio;
   int lu_usp;
   long size_limit;
   int align_limit, bias_limit;
   unsigned *ubuf;
   char *cbuf;
   char tmp_file[GIO_FILENAME_MAX+1];

   if(tstSetup(setup, &size_limit, &align_limit, &bias_limit)) return setup->rc;

   if(tstInit(setup, &fp_clear, tmp_file)) return setup->rc;

   if(tstOpen(setup, &fp_stdio, &fp_gio, &lu_usp, &cbuf, &ubuf, size_limit)) return setup->rc;

   tstExecute(setup, size_limit, align_limit, bias_limit, fp_stdio, fp_gio, lu_usp, cbuf, ubuf);

   tstClose(setup, tmp_file, fp_clear, fp_stdio, fp_gio, lu_usp, cbuf, ubuf);

   tstScore(setup);

   tstVerify(setup);

   return setup->rc;
}
