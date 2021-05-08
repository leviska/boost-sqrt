#pragma once
#include "sqrt.h"

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
        uint64_t val = v.template convert_to<uint64_t>();
        uint64_t res = MathSqrt<uint64_t>().Sqrt(val);
        rem = tInt(val - res * res);
        return tInt(res);
    }
    size_t b = bits / 4;
    tInt tmp = v >> ((bits + 1) / 2);
    tInt s = KaratsubaImpl(tmp, rem, bits / 2);
    std::cout << bits << " " << tmp << " " << s << " " << int(s * s <= tmp) << " " << int((s + 1) * (s + 1) > tmp) << std::endl;
    tInt q;
    tmp = (tInt(1) << (b * 2)) - 1;
    tmp &= v;
    tmp >>= b;
    tmp = (rem << b) + tmp;
    divide_qr(tmp, s << 1, q, rem);
    
    rem = (rem << b) + (v & ((tInt(1) << b) - 1));
    s <<= b;
    s += q;
    q *= q;
    if (rem < q) {
        rem += (s << 1) - 1;
        s--;
    }
    rem -= q;
    std::cout << bits << " " << v << " " << v - s * s - rem << " " << s * s + rem << std::endl;  
    return s;
}


template<unsigned Bits>
struct KaratsubaSqrt<tInt<Bits>> {
	tInt<Bits> Sqrt(tInt<Bits> const& value) {
        size_t size = value.backend().size();
        tInt<Bits> res;
        if (size == 0) {
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
		for (size_t i = Bits; i > 0; i--) {
            if (bit_test(value, i - 1)) {
                tInt<Bits> r;
                return KaratsubaImpl(value, r, i);
            }
        }
        return 0;
	}
};

