static char rcsid[]="$Id: int8.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=HOST, manipulate HOST_INT8_T integer types

*/

#include "chost.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef HOST_FAKE8

#define	IEEE_EXPONENT_BIAS              1023
#define	IEEE_DOUBLE_EXPONENT_MASK       017774000000
#define	IEEE_DOUBLE_MANTISSA_UPPER_MASK 000003777777
#define	IEEE_DOUBLE_MANTISSA_LOWER_MASK 037777777777
#define	IEEE_DOUBLE_SIGN_MASK           020000000000

#define	TWO_POWER_32 4294967296.0

HOST_INT8_T HostDoubleToInt8(
	HOST_DOUBLE_T arg)
{
   HOST_INT8_T offset = { 0, 0};
   union onion targ;
   int adjusted_exponent, exponent, exponent_shift;
   unsigned int mantissa_lower, mantissa_upper;

   if (arg >= 0.0 && arg < 2147483648.0) {
      offset.hi = 0;
      offset.lo = arg;

      return offset;
   }

   targ.d = arg;
   exponent = (targ.i.int1 & IEEE_DOUBLE_EXPONENT_MASK) >> 20;
   if(exponent)
      adjusted_exponent = exponent - IEEE_EXPONENT_BIAS;
   else
      adjusted_exponent = -1;
   exponent_shift = (52 - adjusted_exponent);

   mantissa_upper = (1 << 20) +	/* IEEE implied mantissa bit */
                    (targ.i.int1 & IEEE_DOUBLE_MANTISSA_UPPER_MASK);
   mantissa_lower = (targ.i.int0 & IEEE_DOUBLE_MANTISSA_LOWER_MASK);

   if(exponent_shift > 31){
      offset.lo = mantissa_upper >> (exponent_shift - 32);
      offset.hi = 0;
   } else {
      offset.lo = mantissa_lower >> exponent_shift;
      offset.lo |= mantissa_upper << (32 - exponent_shift);
      offset.hi = mantissa_upper >> exponent_shift;
   }
   if(targ.i.int1 & IEEE_DOUBLE_SIGN_MASK) {
      if(offset.lo) {
         offset.lo = ~offset.lo + 1;
         offset.hi = ~offset.hi;
      } else {
         offset.hi = ~offset.hi + 1;
      }
   }

	return offset;
}

HOST_DOUBLE_T Int8ToHostDouble(
	HOST_INT8_T arg)
{
   int neg;
   HOST_DOUBLE_T result;

   neg = (arg.hi < 0);
   if (neg) {
      if (arg.lo) {
         arg.lo = ~arg.lo + 1;
         arg.hi = ~arg.hi;
      } else {
         arg.hi = ~arg.hi + 1;
      }
   }

   if (arg.lo < 0) arg.hi++;

   result = (HOST_DOUBLE_T)arg.lo + (HOST_DOUBLE_T)arg.hi * TWO_POWER_32;
   if (neg) result = -result;

	return result;
}

HOST_INT6_T HostInt6Div(
   HOST_INT6_T a,
   HOST_INT6_T b)
{
   HOST_INT6_T c = a / b;

   return (c < 0.0) ? ceil(c) : floor(c);
}

HOST_INT8_T HostLongToInt8(
   long arg)
{
   HOST_INT8_T offset;

   if(arg < 0) offset.hi = -1;
   else offset.hi = 0;

   offset.lo = arg;

   return offset;
}

HOST_INT8_T HostLLongToInt8(
   HOST_LLONG_T arg)
{
   HOST_INT8_T offset;

   if(arg < 0) offset.hi = -1;
   else offset.hi = 0;

   offset.lo = arg;

   return offset;
}

void HostInt8Inc(
   HOST_INT8_T *a,
   HOST_INT8_T b)
{
   *a = HostDoubleToInt8( Int8ToHostDouble(*a) + Int8ToHostDouble(b) );
   return;
}

void HostInt8Dec(
   HOST_INT8_T *a,
   HOST_INT8_T b)
{
   if((a->lo & (~b.lo + 1)) & HOST_FAKE8SIGN) {
      /* always carry */
      a->hi += (~b.hi + 1); a->lo += (~b.lo + 1);
   } else if((a->lo | (~b.lo + 1)) & HOST_FAKE8SIGN) {
      a->lo += (~b.lo + 1);
      if(a->lo & HOST_FAKE8SIGN) {
         a->hi += (~b.hi);
      } else {
         a->hi += (~b.hi + 1);
      }
   } else {
      /* never carry */
      a->hi += (~b.hi); a->lo += (~b.lo + 1);
   }
   if(! b.lo) (a->hi)++;
   return;
}

void HostInt8Mul(
   HOST_INT8_T *a,
   HOST_INT8_T b)
{
   *a = HostDoubleToInt8( Int8ToHostDouble(*a) * Int8ToHostDouble(b) );
   return;
}

void HostInt8Div(
   HOST_INT8_T *a,
   HOST_INT8_T b)
{
   *a = HostDoubleToInt8(Int8ToHostDouble(*a) / Int8ToHostDouble(b));
   return;
}

void HostInt8Mod(
   HOST_INT8_T *a,
   HOST_INT8_T b)
{
   *a = HostDoubleToInt8( fmod(Int8ToHostDouble(*a), Int8ToHostDouble(b)) );
   return;
}

int HostInt8EQ(
   HOST_INT8_T a)
{
   return (! a.hi) && (! a.lo);
}

int HostInt8NE(
   HOST_INT8_T a)
{
   return a.hi || a.lo;
}

int HostInt8LT(
   HOST_INT8_T a)
{
   return a.hi < 0;
}

int HostInt8GT(
   HOST_INT8_T a)
{
   return a.hi > 0 || (! a.hi && a.lo);
}

int HostInt8EQLong(
   HOST_INT8_T a)
{
   if(a.lo & HOST_FAKE8SIGN)
      return a.hi == (long)-1;
   return a.hi == (long)0;
}

long HostInt8ModLong(
   HOST_INT8_T *a,
   long b)
{
   return (a->lo & ~HOST_FAKE8SIGN) % (unsigned long)b;
}

#endif /* HOST_FAKE8 */
