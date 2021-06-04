#pragma once
#include "sqrt.h"
#include <boost/multiprecision/gmp.hpp>

template <class Integer>
Integer karatsuba_sqrt(const Integer& x, Integer& r, Integer& t, Integer& q, size_t offset, size_t bits)
{
#ifndef BOOST_MP_NO_CONSTEXPR_DETECTION
   // std::sqrt is not constexpr by standard, so use this 
   if (BOOST_MP_IS_CONST_EVALUATED(bits)) {
      if (bits <= 4) {
         Integer c = x;
         c >>= offset;
         if (c.is_zero()) {
            r = 0u;
            return 0u; 
         }
         else if (c < 4) {
            r = c - 1;
            return 1u; 
         }
         else if (c < 9) {
            r = c - 4;
            return 2u; 
         }
         else {
            r = c - 9;
            return 3u; 
         }
      }
   }
   else
#endif
   // we can calculate it faster with std::sqrt
   if (bits <= 64) {
      const uint64_t int32max = uint64_t((std::numeric_limits<uint32_t>::max)());
      uint64_t val = static_cast<uint64_t>(x >> offset);
      uint64_t s64 = static_cast<uint64_t>(std::sqrt(static_cast<long double>(val)));
      // converting to long double can loose some precision, and `sqrt` can give eps error, so we'll fix this
      // this is needed
      while (s64 > int32max || s64 * s64 > val) s64--;
      // in my tests this never fired, but theoretically this might be needed
      while (s64 < int32max && (s64 + 1) * (s64 + 1) <= val) s64++;
      r = val - s64 * s64;
      return s64;
   }
   // https://hal.inria.fr/file/index/docid/72854/filename/RR-3805.pdf
   size_t b = bits / 4;
   Integer s = karatsuba_sqrt(x, r, t, q, offset + b * 2, bits - b * 2);

   r <<= b;
   t ^= t;
   bit_set(t, offset + b * 2);
   t--;
   t &= x;
   t >>= (b + offset);
   t += r;
   s <<= 1;
   divide_qr(t, s, q, r);
   
   r <<= b;
   t ^= t;
   bit_set(t, b + offset);
   t--;
   t &= x;
   t >>= offset;
   r += t;
   s <<= (b - 1); // we already <<1 it before
   s += q;
   q *= q;

   // we substract after, so it works for unsigned integers too
   if (r < q) {
      t = s;
      t <<= 1;
      t--;
      r += t;
      s--;
   }
   r -= q;
   return s;
}

template <class Integer>
Integer kar_sqrt(const Integer& x, Integer& r)
{
   if (x.is_zero()) {
      r = 0u;
      return 0u;
   }
   Integer t{};
   Integer q{};
   return karatsuba_sqrt(x, r, t, q, 0, msb(x) + 1);
}

template <class Integer>
Integer kar_sqrt(const Integer& x)
{
   Integer r(0);
   return kar_sqrt(x, r);
}
