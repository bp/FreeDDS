#ifndef __CFFT_H
#define __CFFT_H

/***********************************************************************/
/*                                                                     */
/*                    B P     C O R P O R A T I O N                    */
/*            PROPRIETARY  -  TO BE MAINTAINED IN CONFIDENCE           */
/*                            COPYRIGHTED 2002                         */
/***********************************************************************/

#include "chost.h"

#ifdef  __cplusplus
extern "C" {
#endif

#define FFT_ESTIMATE 1
#define FFT_MEASURE  2

/*******************************/
/***  Function declaration.  ***/
/*******************************/

#if defined(FUNCPROTO) || __STDC__ || defined(__STDC__) || \
   __SUNPRO_C || defined(__cplusplus) || defined(c_plusplus)

   /* ANSI style function prototypes */

HOST_FTN_INT cfft_nrfft5(
  int n);

HOST_FTN_INT cfft_nrfft5odd(
  int n);

HOST_FTN_INT cfft_ccfft2d(
  int isign,
  float scale,
  int n1,
  int n2,
  int ld1,
  float *sdata,
  int ioption);

HOST_FTN_INT cfft_ccfft3d(
  float scale,
  int n1,
  int n2,
  int n3,
  int ld1,
  int ld2,
  float *sdata,
  int ioption);

HOST_FTN_INT cfft_ccfftm(
   int isign,
  float scale,
  int n1,
  int m,
  int ld1,
  float *sdata,
  int ioption);

HOST_FTN_INT cfft_crfft2d(
  float scale,
  int n1,
  int n2,
  int ld1,
  float *sdata,
  int ioption);

HOST_FTN_INT cfft_crfft3d(
  float scale,
  int n1,
  int n2,
  int n3,
  int ld1,
  int ld3,
  float *sdata,
  int ioption);

HOST_FTN_INT cfft_crfftm(
  float scale,
  int n1,
  int m,
  int ld1,
  float *sdata,
  int ioption);

HOST_FTN_INT cfft_rcfft2d(
  float scale,
  int n1,
  int n2,
  int ld1,
  float *sdata,
  int ioption);

HOST_FTN_INT cfft_rcfft3d(
  float scale,
  int n1,
  int n2,
  int n3,
  int ld1,
  int ld2,
  float *sdata,
  int ioption);

HOST_FTN_INT cfft_rcfftm(
  float scale,
  int n1,
  int m,
  int ld1,
  float *sdata,
  int ioption);

HOST_FTN_INT cfft_zzfft2d(
  int isign,
  float scale,
  int n1,
  int n2,
  int ld1,
  double *sdata,
  int ioption);

HOST_FTN_INT cfft_zzfft3d(          
  int isign,
  float scale,
  int n1,
  int n2,
  int n3,
  int ld1,
  int ld2,
  double *sdata,
  int ioption);

HOST_FTN_INT cfft_zzfftm(
  int isign,
  float scale,
  int n1,
  int m,
  int ld1,
  double *sdata,
  int ioption);

HOST_FTN_INT HOST_FTN_FUNC(fft_nrfft5,FFT_NRFFT5)(
  int *n);

HOST_FTN_INT HOST_FTN_FUNC(fft_nrfft5odd,FFT_NRFFT5ODD)(
  int *n);

HOST_FTN_INT HOST_FTN_FUNC(fft_ccfft2d,FFT_CCFFT2D)(
  int *isign,
  float *scale,
  int *n1,
  int *n2,
  int *ld1,
  float *sdata,
  int *ioption);

HOST_FTN_INT HOST_FTN_FUNC(fft_ccfft3d,FFT_CCFFT3D)(
  float *scale,
  int *n1,
  int *n2,
  int *n3,
  int *ld1,
  int *ld2,
  float *sdata,
  int *ioption);

HOST_FTN_INT HOST_FTN_FUNC(fft_ccfftm,FFT_CCFFTM)(
   int *isign,
  float *scale,
  int *n1,
  int *m,
  int *ld1,
  float *sdata,
  int *ioption);

HOST_FTN_INT HOST_FTN_FUNC(fft_crfft2d,FFT_CRFFT2D)(
  float *scale,
  int *n1,
  int *n2,
  int *ld1,
  float *sdata,
  int *ioption);

HOST_FTN_INT HOST_FTN_FUNC(fft_crfft3d,FFT_CRFFT3D)(
  float *scale,
  int *n1,
  int *n2,
  int *n3,
  int *ld1,
  int *ld3,
  float *sdata,
  int *ioption);

HOST_FTN_INT HOST_FTN_FUNC(fft_crfftm,FFT_CRFFTM)(
  float *scale,
  int *n1,
  int *m,
  int *ld1,
  float *sdata,
  int *ioption);

HOST_FTN_INT HOST_FTN_FUNC(fft_rcfft2d,FFT_RCFFT2D)(
  float *scale,
  int *n1,
  int *n2,
  int *ld1,
  float *sdata,
  int *ioption);

HOST_FTN_INT HOST_FTN_FUNC(fft_rcfft3d,FFT_RCFFT3D)(
  float *scale,
  int *n1,
  int *n2,
  int *n3,
  int *ld1,
  int *ld2,
  float *sdata,
  int *ioption);

HOST_FTN_INT HOST_FTN_FUNC(fft_rcfftm,FFT_RCFFTM)(
  float *scale,
  int *n1,
  int *m,
  int *ld1,
  float *sdata,
  int *ioption);

HOST_FTN_INT HOST_FTN_FUNC(fft_zzfft2d,FFT_ZZFFT2D)(
  int *isign,
  float *scale,
  int *n1,
  int *n2,
  int *ld1,
  double *sdata,
  int *ioption);

HOST_FTN_INT HOST_FTN_FUNC(fft_zzfft3d,FFT_ZZFFT3D)(          
  int *isign,
  float *scale,
  int *n1,
  int *n2,
  int *n3,
  int *ld1,
  int *ld2,
  double *sdata,
  int *ioption);

HOST_FTN_INT HOST_FTN_FUNC(fft_zzfftm,FFT_ZZFFTM)(
  int *isign,
  float *scale,
  int *n1,
  int *m,
  int *ld1,
  double *sdata,
  int *ioption);

#else
   /* old style function declaration */

HOST_FTN_INT cfft_nrfft5();

HOST_FTN_INT cfft_nrfft5odd();

HOST_FTN_INT cfft_ccfft2d();

HOST_FTN_INT cfft_ccfft3d();

HOST_FTN_INT cfft_ccfftm();

HOST_FTN_INT cfft_crfft2d();

HOST_FTN_INT cfft_crfft3d();

HOST_FTN_INT cfft_crfftm();

HOST_FTN_INT cfft_rcfft2d();

HOST_FTN_INT cfft_rcfft3d();

HOST_FTN_INT cfft_rcfftm();

HOST_FTN_INT cfft_zzfft2d();

HOST_FTN_INT cfft_zzfft3d();

   HOST_FTN_INT cfft_zzfftm()

HOST_FTN_INT HOST_FTN_FUNC(fft_nrfft5,FFT_NRFFT5)();

HOST_FTN_INT HOST_FTN_FUNC(fft_nrfft5odd,FFT_NRFFT5ODD)();

HOST_FTN_INT HOST_FTN_FUNC(fft_ccfft2d,FFT_CCFFT2D)();

HOST_FTN_INT HOST_FTN_FUNC(fft_ccfft3d,FFT_CCFFT3D)();

HOST_FTN_INT HOST_FTN_FUNC(fft_ccfftm,FFT_CCFFTM)();

HOST_FTN_INT HOST_FTN_FUNC(fft_crfft2d,FFT_CRFFT2D)();

HOST_FTN_INT HOST_FTN_FUNC(fft_crfft3d,FFT_CRFFT3D)();

HOST_FTN_INT HOST_FTN_FUNC(fft_crfftm,FFT_CRFFTM)();

HOST_FTN_INT HOST_FTN_FUNC(fft_rcfft2d,FFT_RCFFT2D)();

HOST_FTN_INT HOST_FTN_FUNC(fft_rcfft3d,FFT_RCFFT3D)();

HOST_FTN_INT HOST_FTN_FUNC(fft_rcfftm,FFT_RCFFTM)();

HOST_FTN_INT HOST_FTN_FUNC(fft_zzfft2d,FFT_ZZFFT2D)();

HOST_FTN_INT HOST_FTN_FUNC(fft_zzfft3d,FFT_ZZFFT3D)( );

HOST_FTN_INT HOST_FTN_FUNC(fft_zzfftm,FFT_ZZFFTM)();

#endif /* function declaration */

#ifdef  __cplusplus
}
#endif

#endif /* __CFFT_H */
