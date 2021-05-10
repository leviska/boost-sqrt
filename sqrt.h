#pragma once
#include <boost/multiprecision/number.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <algorithm>
#include <cinttypes>
#include <cmath>

using namespace boost::multiprecision;

template<size_t Bits>
using tInt = number<cpp_int_backend<Bits, Bits, signed_magnitude, unchecked, void>>;

template<typename T>
struct MathSqrt {};

template<>
struct MathSqrt<uint32_t> {
	uint32_t Sqrt(uint32_t value) {
		uint32_t res = static_cast<uint32_t>(std::sqrt(static_cast<long double>(value)));
		return res;
	}
};

template<>
struct MathSqrt<uint64_t> {
	uint64_t Sqrt(uint64_t value) {
		constexpr uint64_t int32max = uint64_t(std::numeric_limits<uint32_t>::max());
		uint64_t res = std::min(
			int32max,
			static_cast<uint64_t>(std::sqrt(static_cast<long double>(value)))
		);
		// sqrtl can loose some lowest bits in precision, so we'll fix this
		while (res * res > value) res--;
		//while (res < int32max && (res + 1) * (res + 1) <= value) res++; // i think? that's not needed
		return res;
	}
};

template<typename Int, typename Backend, size_t Length>
void FillRandom(std::vector<Int>& v) {
    boost::random::independent_bits_engine<boost::random::mt19937, Length, Backend> gen;
    for (auto &i : v) {
        i = Int(gen());
    }
}
