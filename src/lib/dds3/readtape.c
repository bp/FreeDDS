#include "dds_sys.h"
#include <fcntl.h>

int main(int argc, char **argv)
{
   /* read a shit load of blocks from a tape */
   int i, j, k, m, n, fd, n_read, start;
   int *buf;
   char name[2048];

   setargcv(argc, argv);
   cdds_dict("cmd:", "scan");

   start = 0;
   cdds_scanf("start", "%i", &start);
   k = 32;
   cdds_scanf("k", "%i", &k);
   j = k * 1024;
   m = j / sizeof(int);

   strcpy(name, "/dev/rmt/0ln");
   cdds_scanf("dev", "%s", name);

   buf = ddsMalloc(j);
   for(i = 0; i < m; i++) buf[i] = i;

   fd = open(name, O_RDWR, (int)0644);

   for(n = start; 1; n++) {
      if(! (n % 1024))
         ddsWarn("readtape: block= %ik, n= %i\n", k, n);
      buf[0] = -1;
      buf[m-1] = +1;
      if(j != (n_read = read(fd, buf, j))) {
         ddsWarn("readtape: name= %s\n\tblock= %ik, n= %i, last read= %i, errno= %i\n",
            name, k, n, n_read, errno);
         ddsMTStatus(fd, "after short read");
         break;
      } else if(buf[0] != n || buf[m-1] != -n) {
         ddsWarn("readtape: name= %s\n\tblock= %ik, n= %i, last read= %i, errno= %i\n",
            name, k, n, n_read, errno);
         ddsMTStatus(fd, "after bad read");
         if(n_read <= 0) break;
      }
   }
   return 0;
}
