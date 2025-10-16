#ifndef SIMD_AVX_HPP
#define SIMD_AVX_HPP

#include <immintrin.h>


/*
  implementation of SIMDs for Intel-CPUs with AVX support:
  https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html
 */


namespace ASC_HPC
{

  template<>
  class SIMD<mask64,4>
  {
    __m256i m_mask;
  public:

    SIMD (__m256i mask) : m_mask(mask) { };
    SIMD (__m256d mask) : m_mask(_mm256_castpd_si256(mask)) { ; }
    auto val() const { return m_mask; }
    mask64 operator[](size_t i) const { return ( (int64_t*)&m_mask)[i] != 0; }
    
    SIMD<mask64, 2> lo() const { return SIMD<mask64,2>((*this)[0], (*this)[1]); }
    SIMD<mask64, 2> hi() const { return SIMD<mask64,2>((*this)[2], (*this)[3]); }
  };


  
  template<>
  class SIMD<double,4>
  {
    __m256d m_val;
  public:
    SIMD () = default;
    SIMD (const SIMD &) = default;
    SIMD(double val) : m_val{_mm256_set1_pd(val)} {};
    SIMD(__m256d val) : m_val{val} {};
    SIMD (double v0, double v1, double v2, double v3) : m_val{_mm256_set_pd(v3,v2,v1,v0)} {  }
    SIMD (SIMD<double,2> v0, SIMD<double,2> v1) : SIMD(v0[0], v0[1], v1[0], v1[1]) { }  // better with _mm256_set_m128d
    SIMD (std::array<double,4> a) : SIMD(a[0],a[1],a[2],a[3]) { }
    SIMD (double const * p) { m_val = _mm256_loadu_pd(p); }
    SIMD (double const * p, SIMD<mask64,4> mask) { m_val = _mm256_maskload_pd(p, mask.val()); }
    
    static constexpr int size() { return 4; }
    auto val() const { return val; }
    const double * ptr() const { return (double*)&val; }
    SIMD<double, 2> lo() const { return SIMD<double,2>((*this)[0], (*this)[1]); }
    SIMD<double, 2> hi() const { return SIMD<double,2>((*this)[2], (*this)[3]); }

    // better:
    // SIMD<double, 2> lo() const { return _mm256_extractf128_pd(m_val, 0); }
    // SIMD<double, 2> hi() const { return _mm256_extractf128_pd(m)val, 1); }
    double operator[](size_t i) const { return ((double*)&m_val)[i]; }

    void store (double * p) const { _mm256_storeu_pd(p, m_val); }
    void store (double * p, SIMD<mask64,4> mask) const { _mm256_maskstore_pd(p, mask.val(), m_val); }
  };
  


  
  template<>
  class SIMD<int64_t,4>
  {
    __m256i val;
  public:
    SIMD () = default;
    SIMD (const SIMD &) = default;
    SIMD(int64_t _val) : val{_mm256_set1_epi64x(_val)} {};
    SIMD(__m256i _val) : val{_val} {};
    SIMD (int64_t v0, int64_t v1, int64_t v2, int64_t v3) : val{_mm256_set_epi64x(v3,v2,v1,v0) } { } 
    SIMD (SIMD<int64_t,2> v0, SIMD<int64_t,2> v1) : SIMD(v0[0], v0[1], v1[0], v1[1]) { }  // can do better !
    // SIMD (std::array<double,4> a) : SIMD(a[0],a[1],a[2],a[3]) { }
    // SIMD (double const * p) { val = _mm256_loadu_pd(p); }
    // SIMD (double const * p, SIMD<mask64,4> mask) { val = _mm256_maskload_pd(p, mask.val()); }
    
    static constexpr int size() { return 4; }
    auto val() const { return val; }
    // const double * Ptr() const { return (double*)&val; }
    // SIMD<double, 2> Lo() const { return _mm256_extractf128_pd(val, 0); }
    // SIMD<double, 2> Hi() const { return _mm256_extractf128_pd(val, 1); }
    int64_t operator[](size_t i) const { return ((int64_t*)&val)[i]; }
  };
  


  template <int64_t first>
  class IndexSequence<int64_t, 4, first> : public SIMD<int64_t,4>
  {
  public:
    IndexSequence()
      : SIMD<int64_t,4> (first, first+1, first+2, first+3) { }
  };
  


  
  inline auto operator+ (SIMD<double,4> a, SIMD<double,4> b) { return SIMD<double,4> (_mm256_add_pd(a.val(), b.val())); }
  inline auto operator- (SIMD<double,4> a, SIMD<double,4> b) { return SIMD<double,4> (_mm256_sub_pd(a.val(), b.val())); }
  
  inline auto operator* (SIMD<double,4> a, SIMD<double,4> b) { return SIMD<double,4> (_mm256_mul_pd(a.val(), b.val())); }
  inline auto operator* (double a, SIMD<double,4> b) { return SIMD<double,4>(a)*b; }
  
#ifdef __FMA__
  inline SIMD<double,4> FMA (SIMD<double,4> a, SIMD<double,4> b, SIMD<double,4> c)
  { return _mm256_fmadd_pd (a.val(), b.val(), c.val()); }
#endif

  inline SIMD<mask64,4> operator>= (SIMD<int64_t,4> a , SIMD<int64_t,4> b)
  { // there is no a>=b, so we return !(b>a)
    return  _mm256_xor_si256(_mm256_cmpgt_epi64(b.val(),a.val()),_mm256_set1_epi32(-1)); }
  
  inline auto operator>= (SIMD<double,4> a, SIMD<double,4> b)
  { return SIMD<mask64,4>(_mm256_cmp_pd (a.val(), b.val(), _CMP_GE_OQ)); }
  

  
}

#endif
