#include <benchmark/benchmark.h>
#include <boost/multiprecision/gmp.hpp>
#include "karatsuba.h"

template<typename T, size_t Left, size_t Right, typename F>
void BenchOper(benchmark::State &state, F f) {
    std::vector<T> a(10000);
    std::vector<T> b(10000);
	std::vector<T> res(10000);
    if constexpr (std::is_same<T, mpz_int>::value) {
	    FillRandom<T, mpz_int, Left>(a);
	    FillRandom<T, mpz_int, Right>(b);
    }
    else {
	    FillRandom<T, cpp_int, Left>(a);
	    FillRandom<T, cpp_int, Right>(b);
    }
	size_t i = 0;
    for (auto _ : state) {
        res[i] = f(a[i], b[i]);
		if (++i >= res.size()) {
			i = 0;
		}
	}
}

template<typename T, size_t Left, size_t Right, typename F>
void RegisterOper(const std::string& testName, F f) {
    benchmark::RegisterBenchmark(testName.c_str(), [f](benchmark::State &state) {
        BenchOper<T, Left, Right, F>(state, f);
	});
}

template<typename T, size_t Left, size_t Right, typename F>
void RegisterOperPref(const std::string& prefix, F f) {
    RegisterOper<T, Left, Right, F>(prefix + "_" + std::to_string(Left) + "_" + std::to_string(Right), f);
}

template<size_t LeftMul, typename F>
void RegisterBoost(const std::string& prefix, F f) {
	RegisterOperPref<tInt<64>, 64 * LeftMul, 64, F>(prefix, f);
	RegisterOperPref<tInt<128>, 128 * LeftMul, 128, F>(prefix, f);
	RegisterOperPref<tInt<256>, 256 * LeftMul, 256, F>(prefix, f);
	RegisterOperPref<tInt<512>, 512 * LeftMul, 512, F>(prefix, f);
	RegisterOperPref<tInt<1024>, 1024 * LeftMul, 1024, F>(prefix, f);
	RegisterOperPref<tInt<8192>, 8192 * LeftMul, 8192, F>(prefix, f);
	//RegisterOperPref<tInt<65536>, 65536 * LeftMul, 65536, F>(prefix, f);
}

template<typename T, size_t LeftMul, typename F>
void RegisterArbitrary(const std::string& prefix, F f) {
	RegisterOperPref<T, 64 * LeftMul, 64, F>(prefix, f);
	RegisterOperPref<T, 128 * LeftMul, 128, F>(prefix, f);
	RegisterOperPref<T, 256 * LeftMul, 256, F>(prefix, f);
	RegisterOperPref<T, 512 * LeftMul, 512, F>(prefix, f);
	RegisterOperPref<T, 1024 * LeftMul, 1024, F>(prefix, f);
	RegisterOperPref<T, 8192 * LeftMul, 8192, F>(prefix, f);
	//RegisterOperPref<T, 65536 * LeftMul, 65536, F>(prefix, f);
}

template<typename T>
T KarSqrt(const T& t) {
    return Karatsuba<T>().Sqrt(t);
}

template<bool Kar, typename T>
T Compute(T a, T b) {
	a >>= msb(a) / 2;
	b >>= msb(b) / 2;
	T c = (a * b) / (a + b);
	T s = a * b * c * (a + b + c);
	if constexpr (Kar) {
		s = Karatsuba<T>().Sqrt(s);
	}
	else {
		s = boost::multiprecision::sqrt(s);
	}
	T x = (a * c - s) / (a * a + a * b);
	T y = (b * c + s) / (b * b + a * b);
	return (x + y);
}

void RegisterOper() {
	RegisterBoost<1>("Boost: CopyBaseline", [](const auto& a, const auto&) { return a; });
	RegisterArbitrary<cpp_int, 1>("Boost arbitrary: CopyBaseline", [](const auto& a, const auto&) { return a; });
	RegisterArbitrary<mpz_int, 1>("GMP: CopyBaseline", [](const auto& a, const auto&) { return a; });

    RegisterBoost<1>("Boost: Add", [](const auto& a, const auto& b) { return a + b; });
	RegisterArbitrary<cpp_int, 1>("Boost arbitrary: Add", [](const auto& a, const auto& b) { return a + b; });
	RegisterArbitrary<mpz_int, 1>("GMP: Add", [](const auto& a, const auto& b) { return a + b; });

	RegisterBoost<1>("Boost: Sub", [](const auto& a, const auto& b) { return a - b; });
	RegisterArbitrary<cpp_int, 1>("Boost arbitrary: Sub", [](const auto& a, const auto& b) { return a - b; });
	RegisterArbitrary<mpz_int, 1>("GMP: Sub", [](const auto& a, const auto& b) { return a - b; });

	RegisterBoost<1>("Boost: Mul", [](const auto& a, const auto& b) { return a * b; });
	RegisterArbitrary<cpp_int, 1>("Boost arbitrary: Mul", [](const auto& a, const auto& b) { return a * b; });
	RegisterArbitrary<mpz_int, 1>("GMP: Mul", [](const auto& a, const auto& b) { return a * b; });

	RegisterBoost<2>("Boost: Div", [](const auto& a, const auto& b) { return a / b; });
	RegisterArbitrary<cpp_int, 2>("Boost arbitrary: Div", [](const auto& a, const auto& b) { return a / b; });
	RegisterArbitrary<mpz_int, 2>("GMP: Div", [](const auto& a, const auto& b) { return a / b; });

	RegisterBoost<1>("Boost: Compute", [](const auto& a, const auto& b) { return Compute<false>(a, b); });
	RegisterBoost<1>("Boost kar: Compute", [](const auto& a, const auto& b) { return Compute<true>(a, b); });
	RegisterArbitrary<cpp_int, 1>("Boost arbitrary: Compute", [](const auto& a, const auto& b) { return Compute<false>(a, b); });
	RegisterArbitrary<cpp_int, 1>("Boost kar arbitrary: Compute", [](const auto& a, const auto& b) { return Compute<true>(a, b); });
	RegisterArbitrary<mpz_int, 1>("GMP: Compute", [](const auto& a, const auto& b) { return Compute<false>(a, b); });
}