c***********************************************************************
c                     B P   A M E R I C A
c             PROPRIETARY - TO BE MAINTAINED IN CONFIDENCE 
c                           COPYRIGHTED 2008
c***********************************************************************
c
c     Multiple 1-D complex-real fft's
c
c     Yk = SUMj Xj exp(2*pi*(+i)*j*k/n)
c
      integer function fft_crfftm(scale,n1,m,ld1,data,ioption)
c
#if defined(MKL) || defined(MKLI8)
      use MKL_DFTI
#endif
c
      implicit none
c
      integer n1,m,ld1,ioption
      real    scale
c
      integer ier,i1max
c
#if defined(MKL) || defined(MKLI8)
c
c     intel MKL routines --- used on all ia32,x86_64,ia64 platforms
c
      real    data(ld1*m)
#ifdef MKL
      integer dim,len,num,dist_in,strides_in(2),status
#else
      integer*8 dim,len,num,dist_in,strides_in(2),status
#endif
      type(DFTI_DESCRIPTOR), POINTER :: desc
c
      ier=0
      i1max=n1+2-mod(n1,2)
      if (n1.lt.1.or.m.lt.1.or.ld1.lt.i1max) then
         fft_crfftm=-1
         return
      endif
c
      dim=1
      len=n1
      num=m
      dist_in=ld1
      strides_in(1)=0
      strides_in(2)=1
c
      status=DftiCreateDescriptor(desc,DFTI_SINGLE,DFTI_REAL,dim,len)
      status=DftiSetValue(desc,DFTI_NUMBER_OF_TRANSFORMS,num)
      status=DftiSetValue(desc,DFTI_INPUT_DISTANCE,dist_in)
      status=DftiSetValue(desc,DFTI_INPUT_STRIDES,strides_in)
c
      status=DftiSetValue(desc,DFTI_BACKWARD_SCALE,scale)
      status=DftiCommitDescriptor(desc)
      status=DftiComputeBackward(desc,data)
      status=DftiFreeDescriptor(desc)
#else
#ifdef SCSL  
c
c     SGI scsl routines on IRIX
c
      real    data(ld1,m)

      integer isys(0:1)
      real    table(n1+256),work(n1+2)
c
      ier=0
      i1max=n1+2-mod(n1,2)
      if (n1.lt.1.or.m.lt.1.or.ld1.lt.i1max) then
         fft_crfftm=-1
         return
      endif
c
      isys(0)=1
      call csfftm(0,n1,m,scale,data,ld1/2,data,ld1,table,work,isys)
      call csfftm(1,n1,m,scale,data,ld1/2,data,ld1,table,work,isys)

#else
#ifdef ppc64
c
c     IBM ESSL routine
c
      real      data(ld1,m)
      integer*8 n1_i8,m_i8,ld1_i8,isign_i8
      integer*8 naux1,naux2,naux3,init
      real*8    aux1(max(35000,30000+n1)),aux2(20000+n1),aux3(1)
c
      ier=0
      i1max=n1+2-mod(n1,2)
      if (n1.lt.1.or.m.lt.1.or.ld1.lt.i1max) then
         fft_crfftm=-1
         return
      endif
c
      n1_i8=n1
      m_i8=m
      ld1_i8=ld1
      isign_i8=-1
      naux1=max(35000,30000+n1)
      naux2=20000+n1
      naux3=1
c
      init=1
      call scrft(init,data,ld1_i8/2,data,ld1_i8,n1_i8,m_i8,
     *     isign_i8,scale,aux1,naux1,aux2,naux2,aux3,naux3)
c
      init=0
      call scrft(init,data,ld1_i8/2,data,ld1_i8,n1_i8,m_i8,
     *     isign_i8,scale,aux1,naux1,aux2,naux2,aux3,naux3)
#else
c
c     default to fftw routines
c
#include <fftw3.f>
c
      integer FFT_ESTIMATE,FFT_MEASURE
      parameter(FFT_ESTIMATE=1,FFT_MEASURE=2)
c
      real      data(ld1,m)
      integer   i,i1,flags
      integer*8 plan
c
      ier=0
      i1max=n1+2-mod(n1,2)
      if (n1.lt.1.or.m.lt.1.or.ld1.lt.i1max) then
         fft_crfftm=-1
         return
      endif
c
      if (iand(ioption,FFT_MEASURE).ne.0) then
         flags=FFTW_MEASURE
      else
         flags=FFTW_ESTIMATE
      endif
      call sfftw_plan_many_dft_c2r(plan,1,n1,m,data,n1,1,ld1/2,
     *     data,n1,1,ld1,flags)
c
      call sfftw_execute(plan)
      call sfftw_destroy_plan(plan)
c
      if (scale.ne.1.0) then
         do i=1,m
            do i1=1,n1
               data(i1,i)=scale*data(i1,i)
            enddo
         enddo
      endif
c
#endif
#endif
#endif
      fft_crfftm=ier
      return
      end
