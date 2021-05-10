#include <benchmark/benchmark.h>
#include <boost/multiprecision/gmp.hpp>
#include "karatsuba.h"

template<typename T, size_t Left, size_t Right, typename F>
void BenchOper(benchmark::State &state, F f) {
    std::vector<T> a(10000);
    std::vector<T> b(10000);
	std::vector<T> res(10000);
    if constexpr (std::is_same<T, mpz_int>::value) {
	    {
            boost::random::independent_bits_engine<boost::random::mt19937, Left, mpz_int> gen;
            for (auto &v : a) {
                v = T(gen());
            }
        }
        {
            boost::random::independent_bits_engine<boost::random::mt19937, Right, mpz_int> gen;
            for (auto &v : b) {
                v = T(gen());
            }
        }
    }
    else {
        {
            boost::random::independent_bits_engine<boost::random::mt19937, Left, cpp_int> gen;
            for (auto &v : a) {
                v = T(gen());
            }
        }
        {
            boost::random::independent_bits_engine<boost::random::mt19937, Right, cpp_int> gen;
            for (auto &v : b) {
                v = T(gen());
            }
        }
    }
	size_t i = 0;
    for (auto _ : state) {
        res[i] = f(a[i], b[i]);
		if (++i >= res.size()) {
			i = 0;
		}
	}
	BENCHMARK_UNUSED(a);
	BENCHMARK_UNUSED(b);
	BENCHMARK_UNUSED(res);
}

template<typename T, size_t Left, size_t Right, typename F>
void RegisterOper(const std::string& testName, F f) {
    benchmark::RegisterBenchmark(testName.c_str(), [f](benchmark::State &state) {
        BenchOper<T, Left, Right, F>(state, f);
	});
}

template<typename T, size_t Left, size_t Right, typename F>
void RegisterOperPref(const std::string& prefix, F f) {
    RegisterOper<T, Left, Right, F>(prefix + " " + std::to_string(Left) + " " + std::to_string(Right), f);
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