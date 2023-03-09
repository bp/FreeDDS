c***********************************************************************
c                     B P   A M E R I C A
c             PROPRIETARY - TO BE MAINTAINED IN CONFIDENCE 
c                           COPYRIGHTED 2008
c***********************************************************************
c
c     Multiple 1-D complex-complex fft's
c
      integer function fft_ccfftm(isign,scale,n1,m,ld1,cdata,ioption)
c
#if defined(MKL) || defined(MKLI8)
      use MKL_DFTI
#endif
      implicit none
c
      integer isign,n1,m,ld1,ioption
      real    scale
c
      integer ier

#if defined(MKL) || defined(MKLI8)
c
c     intel MKL routines --- used on all ia32,x86_64,ia64 platforms
c
      complex cdata(ld1*m)
#ifdef MKL
      integer dim,len,num,dist_in,strides_in(2),status
#else
      integer*8 dim,len,num,dist_in,strides_in(2),status
#endif
      type(DFTI_DESCRIPTOR), POINTER :: desc
c
      ier=0
      if (n1.lt.1.or.m.lt.1.or.ld1.lt.n1.or.isign.eq.0) then
         fft_ccfftm=-1
         return
      endif
c
      dim=1
      len=n1
      num=m
      dist_in=ld1
      strides_in(1)=0
      strides_in(2)=1

      status=DftiCreateDescriptor(desc,DFTI_SINGLE,DFTI_COMPLEX,dim,len)
      status=DftiSetValue(desc,DFTI_NUMBER_OF_TRANSFORMS,num)
      status=DftiSetValue(desc,DFTI_INPUT_DISTANCE,dist_in)
      status=DftiSetValue(desc,DFTI_INPUT_STRIDES,strides_in)
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
#ifdef SCSL  
c
c     SGI scsl routines on IRIX
c
      complex cdata(ld1,m)

      integer isys(0:1)
      real    table(2*n1+256),work(2*n1)

      if (n1.lt.1.or.m.lt.1.or.ld1.lt.n1.or.isign.eq.0) then
         fft_ccfftm=-1
         return
      endif
      ier=0
      isys(0)=1
      call ccfftm(    0,n1,m,scale,cdata,ld1,cdata,ld1,table,work,isys)
      call ccfftm(isign,n1,m,scale,cdata,ld1,cdata,ld1,table,work,isys)
#else
#ifdef ppc64
c
c     IBM ESSL routine
c
      complex   cdata(ld1,m)
      integer*8 isign_i8,n1_i8,m_i8,ld1_i8
      integer*8 naux1,naux2,init,inc
      real*8    aux1(30000+2*n1),aux2(20000+2*n1)
c
      ier=0
      isign_i8=isign
      n1_i8=n1
      m_i8=m
      ld1_i8=ld1
      naux1=30000+2*n1
      naux2=20000+2*n1
      init=1
c
      inc=1
      call scft(init,cdata,inc,ld1_i8,cdata,inc,ld1_i8,n1_i8,m_i8,
     *          -isign_i8,scale,aux1,naux1,aux2,naux2)
c
      init=0
      call scft(init,cdata,inc,ld1_i8,cdata,inc,ld1_i8,n1_i8,m_i8,
     *          -isign_i8,scale,aux1,naux1,aux2,naux2)
#else
c
c     default to fftw routines
c
#include <fftw3.f>
c
      integer FFT_ESTIMATE,FFT_MEASURE
      parameter(FFT_ESTIMATE=1,FFT_MEASURE=2)
c
      complex   cdata(ld1,m)
      integer   i,i1,flags
      integer*8 plan
c
      if (n1.lt.1.or.m.lt.1.or.ld1.lt.n1.or.isign.eq.0) then
         fft_ccfftm=-1
         return
      endif
c
      ier=0
      if (iand(ioption,FFT_MEASURE).ne.0) then
         flags=FFTW_MEASURE
      else
         flags=FFTW_ESTIMATE
      endif
      call sfftw_plan_many_dft(plan,1,n1,m,cdata,n1,1,ld1,
     *     cdata,n1,1,ld1,isign,flags)
c
      call sfftw_execute(plan)
      call sfftw_destroy_plan(plan)
c
      if (scale.ne.1.0) then
         do i=1,m
            do i1=1,n1
               cdata(i1,i)=scale*cdata(i1,i)
            enddo
         enddo
      endif
c
#endif
#endif
#endif
      fft_ccfftm=ier
      return
      end
