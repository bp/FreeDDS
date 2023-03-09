/**********************************************************************/
/*                    B P   A M E R I C A                             */
/*            PROPRIETARY - TO BE MAINTAINED IN CONFIDENCE            */
/*                          COPYRIGHTED 2008                          */
/**********************************************************************/

/* C wrapper around calls to Jerry's Fortran FFT routines. */

#include "chost.h"
#include "cfft.h"

HOST_FTN_INT cfft_ccfft2d(
  int isign,
  float scale,
  int n1,
  int n2,
  int ld1,
  float *sdata,
  int ioption) {
   return HOST_FTN_FUNC(fft_ccfft2d,FFT_CCFFT2D)(
      &isign, &scale, &n1, &n2, &ld1, sdata, &ioption);
}
