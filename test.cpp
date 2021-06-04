#define BOOST_TEST_MODULE My Test
#include <boost/test/included/unit_test.hpp>
#include "newton.h"
#include "karatsuba.h"
#include "karatsubapr.h"

template <class tInt>
tInt IntSqrt(tInt const& n) {
	return bmp_sqrt<tInt>(n);
}

template<typename tInt>
static void CheckSqrt(tInt const& sqrt, tInt const& value) {
	BOOST_CHECK_LE(sqrt * sqrt, value);
	BOOST_CHECK_GT((sqrt + 1) * (sqrt + 1), value);
}

template<typename tInt>
static void TestNearValue(tInt const& value, uint64_t from = 100, uint64_t to = 100) {
	for (boost::multiprecision::cpp_int i = value - from; i <= value + to; i++) {
		boost::multiprecision::cpp_int sqrt = IntSqrt<tInt>(tInt(value));
		CheckSqrt<boost::multiprecision::cpp_int>(sqrt, value);
	}
}

const bool PrintProgress = true;

BOOST_AUTO_TEST_CASE(TestSandbox) {
	tInt<128> a("94768032953577563142726594757254805520");
	std::cout << "norm" << std::endl;
	tInt<128> b = bmp_2_sqrt(a);
	std::cout << "ne norm" << std::endl;
	tInt<128> c = kar_sqrt(a);
	BOOST_CHECK_EQUAL(b, c);
}

BOOST_AUTO_TEST_CASE(TestFirstN) {
	uint64_t maxValue = 10000000;
	// just check every number in [0, maxValue]
	for (uint64_t i = 0; i < maxValue; i++) {
		tInt<32> v32 = IntSqrt<tInt<32>>(tInt<32>(i));
		tInt<64> v64 = IntSqrt<tInt<64>>(tInt<64>(i));
		tInt<128> v128 = IntSqrt<tInt<128>>(tInt<128>(i));
		tInt<256> v256 = IntSqrt<tInt<256>>(tInt<256>(i));
		BOOST_CHECK_EQUAL(v32, v64);
		BOOST_CHECK_EQUAL(tInt<128>(v64), v128);
		BOOST_CHECK_EQUAL(tInt<256>(v64), v256);
		CheckSqrt<tInt<64>>(v32, i);
		CheckSqrt<tInt<64>>(v64, i);
		CheckSqrt<tInt<128>>(v128, i);
		CheckSqrt<tInt<256>>(v256, i);
		if (PrintProgress && i % 1000000 == 0) std::cout << i / 100000 << std::endl;
	}
}

BOOST_AUTO_TEST_CASE(TestMinMax) {
	std::array<tInt<256>, 8> intMax;
	for (int i = 2; i < 8; i++) {
		intMax[i] = tInt<256>((tInt<512>(1) << ((i + 1) * 32)) - 1);
	}
	int delta = 100000;
	// test max values for each type
	TestNearValue(tInt<32>(intMax[0]), delta, 0);
	TestNearValue(tInt<64>(intMax[1]), delta, 0);
	TestNearValue(tInt<96>(intMax[2]), delta, 0);
	TestNearValue(tInt<128>(intMax[3]), delta, 0);
	TestNearValue(tInt<160>(intMax[4]), delta, 0);
	TestNearValue(tInt<192>(intMax[5]), delta, 0);
	TestNearValue(tInt<224>(intMax[6]), delta, 0);
	TestNearValue(tInt<256>(intMax[7]), delta, 0);
}

BOOST_AUTO_TEST_CASE(TestNearMax) {
	std::array<tInt<256>, 8> intMax;
	for (int i = 2; i < 8; i++) {
		intMax[i] = tInt<256>((tInt<512>(1) << ((i + 1) * 32)) - 1);
	}	
	for (int i = 0; i < 7; i++) {
		tInt<512> cur = intMax[i];
		tInt<512> next = intMax[i + 1];
		const int N = 100;
		for (int j = 0; j < N; j++) {
			tInt<512> middle = ((next - cur) * j) / N + cur;
			TestNearValue(tInt<256>(middle), 1000, 1000);
		}
		if (PrintProgress) std::cout << i << std::endl;
	}
}

BOOST_AUTO_TEST_CASE(TestSegments) {
	for (uint64_t i = 1000; i < 100000; i++) {
		uint64_t value = i * i;
		uint64_t next = (i + 1) * (i + 1);
		const int N = 5;
		for (int j = 0; j < N; j++) {
			uint64_t middle = ((next - value) * j) / N + value;
			TestNearValue<tInt<64>>(middle);
		}
		if (PrintProgress && i % 10000 == 0) std::cout << i / 1000 << std::endl;
	}
}
