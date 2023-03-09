/**********************************************************************/
/*                    B P   A M E R I C A                             */
/*            PROPRIETARY - TO BE MAINTAINED IN CONFIDENCE            */
/*                          COPYRIGHTED 2008                          */
/**********************************************************************/

/* C wrapper around calls to Jerry's Fortran FFT routines. */

#include "chost.h"
#include "cfft.h"

HOST_FTN_INT cfft_zzfft3d(          
  int isign,
  float scale,
  int n1,
  int n2,
  int n3,
  int ld1,
  int ld2,
  double *sdata,
  int ioption) {
   return HOST_FTN_FUNC(fft_zzfft3d,FFT_ZZFFT3D)(
      &isign, &scale, &n1, &n2, &n3, &ld1, &ld2, sdata, &ioption);
}
