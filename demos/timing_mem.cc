// taskpolicy -a timing_mem & ; taskpolicy -a timing_mem & ;   taskpolicy -a timing_mem & ;  taskpolicy -a timing_mem > output.txt ; 

#include <iostream>
#include <sstream>
#include <chrono>


#include <simd.h>

using namespace ASC_HPC;
using namespace std;


auto SumIt (size_t n, SIMD<double,32> * data)
{
  SIMD<double,32> sum(0.0);
  for (size_t i = 0; i < n; i++)
    sum += data[i];
  return sum;
}

void Triade (size_t n, SIMD<double,16> * a, SIMD<double,16> * b, SIMD<double,16> * c)
{
  for (size_t i = 0; i < n; i++)
    c[i] += a[i]+b[i];
}


int main()
{
  SIMD<double,32> sum(0.0);

  if (true)
  for (size_t n = 1; n <= 2<<22; n*=2)
    {
      SIMD<double,32> * data = new SIMD<double,32>[n];

      for (size_t i = 0; i < n; i++)
        data[i] = SIMD<double,32>(i);
      
      size_t mem = n*sizeof(SIMD<double,32>);

      size_t runs = 1e10/mem+1;

      auto start = std::chrono::high_resolution_clock::now();
      
      for (size_t i = 0; i < runs; i++)
        sum += SumIt (n, data);
      
      auto end = std::chrono::high_resolution_clock::now();

      double time = std::chrono::duration<double, std::milli>(end-start).count();
        
      cout << "n = " << n << ", mem = " << mem/1000.0 << ", time = " << time 
           << " ms, GB/sec = " << (mem*runs)/time/1e6
           << endl;

      delete [] data;
    }
  cout << "sum = " << HSum(sum) << endl;

  if (false)
  for (size_t n = 1; n <= 2<<22; n*=2)
    {
      SIMD<double,16> * pa = new SIMD<double,16>[n];
      SIMD<double,16> * pb = new SIMD<double,16>[n];
      SIMD<double,16> * pc = new SIMD<double,16>[n];

      for (size_t i = 0; i < n; i++)
        {
          pa[i] = SIMD<double,16>(i);
          pb[i] = SIMD<double,16>(i);
        }
      
      size_t mem = 3*n*sizeof(SIMD<double,16>);

      size_t runs = 1e10/mem+1;

      auto start = std::chrono::high_resolution_clock::now();
      
      for (size_t i = 0; i < runs; i++)
        Triade (n, pa, pb, pc);
      
      auto end = std::chrono::high_resolution_clock::now();

      double time = std::chrono::duration<double, std::milli>(end-start).count();
        
      cout << "n = " << n << ", mem = " << mem << ", time = " << time 
           << " ms, GB/sec = " << (mem*runs)/time/1e6
           << endl;

      delete [] pc;
      delete [] pb;
      delete [] pa;
    }

  
}
