/**********************************************************************/
/*                    B P   A M E R I C A                             */
/*            PROPRIETARY - TO BE MAINTAINED IN CONFIDENCE            */
/*                          COPYRIGHTED 2005                          */
/**********************************************************************/

/* C wrapper around calls to Jerry's Fortran FFT routines. */

#include "chost.h"
#include "cfft.h"

HOST_FTN_INT cfft_nrfft5(
  int n) {
  return HOST_FTN_FUNC(fft_nrfft5,FFT_NRFFT5)(&n);
}
