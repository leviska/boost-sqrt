#pragma once
#include "sqrt.h"

template<typename T>
struct NewtonSqrt {};

template<>
struct NewtonSqrt<tInt<32>> {
	tInt<32> Sqrt(const tInt<32>& value) {
		return tInt<32>(MathSqrt<uint32_t>().Sqrt(value.convert_to<uint32_t>()));
	}
};

template<>
struct NewtonSqrt<tInt<64>> {
	tInt<64> Sqrt(const tInt<64>& value) {
		return tInt<64>(MathSqrt<uint64_t>().Sqrt(value.convert_to<uint64_t>()));
	}
};

template<unsigned Bits>
struct NewtonSqrt<tInt<Bits>> {
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
		size_t shift = msb(value);
		shift = shift < 64 ? 0 : shift - 64;
		tInt<Bits> res(MathSqrt<uint64_t>().Sqrt((value >> shift).template convert_to<uint64_t>()));
		res <<= shift / 2;
		bool decreased = false; // newton approximation can get stuck in +-1 cycle
		while (true) {
			tInt<Bits> nx = (res + value / res) >> 1; // newton approximation
			int cmp = res.compare(nx);
			if (cmp == 0 || (cmp < 0 && decreased))
				break;
			decreased = cmp > 0;
			res = std::move(nx);
		}
		return res;
	}
};
