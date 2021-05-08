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

template<>
struct NewtonSqrt<tInt<128>> {
	tInt<128> Sqrt(tInt<128> const& value) {
		if (value == 0)
			return 0;
		tInt<128> res = sqrt(value);
		return res;
	}

private:
	tInt<128> sqrt(tInt<128> const& value) {
		tInt<128> res = std::max(uint64_t(1), MathSqrt<uint64_t>().Sqrt((value >> 64).convert_to<uint64_t>()));
		res <<= 32; // shift sqrt in the right place
		
		bool decreased = false; // newton approximation can get stuck in +-1 cycle
		while (true) {
			tInt<128> nx = (res + value / res) >> 1; // newton approximation
			int cmp = res.compare(nx);
			if (cmp == 0 || (cmp < 0 && decreased))
				break;
			decreased = cmp > 0;
			res = std::move(nx);
		}
		return res;
	}
};

template<unsigned Bits>
struct NewtonSqrt<tInt<Bits>> {
	tInt<Bits> Sqrt(tInt<Bits> const& value) {
		if constexpr (Bits <= 128) {
			return tInt<Bits>(NewtonSqrt<tInt<128>>().Sqrt(tInt<128>(value)));
		}
		else {
			size_t size = value.backend().size();
			tInt<Bits> res;
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
	}

private:
	tInt<Bits> sqrt(tInt<Bits> const& value) {
		size_t size = value.backend().size();
		boost::multiprecision::limb_type const* arr = value.backend().limbs();
		// we'll approximate result with sqrt of 2 highest bins (because sqrt<int64_t> is basically free)
		tInt<Bits> res = MathSqrt<uint64_t>().Sqrt(arr[size - 1]);
		res <<= (size - 1) * 32; // shift sqrt in the right place
		assert(res > 0);
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
