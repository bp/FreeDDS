c 
c RCS-t=DDS main, test parameter dictionary simplest functionality.

c $Id: ftest1.f 16 2009-01-06 21:52:47Z user $

      program ftest1
      implicit none
      include 'fdds.h'

      character*64 rcsid

      integer rc
      character*80 value

      data rcsid
     :/'$Id: ftest1.f 16 2009-01-06 21:52:47Z user $'/

      rc = fdds_init()
 
      rc = fdds_dict('stdin:', 'scan')
      rc = fdds_scanv('hello', value)
      rc = fdds_dict('stdout:', 'print')
      rc = fdds_printv('wallo', ' hollow\n')
      rc = fdds_dict('stdout:', 'reset')
      rc = fdds_dict('missing', 'scan')

      write(0,10)
   10 format('tab=\t(tab) newline=\n(newline)')

      stop
      end
