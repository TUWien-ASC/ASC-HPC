#ifndef SIMD_ARM64_H
#define SIMD_ARM64_H

#include "arm_neon.h"

namespace ASC_HPC
{

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

    auto Val() const { return val; }
    const double * Ptr() const { return (double*)&val; }

    auto Lo() const { return val[0]; }
    auto Hi() const { return val[1]; }
  };

  inline auto operator+ (SIMD<double,2> a, SIMD<double,2> b) { std::cout<<"m1 add" << std::endl; return SIMD<double,2> (a.Val()+b.Val()); }
  inline auto operator- (SIMD<double,2> a, SIMD<double,2> b) { return SIMD<double,2> (a.Val()-b.Val()); }
  
  inline auto operator* (SIMD<double,2> a, SIMD<double,2> b) { return SIMD<double,2> (a.Val()*b.Val()); }
  inline auto operator* (double a, SIMD<double,2> b) { return SIMD<double,2> (a*b.Val()); }    
  
  // a*b+c
  inline SIMD<double,2> FMA (SIMD<double,2> a, SIMD<double,2> b, SIMD<double,2> c) 
  { return vmlaq_f64(c.Val(), a.Val(), b.Val()); }
  
  
}

#endif
