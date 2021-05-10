#include <iostream>
#include "sqrt_bench.h"
#include "oper_bench.h"

int main(int argc, char **argv) {
	if (argc < 2) {
		std::cout << "Usage sqrt_bench (sqrt|oper) [bench args]" << std::endl;
		return 0;
	}
	if (std::string(argv[1]) == "sqrt") {
		RegisterSqrt();
	}
	else if (std::string(argv[1]) == "oper") {
		RegisterOper();
	}
	else {
		std::cout << "Usage sqrt_bench (sqrt|oper) [bench args]" << std::endl;
		return 0;
	}
	benchmark::Initialize(&argc, argv);
	benchmark::RunSpecifiedBenchmarks();
}
