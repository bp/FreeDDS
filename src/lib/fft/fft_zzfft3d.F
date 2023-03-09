c***********************************************************************
c                     B P   A M E R I C A
c             PROPRIETARY - TO BE MAINTAINED IN CONFIDENCE 
c                           COPYRIGHTED 2008
c***********************************************************************
c
c     3-D complex16-complex16 fft's
c
      integer function fft_zzfft3d(isign,scale,n1,n2,n3,ld1,ld2,cdata,
     *     ioption)
c
#if defined(MKL) || defined(MKLI8)
      use MKL_DFTI
#endif
c
      implicit none
c
      integer isign,n1,n2,n3,ld1,ld2,ioption
      real*8  scale
c
      integer ier

#if defined(MKL) || defined(MKLI8)
c
c     intel MKL routines --- used on all ia32,x86_64,ia64 platforms
c
      complex cdata(ld1*ld2*n3)
#ifdef MKL
      integer dim,len(3),strides_in(4),status
#else
      integer*8 dim,len(3),strides_in(4),status
#endif
      type(DFTI_DESCRIPTOR), POINTER :: desc
c
      ier=0
      if (n1.lt.1.or.n2.lt.1.or.n3.lt.1.or.ld1.lt.n1.or.ld2.lt.n2
     *     .or.isign.eq.0) then
         fft_zzfft3d=-1
         return
      endif
c
      dim=3
      len(1)=n1
      len(2)=n2
      len(3)=n3
      strides_in(1)=0
      strides_in(2)=1
      strides_in(3)=ld1
      strides_in(4)=strides_in(3)*ld2
c
      status=DftiCreateDescriptor(desc,DFTI_DOUBLE,DFTI_COMPLEX,dim,len)
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
#ifdef SCSL  
c
c     SGI scsl routines on IRIX
c
      complex*16 cdata(ld1,ld2,n3)

      integer isys(0:1)
      real*8  table(2*(n1+n2+n3)+3*256),work(2*max(n1,n2,n3))
c
      if (n1.lt.1.or.n2.lt.1.or.n3.lt.1.or.ld1.lt.n1.or.ld2.lt.n2
     *     .or.isign.eq.0) then
         fft_zzfft3d=-1
         return
      endif
c
      ier=0
      isys(0)=1
      call zzfft3d(    0,n1,n2,n3,scale,cdata,ld1,ld2,cdata,ld1,ld2,
     *     table,work,isys)
      call zzfft3d(isign,n1,n2,n3,scale,cdata,ld1,ld2,cdata,ld1,ld2,
     *     table,work,isys)

#else
#ifdef ppc64
c
c     IBM ESSL routine
c
      complex*16 cdata(ld1,ld2,n3)
      integer*8  isign_i8,n1_i8,n2_i8,n3_i8,ld1_i8,ld2_i8
      integer*8  naux
      real*8     aux(90000+5*n1+((2*max(n2,n3)+256)*69))
c
      ier=0
      isign_i8=isign
      n1_i8=n1
      n2_i8=n2
      n3_i8=n3
      ld1_i8=ld1
      ld2_i8=ld2
      naux=90000+5*n1+((2*max(n2,n3)+256)*69)
c
      call dcft3(cdata,ld1_i8,ld1_i8*ld2_i8,cdata,ld1_i8,ld1_i8*ld2_i8,
     *     n1_i8,n2_i8,n3_i8,-isign_i8,scale,aux,naux)
#else
c
c     default to fftw routines
c
#include <fftw3.f>
c
      integer FFT_ESTIMATE,FFT_MEASURE
      parameter(FFT_ESTIMATE=1,FFT_MEASURE=2)
c
      complex*16 cdata(ld1,ld2,n3)
      complex*16 ctmp(n1,n2,n3)
      integer   i1,i2,i3,flags
      integer*8 plan
c
      if (n1.lt.1.or.n2.lt.1.or.n3.lt.1.or.ld1.lt.n1.or.ld2.lt.n2
     *     .or.isign.eq.0) then
         fft_zzfft3d=-1
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
      if (ld1.eq.n1.and.ld2.eq.n2) then
         call dfftw_plan_dft_3d(plan,n1,n2,n3,cdata,cdata,isign,flags)
c        
         if (scale.ne.1.0) then
            do i3=1,n3
               do i2=1,n2
                  do i1=1,n1
                     cdata(i1,i2,i3)=scale*cdata(i1,i2,i3)
                  enddo
               enddo
            enddo
         endif
      else
         do i3=1,n3
            do i2=1,n2
               do i1=1,n1
                  ctmp(i1,i2,i3)=cdata(i1,i2,i3)
               enddo
            enddo
         enddo
c        
         call dfftw_plan_dft_3d(plan,n1,n2,n3,ctmp,ctmp,isign,flags)
c        
         do i3=1,n3
            do i2=1,n2
               do i1=1,n1
                  cdata(i1,i2,i3)=scale*ctmp(i1,i2,i3)
               enddo
            enddo
         enddo
      endif
c     
      call dfftw_execute(plan)
      call dfftw_destroy_plan(plan)
c
#endif
#endif
#endif
      fft_zzfft3d=ier
      return
      end
