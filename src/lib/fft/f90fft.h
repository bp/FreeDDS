!
!     Free-form F90 header file for fft wrapper routines
!
!     ioption parameters
!        FFT_ESTIMATE for fftw routines
!        FFT_MEASURE  for fftw routines
!
      integer,parameter :: FFT_ESTIMATE=1,FFT_MEASURE=2
!
      integer &
           fft_nrfft5,fft_nrfft5odd,fft_rcfftm,fft_crfftm, &
           fft_rcfft2d,fft_crfft2d,fft_rcfft3d,fft_crfft3d, &
           fft_rcfft3d_outplace,fft_crfft3d_outplace, &
           fft_ccfftm,fft_ccfft2d,fft_ccfft3d, &
           fft_zzfftm,fft_zzfft2d,fft_zzfft3d
      external &
           fft_nrfft5,fft_nrfft5odd,fft_rcfftm,fft_crfftm, &
           fft_rcfft2d,fft_crfft2d,fft_rcfft3d,fft_crfft3d, &
           fft_rcfft3d_outplace,fft_crfft3d_outplace, &
           fft_ccfftm,fft_ccfft2d,fft_ccfft3d, &
           fft_zzfftm,fft_zzfft2d,fft_zzfft3d
