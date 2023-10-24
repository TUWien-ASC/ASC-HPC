#ifndef SIMD_AVX_H
#define SIMD_AVX_H

#include <immintrin.h>

namespace ASC_HPC
{
  
  
  template<>
  class SIMD<double,4>
  {
    __m256d val;
  public:
    SIMD () = default;
    SIMD (const SIMD &) = default;
    SIMD(double _val) : val{_mm256_set1_pd(_val)} {};
    SIMD(__m256d _val) : val{_val} {};
    SIMD (double v0, double v1, double v2, double v3) : val{_mm256_set_pd(v3,v2,v1,v0)} {  }
    SIMD (SIMD<double,2> v0, SIMD<double,2> v1) : SIMD(v0[0], v0[1], v1[0], v1[1]) { }  // can do better !
    SIMD (std::array<double,4> a) : SIMD(a[0],a[1],a[2],a[3]) { }
    static constexpr int Size() { return 4; }
    auto Val() const { return val; }
    const double * Ptr() const { return (double*)&val; }
    // SIMD<double, 2> Lo() const { return _mm256_extractf128_pd(val, 0); }
    // SIMD<double, 2> Hi() const { return _mm256_extractf128_pd(val, 1); }
    double operator[](size_t i) const { return val[i]; }
  };
  
  
  inline auto operator+ (SIMD<double,4> a, SIMD<double,4> b) { return SIMD<double,4> (_mm256_add_pd(a.Val(), b.Val())); }
  inline auto operator- (SIMD<double,4> a, SIMD<double,4> b) { return SIMD<double,4> (_mm256_sub_pd(a.Val(), b.Val())); }
  
  inline auto operator* (SIMD<double,4> a, SIMD<double,4> b) { return SIMD<double,4> (_mm256_mul_pd(a.Val(), b.Val())); }
  inline auto operator* (double a, SIMD<double,4> b) { return SIMD<double,4>(a)*b; }
  
#ifdef __FMA__
  inline SIMD<double,4> FMA (SIMD<double,4> a, SIMD<double,4> b, SIMD<double,4> c)
  { return _mm256_fmadd_pd (a.Val(), b.Val(), c.Val()); }
#endif
  
}

#endif
