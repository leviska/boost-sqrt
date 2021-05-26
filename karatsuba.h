#pragma once
#include "sqrt.h"
#include <boost/multiprecision/gmp.hpp>

template<typename T>
struct KaratsubaSqrt {};

template<>
struct KaratsubaSqrt<tInt<32>> {
	tInt<32> Sqrt(const tInt<32>& value) {
		return tInt<32>(MathSqrt<uint32_t>().Sqrt(value.convert_to<uint32_t>()));
	}
};

template<>
struct KaratsubaSqrt<tInt<64>> {
	tInt<64> Sqrt(const tInt<64>& value) {
		return tInt<64>(MathSqrt<uint64_t>().Sqrt(value.convert_to<uint64_t>()));
	}
};

template<typename tInt>
tInt KaratsubaImpl(tInt v, tInt& rem, size_t bits) {
    if (bits <= 64) {
        uint64_t val = static_cast<uint64_t>(v);
        uint64_t s = MathSqrt<uint64_t>().Sqrt(val);
        rem = tInt(val - s * s);
        return tInt(s);
    }
    size_t b = bits / 4;
    tInt s = KaratsubaImpl(tInt(v >> (b * 2)), rem, bits - b * 2);
    tInt q;
    rem <<= b;
    divide_qr(tInt(rem + ((v & ((tInt(1) << (b * 2)) - 1)) >> b)), tInt(s << 1), q, rem);
    rem <<= b;
    rem += (v & ((tInt(1) << b) - 1));
    s <<= b;
    s += q;
    q *= q;
    if (rem < q) {
        rem += (s << 1) - 1;
        s--;
    }
    rem -= q;
    return s;
}


template<unsigned Bits>
struct KaratsubaSqrt<tInt<Bits>> {
	tInt<Bits> Sqrt(tInt<Bits> const& value) {
        size_t size = value.backend().size();
        tInt<Bits> res;
        if (size == 0 || Bits <= 128 && value == 0) {
            res = 0;
        }
        else if (size == 1 && Bits > 128) {
            res = MathSqrt<uint64_t>().Sqrt(value.template convert_to<uint64_t>());
        }
        else {
            res = sqrt(value);
        }
        return res;
	}
private:
	tInt<Bits> sqrt(tInt<Bits> const& value) {
        tInt<Bits> r;
        return KaratsubaImpl(value, r, msb(value) + 1);
	}
};

template<>
struct KaratsubaSqrt<cpp_int> {
	cpp_int Sqrt(cpp_int const& value) {
        size_t size = value.backend().size();
        cpp_int res;
        if (size == 0) {
            res = 0;
        }
        else if (size == 1) {
            res = MathSqrt<uint64_t>().Sqrt(value.template convert_to<uint64_t>());
        }
        else {
            res = sqrt(value);
        }
        return res;
	}
private:
	cpp_int sqrt(cpp_int const& value) {
        cpp_int r;
        return KaratsubaImpl(value, r, msb(value) + 1);
	}
};

template<>
struct KaratsubaSqrt<mpz_int> {
	mpz_int Sqrt(mpz_int const& value) {
        size_t size = value.backend().data()->_mp_size;
        mpz_int res;
        if (size == 0) {
            res = 0;
        }
        else if (size == 1) {
            res = MathSqrt<uint64_t>().Sqrt(value.template convert_to<uint64_t>());
        }
        else {
            res = sqrt(value);
        }
        return res;
	}
private:
	mpz_int sqrt(mpz_int const& value) {
        mpz_int r;
        return KaratsubaImpl(value, r, msb(value) + 1);
	}
};
