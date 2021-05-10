#include <benchmark/benchmark.h>
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

template <size_t Bits, size_t Length, template <typename> typename Sqrt>
void RegisterOne(const std::string &name) {
	if (Bits < Length) {
		return;
	}
	std::string testName = name + "_" + std::to_string(Bits) + "_" + std::to_string(Length);
	benchmark::RegisterBenchmark(testName.c_str(), [](benchmark::State &state) {
		BenchSqrt<Bits, Length, Sqrt>(state);
	});
}

template <template <typename> typename Sqrt, size_t V, size_t... T>
void RegisterIterInner(const std::string &name) {
	(RegisterOne<V, T, Sqrt>(name), ...);
}

template <template <typename> typename Sqrt, size_t... T>
void RegisterIter(const std::string &name) {
	(RegisterIterInner<Sqrt, T, T...>(name), ...);
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

void RegisterSqrt() {
    Register<CopyBaseline>("CopyBaseline");
    Register<BoostSqrt>("BoostSqrt");
    Register<NewtonSqrt>("NewtonSqrt");
    Register<KaratsubaSqrt>("KaratsubaSqrt");
}