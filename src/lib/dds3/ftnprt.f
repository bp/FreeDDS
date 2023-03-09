c Fortran print support
c RCS-t=DDS, Print one line, using Fortran write

c $Id: ftnprt.f 16 2009-01-06 21:52:47Z user $

      subroutine ddsftnprt(lu, msg)
      implicit none
      integer lu
      character*(*) msg
      write(lu,10) msg
   10 format(a)
      return
      end
