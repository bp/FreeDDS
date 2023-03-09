c***********************************************************************
c                     B P   A M E R I C A
c             PROPRIETARY - TO BE MAINTAINED IN CONFIDENCE 
c                           COPYRIGHTED 2008
c***********************************************************************
c
c     2-D real-complex fft's
c
      integer function fft_rcfft2d(scale,n1,n2,ld1,data,ioption)
c
#if defined(MKL) || defined(MKLI8)
      use MKL_DFTI
#endif
c
      implicit none
c
      integer n1,ld1,n2,ioption
      real    scale
c
      integer ier,i1max
c
#if defined(MKL) || defined(MKLI8)
c
c     intel MKL routines --- used on ia32 platforms
c
      real data(ld1*n2)
#ifdef MKL
      integer dim,len(2),strides_in(3),strides_out(3),status
#else
      integer*8 dim,len(2),strides_in(3),strides_out(3),status
#endif
      type(DFTI_DESCRIPTOR), POINTER :: desc
c
      ier=0
      i1max=n1+2-mod(n1,2)
      if (n1.lt.1.or.n2.lt.1.or.ld1.lt.i1max) then
         fft_rcfft2d=-1
         return
      endif
c
      dim=2
      len(1)=n1
      len(2)=n2
      strides_in(1)=0
      strides_in(2)=1
      strides_in(3)=ld1
      strides_out(1)=0
      strides_out(2)=1
      strides_out(3)=ld1/2
c
      status=DftiCreateDescriptor(desc,DFTI_SINGLE,DFTI_REAL,dim,len)
      status=DftiSetValue(desc,DFTI_CONJUGATE_EVEN_STORAGE,
     *     DFTI_COMPLEX_COMPLEX)
      status=DftiSetValue(desc,DFTI_INPUT_STRIDES,strides_in)
      status=DftiSetValue(desc,DFTI_OUTPUT_STRIDES,strides_out)
      status=DftiSetValue(desc,DFTI_FORWARD_SCALE,scale)
      status=DftiCommitDescriptor(desc)
      status=DftiComputeForward(desc,data)
      status=DftiFreeDescriptor(desc)
#else
#ifdef ppc64
c
c     IBM ESSL routine
c
      real      data(ld1,n2)
      integer*8 isign_i8,n1_i8,n2_i8,inc1x,inc2x
      integer*8 naux1,naux2,naux3,init
      real*8    aux1(60000+(n1+2*n2))
      real*8    aux2(20000+n1+66*(n2+256))
      real*8    aux3(1)
c
      ier=0
      i1max=n1+2-mod(n1,2)
      if (n1.lt.1.or.n2.lt.1.or.ld1.lt.i1max) then
         fft_rcfft2d=-1
         return
      endif
c
      isign_i8=1
      n1_i8=n1
      n2_i8=n2
      inc2x=ld1
      naux1=60000+(n1+2*n2)
      naux2=20000+n1+66*(n2+256)
      naux3=1
c
      init=1
      call srcft2(init,data,inc2x,data,inc2x/2,n1_i8,n2_i8,
     *     isign_i8,scale,aux1,naux1,aux2,naux2,aux3,naux3)
c
      init=0
      call srcft2(init,data,inc2x,data,inc2x/2,n1_i8,n2_i8,
     *     isign_i8,scale,aux1,naux1,aux2,naux2,aux3,naux3)
#else
#ifdef SCSL
c
c     SGI scsl routines on IRIX
c
      real data(ld1,n2)
      integer n,isys(0:1)
      real    table((n1+256++2*n2+256)),work(n1+4*n2)
c
      ier=0
      i1max=n1+2-mod(n1,2)
      if (n1.lt.1.or.n2.lt.1.or.ld1.lt.i1max) then
         fft_rcfft2d=-1
         return
      endif
c
      isys(0)=1
      call scfft2d( 0,n1,n2,scale,data,ld1,data,ld1/2,table,work,
     *     isys)
      call scfft2d(-1,n1,n2,scale,data,ld1,data,ld1/2,table,work,
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
      real      data(ld1,n2)
      real      tmp(n1+2,n2)
      integer   i1,i2,flags
      integer*8 plan
c
      ier=0
      i1max=n1+2-mod(n1,2)
      if (n1.lt.1.or.n2.lt.1.or.ld1.lt.i1max) then
         fft_rcfft2d=-1
         return
      endif
c
      if (iand(ioption,FFT_MEASURE).ne.0) then
         flags=FFTW_MEASURE
      else
         flags=FFTW_ESTIMATE
      endif
      call sfftw_plan_dft_r2c_2d(plan,n1,n2,tmp,tmp,flags)
c
      do i2=1,n2
         do i1=1,n1
            tmp(i1,i2)=data(i1,i2)
         enddo
      enddo
c
      call sfftw_execute(plan)
      call sfftw_destroy_plan(plan)
c
      do i2=1,n2
         do i1=1,i1max
            data(i1,i2)=scale*tmp(i1,i2)
         enddo
      enddo
c
#endif
#endif
#endif
      fft_rcfft2d=ier
      return
      end
