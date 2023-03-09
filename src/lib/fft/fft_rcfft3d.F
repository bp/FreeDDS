c***********************************************************************
c                     B P   A M E R I C A
c             PROPRIETARY - TO BE MAINTAINED IN CONFIDENCE 
c                           COPYRIGHTED 2008
c***********************************************************************
c
c     3-D complex-complex fft's
c
      integer function fft_rcfft3d(scale,n1,n2,n3,ld1,ld2,data,
     *     ioption)
c
#if defined(MKL) || defined(MKLI8)
      use MKL_DFTI
#endif
c
      implicit none
c
      integer n1,n2,n3,ld1,ld2,ioption
      real    scale
c
      integer ier,i1max
c
#if defined(MKL) || defined(MKLI8)
c
c     intel MKL routines --- used on ia32 platforms
c
      real data(ld1*ld2*n3)
#ifdef MKL
      integer dim,len(3),strides_in(4),strides_out(4),status
#else
      integer*8 dim,len(3),strides_in(4),strides_out(4),status
#endif
      type(DFTI_DESCRIPTOR), POINTER :: desc
c
      ier=0
      i1max=n1+2-mod(n1,2)
      if (n1.lt.1.or.n2.lt.1.or.n3.lt.1.or.ld2.lt.n2.or.
     *    ld1.lt.i1max) then
         fft_rcfft3d=-1
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
      strides_out(1)=0
      strides_out(2)=1
      strides_out(3)=ld1/2
      strides_out(4)=strides_out(3)*ld2
c
      status=DftiCreateDescriptor(desc,DFTI_SINGLE,DFTI_REAL,dim,len)
      status=DftiSetValue(desc,DFTI_CONJUGATE_EVEN_STORAGE,
     *     DFTI_COMPLEX_COMPLEX)
c
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
      real      data(ld1,ld2,n3)
      integer*8 isign_i8,n1_i8,n2_i8,n3_i8,ld1_i8,ld2_i8
      integer*8 naux
      real*8    aux(90000+2*n1+((max(n2,n3)+256)*67))
c
      ier=0
      i1max=n1+2-mod(n1,2)
      if (n1.lt.1.or.n2.lt.1.or.n3.lt.1.or.ld2.lt.n2.or.
     *    ld1.lt.i1max) then
         fft_rcfft3d=-1
         return
      endif
c
      isign_i8=1
      n1_i8=n1
      n2_i8=n2
      n3_i8=n3
      ld1_i8=ld1
      ld2_i8=ld2
      naux=90000+2*n1+((max(n2,n3)+256)*67)
c
      call srcft3(data,ld1_i8,ld1_i8*ld2_i8,data,ld1_i8/2,
     *     (ld1_i8/2)*ld2_i8,n1_i8,n2_i8,n3_i8,isign_i8,scale,aux,naux)
#else
#ifdef SCSL
c
c     SGI scsl routines on IRIX
c
      real data(ld1,ld2,n3)
      integer isys(0:1)
      real    table((n1+2*n2+2*n3)+3*256),work(n1+4*n3)
c
      i1max=n1+2-mod(n1,2)
      if (n1.lt.1.or.n2.lt.1.or.n3.lt.1.or.ld2.lt.n2.or.
     *    ld1.lt.i1max) then
         fft_rcfft3d=-1
         return
      endif
c
      ier=0
      isys(0)=1
      call scfft3d( 0,n1,n2,n3,scale,data,ld1,ld2,data,ld1/2,ld2,
     *     table,work,isys)
      call scfft3d(-1,n1,n2,n3,scale,data,ld1,ld2,data,ld1/2,ld2,
     *     table,work,isys)

#else
c
c     default to fftw routines 
c
#include <fdds.h>
#include <fftw3.f>
c
      integer FFT_ESTIMATE,FFT_MEASURE
      parameter(FFT_ESTIMATE=1,FFT_MEASURE=2)
c
      real      data(ld1,ld2,n3)
      real      tmp(n1+2,n2,n3)
      pointer   (ptr_tmp,tmp)
      integer   i1,i2,i3,flags, ier_free
      integer*8 plan
c
      i1max=n1+2-mod(n1,2)
      if (n1.lt.1.or.n2.lt.1.or.n3.lt.1.or.ld2.lt.n2.or.
     *    ld1.lt.i1max) then
         fft_rcfft3d=-1
         return
      endif
c
      ptr_tmp=fdds_malloc8(dble(n1+2)*n2*n3*SIZEOF_REAL)
c
      ier=0
      if (iand(ioption,FFT_MEASURE).ne.0) then
         flags=FFTW_MEASURE
      else
         flags=FFTW_ESTIMATE
      endif
      call sfftw_plan_dft_r2c_3d(plan,n1,n2,n3,tmp,tmp,flags)
c
      do i3=1,n3
         do i2=1,n2
            do i1=1,n1
               tmp(i1,i2,i3)=data(i1,i2,i3)
            enddo
         enddo
      enddo
c
      call sfftw_execute(plan)
      call sfftw_destroy_plan(plan)
c
      do i3=1,n3
         do i2=1,n2
            do i1=1,i1max
               data(i1,i2,i3)=scale*tmp(i1,i2,i3)
            enddo
         enddo
      enddo

      ier_free=fdds_free(ptr_tmp)
c
#endif
#endif
#endif
      fft_rcfft3d=ier
      return
      end
