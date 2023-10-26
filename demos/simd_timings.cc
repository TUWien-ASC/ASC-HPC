#include <iostream>
#include <sstream>
#include <chrono>


#include <simd.h>

using namespace ASC_HPC;
using namespace std;



// daxpy: y += alpha * x
void daxpy (size_t n, double * px, double * py, double alpha)
{
  SIMD<double> simd_alpha(alpha);
  for (size_t i = 0; i < n; i += SIMD<double>::Size())
    {
      SIMD<double> yi(py+i);
      // yi += simd_alpha * SIMD<double> (px+i);
      yi = FMA(simd_alpha, SIMD<double> (px+i), yi);
      yi.Store(py+i);
    }
}



// multi-daxpy:
// y0 += alpha00 * x0 + alpha01 * x1
// y1 += alpha10 * x0 + alpha11 * x1
void daxpy2x2 (size_t n, double * px0, double * px1,
               double * py0, double * py1, double alpha00, double alpha01, double alpha10, double alpha11)
{
  SIMD<double> simd_alpha00(alpha00);
  SIMD<double> simd_alpha01(alpha01);
  SIMD<double> simd_alpha10(alpha10);
  SIMD<double> simd_alpha11(alpha11);  
  
  for (size_t i = 0; i < n; i += SIMD<double>::Size())
    {
      SIMD<double> xi0(px0+i);
      SIMD<double> xi1(px1+i);
      // (SIMD<double>(py0+i)+simd_alpha00*xi0+simd_alpha01*xi1).Store(py0+i);
      // (SIMD<double>(py1+i)+simd_alpha10*xi0+simd_alpha11*xi1).Store(py1+i);

      SIMD<double> yi0(py0+i);
      yi0 = FMA(simd_alpha00, xi0, yi0);
      yi0 = FMA(simd_alpha01, xi1, yi0);
      yi0.Store(py0+i);
      
      SIMD<double> yi1(py1+i);
      yi1 = FMA(simd_alpha10, xi0, yi1);
      yi1 = FMA(simd_alpha11, xi1, yi1);
      yi1.Store(py1+i);
    }
}


// Inner product
// x ... double vector
// y ... simd vector, with dy distance in doubles
template <size_t SW>
auto InnerProduct (size_t n, double * px, double * py, size_t dy)
{
  SIMD<double,SW> sum;
  for (size_t i = 0; i < n; i++)
    {
      // sum += px[i] * SIMD<double,SW>(py+i*dy);
      sum = FMA(SIMD<double,SW>(px[i]), SIMD<double,SW>(py+i*dy), sum);
    }
  return sum;
}


auto InnerProduct8 (size_t n, double * px, double * py, size_t dy)
{
  return InnerProduct<8> (n, px, py, dy);
}


int main()
{

  cout << "timing daxpy" << endl;  
  for (size_t n = 16; n <= 1024; n*= 2)
    {
      double * px = new double[n];
      double * py = new double[n];
      for (size_t i = 0; i < n; i++)
        {
          px[i] = i;
          py[i] = 2;
        }
  
      auto start = std::chrono::high_resolution_clock::now();

      size_t runs = size_t (1e8 / n) + 1;

      for (size_t i = 0; i < runs; i++)
        daxpy (n, px, py, 2.8);
      
      auto end = std::chrono::high_resolution_clock::now();
      double time = std::chrono::duration<double, std::milli>(end-start).count();
        
      cout << "n = " << n << ", time = " << time 
           << " ms, GFlops = " << (n*runs)/time/1e6
           << endl;
      
      delete [] py;
      delete [] px;
    }


  cout << "timing daxpy 2x2" << endl;
  for (size_t n = 16; n <= 1024; n*= 2)
    {
      double * px0 = new double[n];
      double * py0 = new double[n];
      double * px1 = new double[n];
      double * py1 = new double[n];
      for (size_t i = 0; i < n; i++)
        {
          px0[i] = i;
          py0[i] = 2;
          px1[i] = i;
          py1[i] = 2;
        }
  
      auto start = std::chrono::high_resolution_clock::now();

      size_t runs = size_t (1e8 / (4*n)) + 1;

      for (size_t i = 0; i < runs; i++)
        daxpy2x2 (n, px0, px1, py0, py1, 0.4, 1.3, 2.5, 2.8);
      
      auto end = std::chrono::high_resolution_clock::now();
      double time = std::chrono::duration<double, std::milli>(end-start).count();
        
      cout << "n = " << n << "time = " << time 
           << " ms, GFlops = " << (4*n*runs)/time/1e6
           << endl;
      
      delete [] py0;
      delete [] px0;
      delete [] py1;
      delete [] px1;
    }

  

  cout << "timing inner product" << endl;
  constexpr size_t SW=8;
  for (size_t n = 16; n <= 1024; n*= 2)
    {
      double * px = new double[n];
      double * py = new double[n*SIMD<double,SW>::Size()];
      for (size_t i = 0; i < n; i++)
        px[i] = i;
      for (size_t i = 0; i < n*SIMD<double,SW>::Size(); i++)
        py[i] = 2;
  
      auto start = std::chrono::high_resolution_clock::now();

      size_t runs = size_t (1e8 / (n*SIMD<double,SW>::Size())) + 1;

      SIMD<double,SW> sum{0.0};
      extern int myvar; myvar++;
      for (size_t i = 0; i < runs; i++)
        sum += InnerProduct<SW> (n, px, py, SIMD<double,SW>::Size());
      
      auto end = std::chrono::high_resolution_clock::now();
      double time = std::chrono::duration<double, std::milli>(end-start).count();
      cout << "sum = " << sum;
      cout << ", n = " << n << ", time = " << time 
           << " ms, GFlops = " << (SIMD<double,SW>::Size()*n*runs)/time/1e6
           << endl;
      
      delete [] py;
      delete [] px;
    }
  
}
