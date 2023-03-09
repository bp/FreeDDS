c RCS-t=GIO, tuneup verify read and write
c

      integer function frverify(buf, ubuf, lloffset, ibytes,
     :   init_first, init_words, n_shift, scale)
      implicit none
      integer buf(*), ubuf(*), lloffset(*)
      integer ibytes, init_first, init_words, n_shift, scale
      integer i
      character*66 rcsid
      data rcsid
     :/'$Id: fverify.f 13 2009-01-06 16:56:43Z user $'/

      do i = 1, init_words
         ubuf(i) = init_first + (i - 1) * scale
      enddo

c     verify only the number of whole words
      do i = 1, init_words - 2
         if(buf(i) .ne. ubuf(i)) then
            call verify(buf, ubuf, lloffset, ibytes)
            frverify = 27
            return
         endif
      enddo

      frverify = 0
      return
      end


      integer function fwverify(buf,
     :   init_first, init_words, n_shift, scale)
      implicit none
      integer buf(*)
      integer init_first, init_words, n_shift, scale
      integer i

      do i = 1, init_words
         buf(i) = init_first + (i - 1) * scale
      enddo

      fwverify = 0
      return
      end
