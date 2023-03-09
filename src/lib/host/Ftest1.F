c!< main, test basic make functionality

      program Ftest1
c
      implicit none
c
#include "ftest1.h"
c
      integer lnblnk,lsize
      logical flag(2)
      character rcsid*64
      data rcsid /
     *'$Id: Ftest1.F 50 2009-02-10 21:29:01Z user $'/
c
      write(0,*) '(',rcsid,')'
      write(0,*) '(',rcsid(1:lnblnk(rcsid)),')'
      call host_linkf(2)
      write(0,100) test_inc
  100 format('Ftest1: hello world, ',A)
      write(0,200) lsize(flag(1),flag(2))
  200 format('Ftest1: sizeof(logical)="',I2,'"')
      stop
      end
