#include "simd.hpp"
#include "simd_math.hpp"

using namespace ASC_HPC;

int main() {
    double x = 1.0;
    SIMD<double,4> v(1.0);

    auto [s, c] = sincos(x);

    std::cout << "scalar sin(1.0) = " << s << "\n";
    std::cout << "scalar cos(1.0) = " << c << "\n";

    std::cout << custom_exp(x) << " vs " << std::exp(x) << "\n";

    auto ve = custom_exp(v);
    std::cout << "SIMD exp(1.0,1.0,1.0,1.0) = " << ve << "\n";
}
