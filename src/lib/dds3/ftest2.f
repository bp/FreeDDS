c RCS-t=DDS main, test parameter dictionary functionality.

c $Id: ftest2.f 16 2009-01-06 21:52:47Z user $

      subroutine foobar(name, note)
      implicit none
      include 'fdds.h'

      character*(*) name, note

      character rcsid*64

c retrieve and validate definitions

      integer i, j, n0, n1, n2, n3, n4, n5, nzero, neof
      double precision f
      character s1*16, s2*16, s3*16, s4*16
      character value*80

      data rcsid
     :/'$Id: ftest2.f 16 2009-01-06 21:52:47Z user $'/

      i = -123
      j = -123
      n0 = -123
      n1 = -123
      n2 = -123
      n3 = -123
      n4 = -123
      n5 = -123
      nzero = -123
      neof = -123
      f = -123.456
      s1 = 'default'
      s2 = 'default'
      s3 = 'default'
      s4 = 'default'
      value = 'default'

      write(0,100) name, note
  100 format('Testing dictionary= ',A,' (',A,')')
 
      j = fdds_scanf('foo', '%8s %i %lf %8s %8s %8s\0',
     :   s1, i, f, s2, s3, s4)
      if((5 .NE. j) .or. (s1 .NE. 'aaa') .or. (11 .NE. i) .or.
     :   (22.21 .GT. f) .or. (22.23 .LT. f) .or.
     :   (s2 .NE. 'bbb') .or. (s3 .NE. 'ccc')) then
         write(0,101) j, s1, i, f, s2, s3, s4
  101 format('\tFail:scanf combo, j= ',I5,', s1= ',A,', i= ',I5,
     :   ', f= ',F8.2,', s2= ',A,', s3= ',A,', s4= ',A)
      endif

      j = fdds_scanf('bar', ' ')
      if(0 .NE. j)
     :   write(0,102) j
  102 format('\tFail:scanf select, j= ',I5)
 
      neof = fdds_scanf(' ', '%i\0', i)
      j = fdds_scanf(' ', '%s\0', s1)
      if((-1 .NE. neof) .or. (1 .NE. j) .or. (s1 .NE. 'ddd'))
     :   write(0,103) neof, j, s1
  103 format('\tFail:scanf string, neof= ',I5,', j= ',I5,', s1= ',A)
 
      neof = fdds_scanf(' ', '  %[a-zA-Z]\0', s1)
      j = fdds_scanf(' ', '%lf\0', f)
      if((-1 .NE. neof) .or. (1 .NE. j) .or.
     :   (33.32 .GT. f) .or. (33.34 .LT. f))
     :   write(0,104) neof, j, f
  104 format('\tFail:scanf dbl prec, neof= ',I5,', j= ',I5,', f= ',F8.2)
 
      neof = fdds_scanf(' ', '  %[a-zA-Z]\0', s1)
      j = fdds_scanf(' ', '%i\0', i)
      if((-1 .NE. neof) .or. (1 .NE. j) .or. (44 .NE. i))
     :   write(0,105) neof, j, i
  105 format('\tFail:scanf int, neof= ',I5,', j= ',I5,', i= ',I5)
 
      j = fdds_scanf('bar', '  %[a-z]  %[0-9.]  %[0-9]\0', s1, s2, s3)
      if((3 .NE. j) .or. (s1 .NE. 'ddd') .or.
     :   (s2 .NE. '33.330000') .or. (s3 .NE. '44'))
     :   write(0,106) j, s1, s2, s3
  106 format('\tFail:scanf sets, j= ',I5,', s1= ',A,
     :   ', s2= ',A,', s3= ',A)

      j = fdds_scanv('foo', value)
      neof = fdds_scanv(' ', value)
      if((26 .NE. j) .or. (value .NE. ' aaa 11 22.220000 bbb ccc\n')
     :   .or. (-1 .NE. neof))
     :   write(0,107) j, value, neof
  107 format('\tFail:scanf value, j= ',I5,', value= ',A,
     :   '\n, neof= ',I5,'\0')

      i = fdds_scank('bar', ' ')
      j = fdds_scank(' ', '44')
      nzero = fdds_scank(' ', 'ABSENT')
      if((4 .NE. i) .or. (3 .NE. j) .or. (0 .NE. nzero))
     :   write(0,108) i, j, nzero
  108 format('\tFail:scanf key count, i= ',I5,', j= ',I5,', nzero= ',I5)
 
      j = fdds_scanf('chain', '%8s %i %lf %8s %8s %8s\0',
     :   s1, i, f, s2, s3, s4)
      if((5 .NE. j) .or. (s1 .NE. 'aaa') .or. (11 .NE. i) .or.
     :   (22.21 .GT. f) .or. (22.23 .LT. f) .or.
     :   (s2 .NE. 'bbb') .or. (s3 .NE. 'ccc')) then
         write(0,109) j, s1, i, f, s2, s3, s4
  109 format('\tFail:scanf combo, j= ',I5,', s1= ',A,', i= ',I5,
     :   ', f= ',F8.2,', s2= ',A,', s3= ',A,', s4= ',A)
      endif

      j = fdds_scanf('$chain', '%8s\0', s1)
      if((1 .NE. j) .or. (s1 .NE. 'foo')) then
         write(0,110) j, s1
  110 format('\tFail:$chain, j= ',I5,', ',A)
      endif

      j = fdds_scanf('color', ' ')
      n0 = fdds_key('red')
      n1 = fdds_key(' ')
      neof = fdds_key('clear')
      n2 = fdds_key('GREEN green Green')
      n3 = fdds_scant(' ', value)
      n4 = fdds_key('blue')
      if((1 .NE. n0) .OR. (3 .NE. n1) .OR. (-1 .NE. neof) .OR.
     :   (2 .NE. n2) .OR. (5 .NE. n3) .OR.
     :   (value .NE. 'green') .OR. (3 .NE. n4)) then
         write(0,111) j, n0, n1, n2, n3, n4, neof, value
  111 format('\tFail: keys, j= ',I5,', n0= ',I5,', n1= ',I5,', n2= ',I5,
     :   ', n3= ',I5,', n4= ',I5,', neof= ',I5,', value= ',A)
      endif

      j = fdds_scanf('UnDeFiNeD', '%c\0', s1)
      if(-1 .NE. j) then
         write(0,112) j
  112 format('\tFail: UnDeFiNeD, j= ',I5)
      endif

      j = fdds_scanv('axis', value)
      if((j .NE. 11) .OR. (value .NE. '  t xx yyy\n')) then
         write(0,113) j, value
  113 format('\tFail: axis, j= ',I5,', value= ',A)
      endif

      j = fdds_scanf('size.axis(2)', '%i\0', n1)
      if((j .NE. 1) .OR. (22 .NE. n1)) then
         write(0,114) j, n1
  114 format('\tFail: axis, j= ',I5,', n1= ',I5)
      endif
 
      return
      end

      program ftest2
      implicit none
      include 'fdds.h'

