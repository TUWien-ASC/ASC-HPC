#include <iostream>
#include <sstream>


#include <simd.hpp>

using namespace ASC_HPC;
using std::cout, std::endl;

auto func1 (SIMD<double> a, SIMD<double> b)
{
  return a+b;
}

auto func2 (SIMD<double,4> a, SIMD<double,4> b)
{
  return a+3*b;
}

auto func3 (SIMD<double,4> a, SIMD<double,4> b, SIMD<double,4> c)
{
  return fma(a,b,c);
}


auto load (double * p)
{
  return SIMD<double,2>(p);
}

auto loadMask(double *p, SIMD<mask64, 2> m)
{
  return SIMD<double,2>(p, m);
}

auto testSelect (SIMD<mask64,2> m, SIMD<double,2> a, SIMD<double,2> b)
{
  return select (m, a, b);
}

SIMD<double,2> testHSum (SIMD<double,4> a, SIMD<double,4> b)
{
  return hSum(a,b);
}



int main()
{
  SIMD<double,4> a(1.,2.,3.,4.);
  SIMD<double,4> b(5.,6.,7.,8.);
  SIMD<double,4> c(9.,10.,11.,12.);
  SIMD<double,4> d(13.,14.,15.,16.);

  SIMD<double,4> at, bt, ct, dt;
  transpose (a, b, c, d, at, bt, ct, dt);

  std::cout << "a: " << a << "\n";
  std::cout << "b: " << b << "\n";
  std::cout << "c: " << c << "\n";
  std::cout << "d: " << d << "\n";
  std::cout << "---- transposed ----" << "\n";
  std::cout << "at: " << at << "\n";
  std::cout << "bt: " << bt << "\n";
  std::cout << "ct: " << ct << "\n";
  std::cout << "dt: " << dt << "\n";
  
}
