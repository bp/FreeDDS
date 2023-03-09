c
c     header file for fft wrapper routines
c
c     ioption parameters
c        FFT_ESTIMATE for fftw routines
c        FFT_MEASURE  for fftw routines
c
      integer FFT_ESTIMATE,FFT_MEASURE
      parameter(FFT_ESTIMATE=1,FFT_MEASURE=2)
c
      integer  
     *     fft_nrfft5,fft_nrfft5odd,fft_rcfftm,fft_crfftm,
     *     fft_rcfft2d,fft_crfft2d,fft_rcfft3d,fft_crfft3d,
     *     fft_rcfft3d_outplace,fft_crfft3d_outplace,
     *     fft_ccfftm,fft_ccfft2d,fft_ccfft3d,
     *     fft_zzfftm,fft_zzfft2d,fft_zzfft3d
      external 
     *     fft_nrfft5,fft_nrfft5odd,fft_rcfftm,fft_crfftm,
     *     fft_rcfft2d,fft_crfft2d,fft_rcfft3d,fft_crfft3d,
     *     fft_rcfft3d_outplace,fft_crfft3d_outplace,
     *     fft_ccfftm,fft_ccfft2d,fft_ccfft3d,
     *     fft_zzfftm,fft_zzfft2d,fft_zzfft3d