c ftest2H contents
c foo= xyz
c bar= pqr
c axis  =  t xx yyy
c foo= aaa 11 22.220000 bbb ccc
c size.xx=  22
c bar= ddd 33.330000 44
c eee
c $chain= foo
c color= red green blue

      integer rc
      double precision f
c     character s*9
      character name*80
      character*(PATHNAME_MAX) in_dict, out_dict
 
      rc = fdds_init()
 
      rc = fdds_dict(' ', 'print')
      rc = fdds_dict(' ', 'scan')

      rc = fdds_in(in_dict, 'in', 'stdin:', 'ftest2')
      rc = fdds_out(out_dict, 'out', 'stdout:', in_dict)

c  open dict, and append definitions
c     remove('ftest2H')
c     remove('ftest2aH')
      name = 'ftest2H'
      rc = fdds_dict(name, 'print')
 
      rc = fdds_printf('foo', ' xyz\n\0')
      rc = fdds_printf('bar', ' pqr\n\0')

      rc = fdds_printf('axis  ', '  t xx yyy\n\0')
 
      f = 22.22
      rc = fdds_printf('foo', ' aaa %i %lf %s ccc\n\0', 11, f, 'bbb')
      rc = fdds_printv('size.axis(2)', '22\n\0')
      rc = fdds_printf('bar', '\0')
      f = 33.33
      rc = fdds_printf(' ', ' %s %lf %i\n\0', 'ddd', f, 44)
      rc = fdds_printf(' ', 'eee\n\0')
      rc = fdds_printf('$chain', ' foo\n\0')
      rc = fdds_printf('color', ' red green blue\n\0')
 
      rc = fdds_dict(name, 'scan')
      call foobar(name, 'original dict')
 
      name = 'ftest2aH'
      rc = fdds_dict(name, 'print')
      rc = fdds_history()
      rc = fdds_dict(name, 'scan')
      call foobar(name, 'copy history')
      
      rc = fdds_dict(name, 'print')
      rc = fdds_dict('cmd:', 'scan')
      rc = fdds_history()
      rc = fdds_dict('env: ftest2H cmd:', 'scan')
      rc = fdds_copy('USER')
      rc = fdds_copy('GaRbAgE')
      rc = fdds_dict(name, 'scan')
      call foobar(name, 'copy history + cmd: + misc')
      
c     name = 'stdout:'
c     rc = fdds_dict(name, 'print')
c     rc = fdds_dict('ftest2H cmd:', 'scan')
c     rc = fdds_history()
c     rc = fdds_dict(name, 'scan')
c     call foobar(name, 'copy history + cmd:')

      rc = fdds_dict('cmd: ftest2H ftest2aH env: stdout:', 'reset')

      name = 'ftest2H'
      rc = fdds_dict(name, 'scan')
      call foobar(name, 'close & reopen')
 
      stop
      end
