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
	RegisterOperPref<tInt<128>, 128 * LeftMul, 128, F>(prefix, f);
	RegisterOperPref<tInt<256>, 256 * LeftMul, 256, F>(prefix, f);
	RegisterOperPref<tInt<512>, 512 * LeftMul, 512, F>(prefix, f);
	RegisterOperPref<tInt<1024>, 1024 * LeftMul, 1024, F>(prefix, f);
}

template<size_t LeftMul, typename F>
void RegisterGMP(const std::string& prefix, F f) {
	RegisterOperPref<mpz_int, 128 * LeftMul, 128, F>(prefix, f);
	RegisterOperPref<mpz_int, 256 * LeftMul, 256, F>(prefix, f);
	RegisterOperPref<mpz_int, 512 * LeftMul, 512, F>(prefix, f);
	RegisterOperPref<mpz_int, 1024 * LeftMul, 1024, F>(prefix, f);
}

template<typename T>
T KarSqrt(const T& t) {
    return KaratsubaSqrt<T>().Sqrt(t);
}

void RegisterOper() {
	RegisterBoost<1>("Boost: Add", [](const auto& a, const auto& b) { return a + b; });
	RegisterGMP<1>("GMP: Add", [](const auto& a, const auto& b) { return a + b; });

	RegisterBoost<1>("Boost: Sub", [](const auto& a, const auto& b) { return a - b; });
	RegisterGMP<1>("GMP: Sub", [](const auto& a, const auto& b) { return a - b; });

	RegisterBoost<1>("Boost: Mul", [](const auto& a, const auto& b) { return a * b; });
	RegisterGMP<1>("GMP: Mul", [](const auto& a, const auto& b) { return a * b; });

	RegisterBoost<2>("Boost: Div", [](const auto& a, const auto& b) { return a / b; });
	RegisterGMP<2>("GMP: Div", [](const auto& a, const auto& b) { return a / b; });

	RegisterBoost<1>("Boost: Sqrt", [](const auto& a, const auto&) { return sqrt(a); });
	RegisterBoost<1>("Karatsuba: Sqrt", [](const auto& a, const auto&) { return KarSqrt(a); });
	RegisterGMP<1>("GMP: Sqrt", [](const auto& a, const auto&) { return sqrt(a); });
}