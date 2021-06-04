#pragma once
#include "sqrt.h"
#include <boost/multiprecision/gmp.hpp>

template <class Integer>
Integer bmp_karatsuba_sqrt(const Integer& x, Integer& r, size_t bits)
{
   #ifndef BOOST_MP_NO_CONSTEXPR_DETECTION
   // std::sqrt is not constexpr by standard, so use this 
   if (BOOST_MP_IS_CONST_EVALUATED(bits)) {
      if (bits <= 4) {
         if (x == 0) {
            r = 0;
            return 0;
         }
         else if (x < 4) {
            r = x - 1;
            return 1;
         }
         else if (x < 9) {
            r = x - 4;
            return 2;
         }
         else {
            r = x - 9;
            return 3;
         }
      }
   }
   else
#endif
   // we can calculate it faster with std::sqrt
   if (bits <= 64) {
      const uint64_t int32max = uint64_t((std::numeric_limits<uint32_t>::max)());
      uint64_t val = static_cast<uint64_t>(x);
      uint64_t s = static_cast<uint64_t>(std::sqrt(static_cast<long double>(val)));
      // converting to long double can loose some precision, and `sqrt` can give eps error, so we'll fix this
      // this is needed
      while (s > int32max || s * s > val) s--;
      // in my tests this never fired, but theoretically this might be needed
      while (s < int32max && (s + 1) * (s + 1) <= val) s++;
      r = Integer(val - s * s);
      return Integer(s);
   }
   // https://hal.inria.fr/file/index/docid/72854/filename/RR-3805.pdf
   size_t b = bits / 4;
   Integer s = bmp_karatsuba_sqrt(Integer(x >> (b * 2)), r, bits - b * 2);
   Integer q{};
   r <<= b;
   divide_qr(Integer(r + ((x & ((Integer(1) << (b * 2)) - 1)) >> b)), Integer(s << 1), q, r);
   r <<= b;
   r += (x & ((Integer(1) << b) - 1));
   s <<= b;
   s += q;
   q *= q;
   // we substract after, so it works for unsigned integers too
   if (r < q) {
      r += (s << 1) - 1;
      s--;
   }
   r -= q;
   return s;
}

template <class Integer>
Integer bmp_sqrt(const Integer& x, Integer& r)
{
   if (x == Integer(0)) {
      r = Integer(0);
      return Integer(0);
   }
   return bmp_karatsuba_sqrt(x, r, msb(x) + 1);
}

template <class Integer>
Integer bmp_sqrt(const Integer& x)
{
   Integer r(0);
   return bmp_sqrt(x, r);
}

template <class Integer>
Integer bmp_2_karatsuba_sqrt(const Integer& x, Integer& r, Integer& t, size_t bits)
{
#ifndef BOOST_MP_NO_CONSTEXPR_DETECTION
   // std::sqrt is not constexpr by standard, so use this 
   if (BOOST_MP_IS_CONST_EVALUATED(bits)) {
      if (bits <= 4) {
         if (x.is_zero()) {
            r = 0u;
            return 0u; 
         }
         else if (x < 4) {
            r = x - 1;
            return 1u; 
         }
         else if (x < 9) {
            r = x - 4;
            return 2u; 
         }
         else {
            r = x - 9;
            return 3u; 
         }
      }
   }
   else
#endif
   // we can calculate it faster with std::sqrt
   if (bits <= 64) {
      const uint64_t int32max = uint64_t((std::numeric_limits<uint32_t>::max)());
      uint64_t val = static_cast<uint64_t>(x);
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
   Integer q = x;
   q >>= b * 2;
   Integer s = bmp_2_karatsuba_sqrt(q, r, t, bits - b * 2);
   t = 0u;
   bit_set(t, b * 2);
   r <<= b;
   t--;
   t &= x;
   t >>= b;
   t += r;
   s <<= 1;
   divide_qr(t, s, q, r);
   r <<= b;
   t = 0u;
   bit_set(t, b);
   t--;
   t &= x;
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
Integer bmp_2_sqrt(const Integer& x, Integer& r)
{
   if (x.is_zero()) {
      r = 0u;
      return 0u;
   }
   Integer t;
   return bmp_2_karatsuba_sqrt(x, r, t, msb(x) + 1);
}

template <class Integer>
Integer bmp_2_sqrt(const Integer& x)
{
   Integer r(0);
   return bmp_2_sqrt(x, r);
}
