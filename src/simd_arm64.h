#ifndef SIMD_ARM64_H
#define SIMD_ARM64_H

#include "arm_neon.h"

/*
  implementation of SIMDs for ARM-Neon CPUs:
  https://arm-software.github.io/acle/neon_intrinsics/advsimd.html

  // neon coding:
  https://developer.arm.com/documentation/102159/0400
*/


namespace ASC_HPC
{

  template<>
  class SIMD<mask64,2>
  {
    int64x2_t val;
  public:
    SIMD (int64x2_t _val) : val(_val) { };
    SIMD (SIMD<mask64,1> v0, SIMD<mask64,1> v1) : val{v0.Val(), v1.Val()} { }

    auto Val() const { return val; }
    mask64 operator[](size_t i) const { return ( (int64_t*)&val)[0] != 0; }

    SIMD<mask64, 1> Lo() const { return SIMD<mask64,1>(val[0]); }
    SIMD<mask64, 1> Hi() const { return SIMD<mask64,1>(val[1]); }
    const mask64 * Ptr() const { return (mask64*)&val; }
  };




  
  template<>
  class SIMD<double,2>
  {
    float64x2_t val;
  public:
    SIMD () = default;
    SIMD (const SIMD &) = default;
    SIMD (double _val) : val{_val,_val} { }
    SIMD (float64x2_t _val) : val(_val) { }
    SIMD (double v0, double v1) : val{vcombine_f64(float64x1_t{v0}, float64x1_t{v1})} { }
    SIMD (SIMD<double,1> v0, SIMD<double,1> v1) : SIMD(v0.Val(), v1.Val()) { }
    SIMD (std::array<double, 2> arr) : val{arr[0], arr[1]} { }

    SIMD (double const * p) : val{vld1q_f64(p)} { }
    SIMD (double const * p, SIMD<mask64,2> mask)
      {
	val[0] = mask[0] ? p[0] : 0;
	val[1] = mask[1] ? p[1] : 0;
      }

    static constexpr int Size() { return 2; }    
    auto Val() const { return val; }
    const double * Ptr() const { return (double*)&val; }

    auto Lo() const { return SIMD<double,1> (val[0]); }
    auto Hi() const { return SIMD<double,1> (val[1]); }
    double operator[] (int i) const { return val[i]; }

    void Store (double * p)
    {
      vst1q_f64(p, val);
    }
    
    void Store (double * p, SIMD<mask64,2> mask)
    {
      if (mask[0]) p[0] = val[0];
      if (mask[1]) p[1] = val[1];
    }
  };

  inline auto operator+ (SIMD<double,2> a, SIMD<double,2> b) { return SIMD<double,2> (a.Val()+b.Val()); }
  inline auto operator- (SIMD<double,2> a, SIMD<double,2> b) { return SIMD<double,2> (a.Val()-b.Val()); }
  
  inline auto operator* (SIMD<double,2> a, SIMD<double,2> b) { return SIMD<double,2> (a.Val()*b.Val()); }
  inline auto operator* (double a, SIMD<double,2> b) { return SIMD<double,2> (a*b.Val()); }    
  
  // a*b+c
  inline SIMD<double,2> FMA (SIMD<double,2> a, SIMD<double,2> b, SIMD<double,2> c) 
  { return vmlaq_f64(c.Val(), a.Val(), b.Val()); }



  inline SIMD<double,2> Select (SIMD<mask64,2> mask, SIMD<double,2> b, SIMD<double,2> c)
  { return vbslq_f64(mask.Val(), b.Val(), c.Val()); }
  
  inline SIMD<double,2> HSum (SIMD<double,2> a, SIMD<double,2> b)
  { return vpaddq_f64(a.Val(), b.Val()); }
  
}

#endif
