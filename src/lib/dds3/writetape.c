#include "dds_sys.h"
#include <fcntl.h>

int main(int argc, char **argv)
{
   /* write a shit load of blocks to a tape */
   int i, j, k, m, n, fd, n_write;
   int *buf;
   char name[2048];

   setargcv(argc, argv);
   cdds_dict("cmd:", "scan");

   k = 32;
   cdds_scanf("k", "%i", &k);
   j = k * 1024;
   m = j / sizeof(int);

   strcpy(name, "/dev/rmt/0ln");
   cdds_scanf("dev", "%s", name);

   buf = ddsMalloc(j);
   for(i = 0; i < m; i++) buf[i] = i;

   fd = open(name, O_RDWR, (int)0644);
   errno = 0;

   for(n = 0; 1; n++) {
      if(! (n % 1024))
         ddsWarn("writetape: block= %ik, n= %i\n", k, n);
      buf[0] = n;
      buf[m-1] = -n;
      if(j != (n_write = write(fd, buf, j))) {
         ddsWarn("writetape: name= %s\n\tblock= %ik, n= %i, last write= %i, errno= %i\n",
            name, k, n, n_write, errno);
         ddsMTStatus(fd, "after short write");
         errno = 0;
         ddsMTWEOF(fd, 2);
         ddsWarn("writetape: ddsMTWEOF errno= %i\n", errno);
         ddsMTStatus(fd, "after write EOF");
         break;
      }
   }
   return 0;
}
