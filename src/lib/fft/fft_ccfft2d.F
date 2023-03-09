c***********************************************************************
c                     B P   A M E R I C A
c             PROPRIETARY - TO BE MAINTAINED IN CONFIDENCE 
c                           COPYRIGHTED 2008
c***********************************************************************
c
c     2-D complex-complex fft's
c
      integer function fft_ccfft2d(isign,scale,n1,n2,ld1,cdata,ioption)
c
#if defined(MKL) || defined(MKLI8)
      use MKL_DFTI
#endif
c
      implicit none
c
      integer isign,n1,ld1,n2,ioption
      real    scale
c
      integer ier
c
#if defined(MKL) || defined(MKLI8)
c
c     intel MKL routines --- used on all ia32 platforms
c
      complex cdata(ld1*n2)
#ifdef MKL
      integer dim,len(2),strides_in(3),status
#else
      integer*8 dim,len(2),strides_in(3),status
#endif
      type(DFTI_DESCRIPTOR), POINTER :: desc
c
      ier=0
      if (n1.lt.1.or.n2.lt.1.or.ld1.lt.n1.or.isign.eq.0) then
         fft_ccfft2d=-1
         return
      endif
c
      dim=2
      len(1)=n1
      len(2)=n2
      strides_in(1)=0
      strides_in(2)=1
      strides_in(3)=ld1
c
      status=DftiCreateDescriptor(desc,DFTI_SINGLE,DFTI_COMPLEX,dim,len)
      status=DftiSetValue(desc,DFTI_INPUT_STRIDES,strides_in)
c
      if (isign.lt.0) then
         status=DftiSetValue(desc,DFTI_FORWARD_SCALE,scale)
         status=DftiCommitDescriptor(desc)
         status=DftiComputeForward(desc,cdata)
      else
         status=DftiSetValue(desc,DFTI_BACKWARD_SCALE,scale)
         status=DftiCommitDescriptor(desc)
         status=DftiComputeBackward(desc,cdata)
      endif
      status=DftiFreeDescriptor(desc)
#else
#ifdef ppc64
c
c     IBM ESSL routine
c
      complex   cdata(ld1,n2)
      integer*8 isign_i8,n1_i8,n2_i8,inc1x,inc2x
      integer*8 naux1,naux2,init
      real*8    aux1(60000+2*(n1+n2)),aux2(20000+(256+max(n1,n2))*66)
c
      ier=0
      isign_i8=isign
      n1_i8=n1
      n2_i8=n2
      inc1x=1
      inc2x=ld1
      naux1=60000+2*(n1+n2)
      naux2=20000+(256+max(n1,n2))*66
c
      init=1
      call scft2(init,cdata,inc1x,inc2x,cdata,inc1x,inc2x,n1_i8,n2_i8,
     *           -isign_i8,scale,aux1,naux1,aux2,naux2)
c
      init=0
      call scft2(init,cdata,inc1x,inc2x,cdata,inc1x,inc2x,n1_i8,n2_i8,
     *           -isign_i8,scale,aux1,naux1,aux2,naux2)
#else
#ifdef SCSL
c
c     SGI scsl routines on IRIX
c
      complex cdata(ld1,n2)
      integer n,isys(0:1)
      real    table(2*(n1+n2+256)),work(2*max(n1,n2))
c
      if (n1.lt.1.or.n2.lt.1.or.ld1.lt.n1.or.isign.eq.0) then
         fft_ccfft2d=-1
         return
      endif
c
      ier=0
      isys(0)=1
      call ccfft2d(    0,n1,n2,scale,cdata,ld1,cdata,ld1,table,work,
     *     isys)
      call ccfft2d(isign,n1,n2,scale,cdata,ld1,cdata,ld1,table,work,
     *     isys)
#else
c
c     default to fftw routines
c
#include <fftw3.f>
c
      integer FFT_ESTIMATE,FFT_MEASURE
      parameter(FFT_ESTIMATE=1,FFT_MEASURE=2)
c
      complex   cdata(ld1,n2)
      complex   ctmp(n1,n2)
      integer   i1,i2,flags
      integer*8 plan
c
      if (n1.lt.1.or.n2.lt.1.or.ld1.lt.n1.or.isign.eq.0) then
         fft_ccfft2d=-1
         return
      endif
c
      ier=0
      if (iand(ioption,FFT_MEASURE).ne.0) then
         flags=FFTW_MEASURE
      else
         flags=FFTW_ESTIMATE
      endif
c
      if (ld1.eq.n1) then
         call sfftw_plan_dft_2d(plan,n1,n2,cdata,cdata,isign,flags)
c
         if (scale.ne.1.0) then
            do i2=1,n2
               do i1=1,n1
                  cdata(i1,i2)=scale*cdata(i1,i2)
               enddo
            enddo
         endif
      else
         do i2=1,n2
            do i1=1,n1
               ctmp(i1,i2)=cdata(i1,i2)
            enddo
         enddo
c
         call sfftw_plan_dft_2d(plan,n1,n2,ctmp,ctmp,isign,flags)
c
         do i2=1,n2
            do i1=1,n1
               cdata(i1,i2)=scale*ctmp(i1,i2)
            enddo
         enddo
      endif
c
      call sfftw_execute(plan)
      call sfftw_destroy_plan(plan)
c
#endif
#endif
#endif
      fft_ccfft2d=ier
      return
      end
