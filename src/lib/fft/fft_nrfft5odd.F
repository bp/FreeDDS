c***********************************************************************
c                     B P   A M E R I C A
c             PROPRIETARY - TO BE MAINTAINED IN CONFIDENCE 
c                           COPYRIGHTED 2006
c***********************************************************************
c
c     Given the integer input, N, the function returns the integer
c     M such that M is the smallest number greater than or equeal to
c     N such that M = 3**J * 5**K  where J & K are non-negative integer. 
c     This differs from ffd_nrfft5 in that this is always returns an
c     odd number.
c
c     written:  May 1988 R.D. Coleman, QTC
c     modified: May 2002 J.W. Ehlers, BP
c     modified: Aug 2006 J.W. Ehlers, BP - Make an odd version
c
      integer function fft_nrfft5odd(n)
c
      implicit none
c
      integer  n
c
      integer  m,itmp,itest
c
c-----------------------------------------------------------------------
c
c     calculate it
c
      m=max(1,n)
      if (mod(m,2).eq.0) m=m+1
      itmp=m
      m=m-2
      do while(itmp.ne.1)
         m=m+2
         itmp=m
c     
         itest=itmp/3
         do while(3*itest.eq.itmp)
            itmp=itest
            itest=itmp/3
         enddo
c     
         itest=itmp/5
         do while(5*itest.eq.itmp)
            itmp=itest
            itest=itmp/5
         enddo
      enddo
c
      fft_nrfft5odd=m
c
      return
      end
