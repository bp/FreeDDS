c RCS-t=DDS main, test escape sequences withing Fortran strings

c $Id: ftest1a.f 16 2009-01-06 21:52:47Z user $

      program ftest1
      implicit none

      character*64 rcsid
      data rcsid
     :/'$Id: ftest1a.f 16 2009-01-06 21:52:47Z user $'/

      write(0,10)
   10 format('tab=\t(tab) newline=\n(newline)')

      stop
      end
