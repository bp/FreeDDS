/**********************************************************************/
/*                    B P   A M E R I C A                             */
/*            PROPRIETARY - TO BE MAINTAINED IN CONFIDENCE            */
/*                          COPYRIGHTED 2008                          */
/**********************************************************************/

/* C wrapper around calls to Jerry's Fortran FFT routines. */

#include "chost.h"
#include "cfft.h"

HOST_FTN_INT cfft_zzfftm(
  int isign,
  float scale,
  int n1,
  int m,
  int ld1,
  double *sdata,
  int ioption) {
   return HOST_FTN_FUNC(fft_zzfftm,FFT_ZZFFTM)(
      &isign, &scale, &n1, &m, &ld1, sdata, &ioption);
}
