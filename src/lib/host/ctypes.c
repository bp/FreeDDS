static char rcsid[]="$Id: ctypes.c 50 2009-02-10 21:29:01Z user $";
/*
RCS-t=HOST, main, test and display various C types and definitions

*/

#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include <fcntl.h>
#include <limits.h>
#include <float.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "chost.h"

#define ALIGN(type) (int)(sizeof(struct{char x; type y;}) - sizeof(type))

#define promote(a, b)  \
   printf("sizeof((" #a ")m * (" #b ")n)= %i\n", (int)sizeof((a)m * (b)n));

#define MAX6 7
#define BIG_MIN (LONG_MIN / 10)
#define BIG_MAX (LONG_MAX / 10)
#define RANGE   64

/*! TEST Program: display various C types and definitions */
int main(argc, argv)
   int argc;
   char **argv;
{
   /* print some interesting constants */
   HOST_INT6_T int6_out, int6_in;
   HOST_LLONG_T llong_out, llong_in;
   HOST_LLONG_T i6, j6, ij6;
   HOST_LLONG_T min_max[MAX6][4] = {
         {-RANGE, +RANGE, -RANGE, +RANGE},
         {BIG_MIN, BIG_MIN + RANGE, -RANGE, +RANGE},
         {-RANGE, +RANGE, BIG_MIN, BIG_MIN + RANGE},
         {BIG_MAX - RANGE, BIG_MAX, -RANGE, +RANGE},
         {-RANGE, +RANGE, BIG_MAX - RANGE, BIG_MAX},
         {BIG_MIN, BIG_MIN + RANGE, BIG_MAX - RANGE, BIG_MAX},
         {BIG_MAX - RANGE, BIG_MAX, BIG_MIN, BIG_MIN + RANGE}
      };
   int error;
   int m = 1, n = 1;
   int i[3];
   char biggie[100];
   char c[16];

typedef union {
#ifdef HOST_LLONG_C
      signed   char Zschar;          short Zshort;          int Zint;          long Zlong;          long long Zllong;
      unsigned char u1; unsigned short u2; unsigned int u3; unsigned long u4; unsigned long long u5;
#else
      signed   char Zschar;          short Zshort;          int Zint;          long Zlong;
      unsigned char u1; unsigned short u2; unsigned int u3; unsigned long u4;
#endif
#ifdef HOST_LDOUBLE_C
      float Zfloat; double Zdouble; long double Zldouble;
#else
      float Zfloat; double Zdouble;
#endif
      void *v1;} malloc_union;

   unsigned char *swapuc;
   malloc_union swap;
   char *cbuf;
   int fd;
	off_t size;
   double fsize;
   struct stat stat_buf;

   /* check fstat */

   fd = open("ctypes.o", O_RDONLY, 0666);
	n = fstat(fd, &stat_buf);
   size = stat_buf.st_size;
   fsize = size;
   fprintf(stderr, "FSTAT: fd=%d fstat=%d st_size=%g %x %x (sizeof=%d)\n", fd, n, 
      fsize, ((int*)&size)[0], ((int*)&size)[1], (int)sizeof(size));
	n = fstat(1, &stat_buf);
   fprintf(stderr, "FSTAT: stdout fstat=%d ISREG=%d ISFIFO=%d\n", n, 
      S_ISREG(stat_buf.st_mode), S_ISFIFO(stat_buf.st_mode));

   swapuc = (unsigned char*)&swap;
   for(n = 0; n < sizeof(malloc_union); n++) {
      swapuc[n] = (unsigned char)(n + 1);
   }

   swap.Zshort = 258;
   cbuf = (char*)&swap.Zshort;
   printf("swap.short= %hi = 0x%hx (%02x%02x)\n", 
      swap.Zshort, swap.Zshort, cbuf[0], cbuf[1]);
   swap.Zint = 16909060;
   cbuf = (char*)&swap.Zint;
   printf("swap.int= %i = 0x%x (%02x%02x%02x%02x)\n", 
      swap.Zint, swap.Zint, cbuf[0], cbuf[1], cbuf[2], cbuf[3]);
   swap.Zlong = 16909060;
   printf("swap.long= %li = 0x%lx\n", swap.Zlong, swap.Zlong);
#ifdef HOST_LLONG_C
   swap.Zllong = 72623859790382856LL;
   printf("swap.long_long= %lli = 0x%llx\n", swap.Zllong, swap.Zllong);
#endif

   /*** swap.Zfloat = 8520452; ***/
   swapuc[0] = 0x4b;
   swap.Zfloat = 8520452;
   printf("swap.float= %f = 0x%x\n", swap.Zfloat, swap.Zint);
#ifdef HOST_LLONG_C
   /*** swap.Zdouble = 5069865379825416LL; ***/
   swapuc[0] = 0x43;
   swapuc[1] = 0x32;
   swap.Zdouble = 5069865379825416LL;
   printf("swap.double= %f = 0x%llx\n", swap.Zdouble, swap.Zllong);
#endif

   llong_out = 0;
   error = 0;
   for(n = 0; n < 4; n++) {
      sprintf(biggie, HOST_LLONG_QP, llong_out);
      llong_in = 0;
      sscanf(biggie,  HOST_LLONG_QS, &llong_in);
      if(llong_out != llong_in) {
         printf("ERROR HOST_LLONG_T %i '%s'\n", n, biggie);
         error++;
      }
      llong_out += 0x40000000;
   }

   if(! error) printf("HOST_LLONG_T scanf/printf OK\n");

   int6_out = 0;
   error = 0;
   for(n = 0; n < 4; n++) {
      sprintf(biggie, HOST_INT6_QP, int6_out);
      int6_in = 0;
      sscanf(biggie,  HOST_INT6_QS, &int6_in);
      if(int6_out != int6_in) {
         printf("ERROR HOST_INT6_T %i '%s'\n", n, biggie);
         error++;
      }
      int6_out += 0x40000000;
   }

   if(! error) printf("HOST_INT6_T scanf/printf OK\n");

   printf("CHAR_MIN= %i (%s)\n",
      (int)CHAR_MIN, ((CHAR_MIN) ? "signed" : "unsigned"));
 
   printf("       &i[0]= %p,        &i[1]= %p,        &i[2]= %p\n",
      i, i + 1, i + 2);
   printf("(void*)&i[0]= %p, (void*)&i[1]= %p, (void*)&i[2]= %p\n",
      (void*)i, (void*)(i + 1), (void*)(i + 2));

   printf("       &c[0]= %p,        &c[1]= %p,        &c[2]= %p\n",
      c, c + 1, c + 2);
   printf("(void*)&c[0]= %p, (void*)&c[1]= %p, (void*)&c[2]= %p\n",
      (void*)c, (void*)(c + 1), (void*)(c + 2));

   printf(" (int*)&c[0]= %p,  (int*)&c[1]= %p,  (int*)&c[2]= %p\n",
      (int*)c, (int*)(c + 1), (int*)(c + 2));

   printf("       &c[7]= %p,        &c[8]= %p,        &c[9]= %p\n",
      c + 7, c + 8, c + 9);
   printf("(void*)&c[7]= %p, (void*)&c[8]= %p, (void*)&c[9]= %p\n",
      (void*)(c + 7), (void*)(c + 8), (void*)(c + 9));

   printf(" (int*)&c[7]= %p,  (int*)&c[8]= %p,  (int*)&c[9]= %p\n",
      (int*)(c + 7), (int*)(c + 8), (int*)(c + 9));

   printf("sizeof(\"FOUR\")= %i\n", (int)sizeof("FOUR"));

   printf("RDONLY=%i, WRONLY=%i, RDWR=%i\n",
      (int)O_RDONLY, (int)O_WRONLY, (int)O_RDWR);
   printf("APPEND=%i, CREAT=%i, TRUNC=%i, EXCL=%i\n",
      (int)O_APPEND, (int)O_CREAT, (int)O_TRUNC, (int)O_EXCL);

   printf("_IOFBF=0x%lx, _IOLBF=0x%lx, _IONBF=0x%lx\n",
      (long)_IOFBF, (long)_IOLBF, (long)_IONBF);

   printf("argv= %p, *argv= %p\n", argv, *argv);

   printf("fileno(stdin)=   %i\n", (int)fileno(stdin));
   printf("fileno(stdout)=  %i\n", (int)fileno(stdout));
   printf("fileno(stderr)=  %i\n", (int)fileno(stderr));

   printf("sizeof(void*)=       %i, align= %i\n",
      (int)sizeof(void*), ALIGN(void*));
   printf("sizeof(char)=        %i, align= %i\n",
      (int)sizeof(char), ALIGN(char));
   printf("sizeof(short)=       %i, align= %i\n",
      (int)sizeof(short), ALIGN(short));
   printf("sizeof(int)=         %i, align= %i\n",
      (int)sizeof(int), ALIGN(int));
   printf("sizeof(long)=        %i, align= %i\n",
      (int)sizeof(long), ALIGN(long));
#ifdef HOST_LLONG_C
      printf("sizeof(long long)=   %i, align= %i\n",
      (int)sizeof(long long), ALIGN(long long));
#else
      printf("sizeof(long long)=   UNDEFINED\n");
#endif
   printf("sizeof(size_t)=      %i, align= %i\n",
      (int)sizeof(size_t), ALIGN(size_t));
   printf("sizeof(ptrdiff_t)=   %i, align= %i\n",
      (int)sizeof(ptrdiff_t), ALIGN(ptrdiff_t));
   printf("sizeof(off_t)=       %i, align= %i\n",
      (int)sizeof(off_t), ALIGN(off_t));
   printf("sizeof(fpos_t)=      %i, align= %i\n",
      (int)sizeof(fpos_t), ALIGN(fpos_t));
   printf("sizeof(float)=       %i, align= %i\n",
      (int)sizeof(float), ALIGN(float));
   printf("sizeof(double)=      %i, align= %i\n",
      (int)sizeof(double), ALIGN(double));
#ifdef HOST_LDOUBLE_C
      printf("sizeof(long double)= %i, align= %i\n",
      (int)sizeof(long double), ALIGN(long double));
#else
      printf("sizeof(long double)= UNDEFINED\n");
#endif

   printf("EOF= %i\n", (int)EOF);

   printf("MALLOC_A= %i\n", (int)sizeof(malloc_union));

   promote(char, char);
   promote(char, unsigned char);
   promote(char, signed char);
   promote(char, short);
   promote(char, unsigned short);
   promote(char, int);
   promote(char, unsigned);
   promote(char, long);
   promote(char, unsigned long);
#ifdef HOST_LLONG_C
   promote(char, long long);
   promote(char, unsigned long long);
#endif

   promote(unsigned char, char);
   promote(unsigned char, unsigned char);
   promote(unsigned char, signed char);
   promote(unsigned char, short);
   promote(unsigned char, unsigned short);
   promote(unsigned char, int);
   promote(unsigned char, unsigned);
   promote(unsigned char, long);
   promote(unsigned char, unsigned long);
#ifdef HOST_LLONG_C
   promote(unsigned char, long long);
   promote(unsigned char, unsigned long long);
#endif

   promote(signed char, char);
   promote(signed char, unsigned char);
   promote(signed char, signed char);
   promote(signed char, short);
   promote(signed char, unsigned short);
   promote(signed char, int);
   promote(signed char, unsigned);
   promote(signed char, long);
   promote(signed char, unsigned long);
#ifdef HOST_LLONG_C
   promote(signed char, long long);
   promote(signed char, unsigned long long);
#endif

   promote(short, char);
   promote(short, unsigned char);
   promote(short, signed char);
   promote(short, short);
   promote(short, unsigned short);
   promote(short, int);
   promote(short, unsigned);
   promote(short, long);
   promote(short, unsigned long);
#ifdef HOST_LLONG_C
   promote(short, long long);
   promote(short, unsigned long long);
#endif

   promote(unsigned short, char);
   promote(unsigned short, unsigned char);
   promote(unsigned short, signed char);
   promote(unsigned short, short);
   promote(unsigned short, unsigned short);
   promote(unsigned short, int);
   promote(unsigned short, unsigned);
   promote(unsigned short, long);
   promote(unsigned short, unsigned long);
#ifdef HOST_LLONG_C
   promote(unsigned short, long long);
   promote(unsigned short, unsigned long long);
#endif

   promote(int, char);
   promote(int, unsigned char);
   promote(int, signed char);
   promote(int, short);
   promote(int, unsigned short);
   promote(int, int);
   promote(int, unsigned);
   promote(int, long);
   promote(int, unsigned long);
#ifdef HOST_LLONG_C
   promote(int, long long);
   promote(int, unsigned long long);
#endif

   promote(unsigned, char);
   promote(unsigned, unsigned char);
   promote(unsigned, signed char);
   promote(unsigned, short);
   promote(unsigned, unsigned short);
   promote(unsigned, int);
   promote(unsigned, unsigned);
   promote(unsigned, long);
   promote(unsigned, unsigned long);
#ifdef HOST_LLONG_C
   promote(unsigned, long long);
   promote(unsigned, unsigned long long);
#endif

   promote(long, char);
   promote(long, unsigned char);
   promote(long, signed char);
   promote(long, short);
   promote(long, unsigned short);
   promote(long, int);
   promote(long, unsigned);
   promote(long, long);
   promote(long, unsigned long);
#ifdef HOST_LLONG_C
   promote(long, long long);
   promote(long, unsigned long long);
#endif

   promote(unsigned long, char);
   promote(unsigned long, unsigned char);
   promote(unsigned long, signed char);
   promote(unsigned long, short);
   promote(unsigned long, unsigned short);
   promote(unsigned long, int);
   promote(unsigned long, unsigned);
   promote(unsigned long, long);
   promote(unsigned long, unsigned long);
#ifdef HOST_LLONG_C
   promote(unsigned long, long long);
   promote(unsigned long, unsigned long long);
#endif

   for(m = 0; m < MAX6; m++) {
      for(i6 = min_max[m][0]; i6 <= min_max[m][1]; i6++) {
         for(j6 = min_max[m][2]; j6 <= min_max[m][3]; j6++) {
            ij6 = (HOST_INT6_T)i6 + (HOST_INT6_T)j6;
            if((i6 + j6) != ij6) {
               printf("FAKE8 failed '+',"
                 " i6= " HOST_LLONG_QP
                 " j6= " HOST_LLONG_QP
                 " j6= " HOST_LLONG_QP
                 "\n", i6, j6, ij6);
            }

            ij6 = (HOST_INT6_T)i6 - (HOST_INT6_T)j6;
            if((i6 - j6) != ij6) {
               printf("FAKE8 failed '-',"
                 " i6= " HOST_LLONG_QP
                 " j6= " HOST_LLONG_QP
                 " j6= " HOST_LLONG_QP
                 "\n", i6, j6, ij6);
            }

            if(i6 < LONG_MAX / (10 * RANGE) && i6 > LONG_MIN / (10 * RANGE) &&
               j6 < LONG_MAX / (10 * RANGE) && j6 > LONG_MIN / (10 * RANGE)) {
               ij6 = (HOST_INT6_T)i6 * (HOST_INT6_T)j6;
               if((i6 * j6) != ij6) {
                  printf("FAKE8 failed '*',"
                    " i6= " HOST_LLONG_QP
                    " j6= " HOST_LLONG_QP
                    " j6= " HOST_LLONG_QP
                    "\n", i6, j6, ij6);
               }
            }

            if(j6) {
               ij6 = HOST_INT6_DIV(i6, j6);
               if((i6 / j6) != ij6) {
                  printf("FAKE8 failed '/',"
                    " i6= " HOST_LLONG_QP
                    " j6= " HOST_LLONG_QP
                    " j6= " HOST_LLONG_QP
                    "\n", i6, j6, ij6);
               }

               ij6 = HOST_INT6_MOD(i6, j6);
               if((i6 % j6) != ij6) {
                  printf("FAKE8 failed '%%',"
                    " i6= " HOST_LLONG_QP
                    " j6= " HOST_LLONG_QP
                    " j6= " HOST_LLONG_QP
                    "\n", i6, j6, ij6);
               }
            }
         }
      }
   }

   return 0;
}
