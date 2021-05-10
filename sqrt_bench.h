#include <benchmark/benchmark.h>
#include <boost/multiprecision/gmp.hpp>
#include <boost/random.hpp>
#include "newton.h"
#include "karatsuba.h"

template <size_t Bits, size_t Length, template <typename> typename Sqrt>
void BenchSqrt(benchmark::State &state) {
	std::vector<tInt<Bits>> vec(10000);
	std::vector<tInt<Bits>> res(10000);
	boost::random::independent_bits_engine<boost::random::mt19937, Length, cpp_int> gen;
	for (auto &v : vec) {
		v = tInt<Bits>(gen());
	}
	size_t i = 0;
	for (auto _ : state) {
		res[i] = Sqrt<tInt<Bits>>().Sqrt(vec[i]);
		if (++i >= vec.size()) {
			i = 0;
		}
	}
	BENCHMARK_UNUSED(res);
	BENCHMARK_UNUSED(vec);
}

template <typename T, size_t Length, typename F>
void BenchArbitrarySqrt(benchmark::State &state, F f) {
	std::vector<T> vec(10000);
	std::vector<T> res(10000);
    if constexpr (std::is_same<T, mpz_int>::value) {
	    FillRandom<T, mpz_int, Length>(vec);
    }
    else {
	    FillRandom<T, cpp_int, Length>(vec);
    }
	size_t i = 0;
	for (auto _ : state) {
		res[i] = f(vec[i]);
		if (++i >= vec.size()) {
			i = 0;
		}
	}
}

template <typename T, size_t Length, typename F>
void RegisterArbitraryOne(const std::string &name, F f) {
	std::string testName = name + "_" + std::to_string(Length);
	benchmark::RegisterBenchmark(testName.c_str(), [f](benchmark::State &state) {
		BenchArbitrarySqrt<T, Length, F>(state, f);
	});
}

template <typename T, typename F, size_t... Lengths>
void RegisterArbitraryIter(const std::string &name, F f) {
	(RegisterArbitraryOne<T, Lengths, F>(name, f), ...);
}

template <typename T, typename F>
void RegisterArbitrary(const std::string &name, F f) {
    RegisterArbitraryIter<T, F, 32, 64, 96, 128, 256, 512, 1024, 8192, 65536>(name, f);
}

template <size_t Bits, size_t Length, template <typename> typename Sqrt>
void RegisterOne(const std::string &name) {
	std::string testName = name + "_" + std::to_string(Bits) + "_" + std::to_string(Length);
	benchmark::RegisterBenchmark(testName.c_str(), [](benchmark::State &state) {
		BenchSqrt<Bits, Length, Sqrt>(state);
	});
}

template <size_t Bits, template <typename> typename Sqrt>
void RegisterSingle(const std::string &name) {
    if (Bits > 32) {
        RegisterOne<Bits, Bits / 2, Sqrt>(name);
    }
    RegisterOne<Bits, Bits, Sqrt>(name);
}

template <template <typename> typename Sqrt, size_t... T>
void RegisterIter(const std::string &name) {
	(RegisterSingle<T, Sqrt>(name), ...);
}

template <template <typename> typename Sqrt>
void Register(const std::string &name) {
	RegisterIter<Sqrt, 32, 64, 96, 128, 256, 512, 1024, 8192, 65536>(name);
}

template <typename T>
struct BoostSqrt {
	T Sqrt(const T &v) {
		return boost::multiprecision::sqrt(v);
	}
};

template <typename T>
struct CopyBaseline {
	T Sqrt(const T &v) {
		return v;
	}
};

template<template<typename> typename Sqrt, typename T>
T CallSqrt(const T& t) {
    return Sqrt<T>().Sqrt(t);
}

void RegisterSqrt() {
    Register<CopyBaseline>("CopyBaseline");
    Register<BoostSqrt>("BoostSqrt");
    Register<NewtonSqrt>("NewtonSqrt");
    Register<KaratsubaSqrt>("KaratsubaSqrt");
    RegisterArbitrary<cpp_int>("KaratsubaArbitrarySqrt", [](const auto& v) { return CallSqrt<KaratsubaSqrt>(v); });
    RegisterArbitrary<mpz_int>("GMPArbitrarySqrt", [](const auto& v) { return sqrt(v); });
}
