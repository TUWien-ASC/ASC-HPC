#include <iostream>
#include <sstream>


#include <simd.hpp>
#include <simd_bitonic.hpp>

using namespace ASC_HPC;
using std::cout, std::endl;




int main()
{
    SIMD<double,5> a(2., 15., 3., 1., 5.);
    SIMD<double,5> b(13., 7., 8., 4., 6.);
    SIMD<double,5> c(6., 12., 5., 9., 10.);
    SIMD<double,5> d(10., 14., 11., 0., 12.);
    SIMD<double,5>* arr = new SIMD<double,5>[4]{a,b,c,d};

    BitonicSort<true>(arr, 4);

    for (size_t i = 0; i < 4; i++)
      cout << "arr[" << i << "] = " << arr[i] << "\n";

    delete[] arr; 
}
