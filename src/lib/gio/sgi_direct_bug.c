/*** sgi_direct_bug.c   VERSION 2 8/17/95
   8/8/95 Randy Selzler (918)660-3413 rselzler@trc.amoco.com
   Amoco Production Company, Tulsa Technology Center.

   Direct i/o example 
      Primary problems:
         1) system crashes
         2) read verify errors
      Secondary problems:
         1) read error, if fsync isn't called after buffered write
         2) read error, if read size = d_maxiosz
***/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

#define FILENAME "/data4/zrls13/test"

int main(int argc, const char** argv)
{
   int fd, n_io, i, rc;
   struct stat stat_buf;
   struct dioattr sgi_dio;
   int PAGESIZE, BLOCKSIZE, FILESIZE, FILEWORDS, BUFSIZE, BUFWORDS;
   int *buf;

   /*** create file (buffered) and initialize contents ***/

   PAGESIZE = getpagesize();

   fd = open(FILENAME, O_CREAT | O_TRUNC | O_WRONLY, (int)0666);
   assert(fd >= 0);

   rc = fstat(fd, &stat_buf);
   assert(rc == 0);
   BLOCKSIZE = stat_buf.st_blksize;

   /***
      if N = 1024, then  ok.
      if N =  800, then  verify error beginning at i = 2176.
      if N =  512, then  ok.
      if N =  256, then  verify error beginning at i = 2048.
      given "FILESIZE = BLOCKSIZE + N"

      if FILESIZE = 128, then process hangs
         and or crashes IRIX64 6.0.1 (perhaps after some delay)
   ***/

   FILESIZE = BLOCKSIZE + 1024;
   FILEWORDS = FILESIZE / sizeof(int);
   buf = (int*)malloc(FILESIZE);

   for(i = 0; i < FILEWORDS; i++) buf[i] = i;

   n_io = write(fd, buf, FILESIZE);
   assert(n_io == FILESIZE);

   fsync(fd); /* direct read error, if fsync isn't called */
   close(fd);
   free(buf);

   /*** open existing file for direct i/o and verify contents ***/

   fd = open(FILENAME, O_RDONLY | O_DIRECT, (int)0666);
   assert(fd >= 0);

   rc = fcntl(fd, F_DIOINFO, &sgi_dio);
   assert(rc == 0);

   printf("FILESIZE= %i, PAGESIZE= %i, BLOCKSIZE= %i\n",
      (int)FILESIZE, (int)PAGESIZE, (int)BLOCKSIZE);
   printf("d_miniosz= %i, d_maxiosz= %i, d_mem= %i\n",
      (int)sgi_dio.d_miniosz, (int)sgi_dio.d_maxiosz, (int)sgi_dio.d_mem);

   /***
      if BUFSIZE = sgi_dio.d_maxiosz, then read errno= 12 (Not enough space).
      if BUFSIZE = sgi_dio.d_maxiosz - sgi_dio.d_miniosz, then ok.
   ***/

   /***
      On Amoco "gssgi2" (the failing system) PAGESIZE *equals* d_mem,
      and BLOCKSIZE *equals* d_miniosz.   This may not be true on other
      sgi systems.

      The first version of this test was not portable, because the following
      three line referenced PAGESIZE and BLOCKSIZE (not d_mem and d_miniosz).
      This version *is* portable, because it uses d_mem and d_miniosz.

      This correction does *not* invalid the original problem report,
      based upon the first tests on "gssgi2",  i.e.
         Primary problems:
            1) system crashes
            2) read verify errors
         Secondary problems:
            1) read error, if fsync isn't called after buffered write
            2) read error, if read size = d_maxiosz
   ***/

   BUFSIZE = sgi_dio.d_miniosz * (1 + (FILESIZE - 1) / sgi_dio.d_miniosz);
   BUFWORDS = BUFSIZE / sizeof(int);
   buf = (int*)memalign(sgi_dio.d_mem, BUFSIZE);

   for(i = 0; i < BUFWORDS; i++) buf[i] = -1;

   /***
      If BUFSIZE *is* a multiple of d_miniosz, and the remaining file size
      (until eof) is *not* a multiple of 512, then direct read function
      return value (byte count) is equals to the remaining file size 8^).
      The buffer contents are correct, *except* near the end :-(.
      The tail (fraction of 512) is unchanged in the buffer.

      If BUFSIZE *is* the remaining files size (until eof), but *not*
      a multiple of d_miniosz, then the read is reject with -1 and errno.

      Question: given a file size that is *not* a multiple of 512
      (and d_miniosz?), can direct read access its entire contents ???
   ***/
   n_io = read(fd, buf, BUFSIZE);
   assert(n_io == FILESIZE);

   for(i = 0; i < FILEWORDS; i++)
      if(i != buf[i]) printf("verify error, i = %i, buf[i]= %i\n", i, buf[i]);

   close(fd);
   free(buf);

   return 0;
}
