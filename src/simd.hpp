#ifndef SIMD_HPP
#define SIMD_HPP

#include<iostream>
#include<string>
#include<memory>
#include <array>


namespace ASC_HPC
{


#ifdef __AVX__
  constexpr size_t DefaultSimdSizeBytes = 32;
#else
  constexpr size_t DefaultSimdSizeBytes = 16;
#endif

  
  template <typename T, size_t S = DefaultSimdSizeBytes/sizeof(T)> class SIMD;
  
  
  constexpr size_t largestPowerOfTwo (size_t x)
  {
    size_t y = 1;
    while (2*y <= x) y *= 2;
    return y;
  }

  
  class mask64
  {
    int64_t m_mask;
  public:
    mask64 (bool b)
      : m_mask{ b ? -1 : 0 } { }
    auto val() const { return m_mask; }
    operator bool() { return bool(m_mask); }
  };
  
  inline std::ostream & operator<< (std::ostream & ost, mask64 m)
  {
    ost << (m ? 't' : 'f');
    return ost;
  }

  namespace detail {
    template <typename T, size_t N, size_t... I>
    auto array_range_impl(std::array<T, N> const& arr, size_t first,
                          std::index_sequence<I...>) {
      return std::array<T, sizeof...(I)>{arr[first + I]...};
    }
    
    template <size_t FIRST, size_t NEXT, typename T, size_t N>
    auto array_range(std::array<T, N> const& arr) {
      return array_range_impl(arr, FIRST, std::make_index_sequence<NEXT-FIRST>{});
    }
  } // namespace detail

  
  
  template <typename T, size_t S>
  class SIMD
  {
  protected:
    static constexpr size_t S1 = largestPowerOfTwo(S-1);
    static constexpr size_t S2 = S-S1;

    SIMD<T,S1> m_lo;
    SIMD<T,S2> m_hi;
  public:
    SIMD() = default;

    explicit SIMD (T val)
      : m_lo(val), m_hi(val) { }

    explicit SIMD (SIMD<T,S1> lo, SIMD<T,S2> hi)
      : m_lo(lo), m_hi(hi) { }
    
    explicit SIMD (std::array<T, S> arr)
      : m_lo(detail::array_range<0, S1>(arr)),
        m_hi(detail::array_range<S1, S>(arr))
      {}

    template <typename ...T2>
    explicit SIMD (T val0, T2... vals)
      : SIMD(std::array<T, S>{val0, vals...}) { }

    explicit SIMD (T * ptr)
      : m_lo(ptr), m_hi(ptr+S1) { }
    
    explicit SIMD (T * ptr, SIMD<mask64,S> mask)
      : m_lo(ptr, mask.lo()), m_hi(ptr+S1, mask.hi()) { }
    
    
    static constexpr int size() { return S; }    
    auto & lo() { return m_lo; }
    auto & hi() { return m_hi; }

    const T * ptr() const { return m_lo.ptr(); }
    T operator[] (size_t i) const { return ptr()[i]; }

    void store (T * ptr) const {
      m_lo.store(ptr);
      m_hi.store(ptr+S1);
    }

    void store (T * ptr, SIMD<mask64,S> mask) const {
      m_lo.store(ptr, mask.lo());
      m_hi.store(ptr+S1, mask.hi());
    }
  };


  
  template <typename T>
  class SIMD<T,1>
  {
    T m_val;
  public:
    SIMD() = default;
    SIMD(T val) : m_val(val) { }
    SIMD(std::array<T,1> vals) : m_val(vals[0]) { }
    explicit SIMD (T * ptr) : m_val{*ptr} { } 

    auto val() const { return m_val; }
    
    explicit SIMD (T * ptr, SIMD<mask64,1> mask)
      : m_val{ mask.val() ? *ptr : T(0)} { }

    static constexpr size_t size() { return 1; }
    const T * ptr() const { return &m_val; }    
    T operator[] (size_t i) const { return m_val; }

    void store (T * ptr) const { *ptr = m_val; }
    void store (T * ptr, SIMD<mask64,1> mask) const { if (mask.val()) *ptr = m_val; }
  };


  template <typename T, size_t S>  
  std::ostream & operator<< (std::ostream & ost, SIMD<T,S> simd)
  {
    for (size_t i = 0; i < S-1; i++)
      ost << simd[i] << ", ";
    ost << simd[S-1];
    return ost;
  }

  // ********************** Arithmetic operations ********************************

  template <typename T, size_t S>
  auto operator+ (SIMD<T,S> a, SIMD<T,S> b) { return SIMD<T,S> (a.lo()+b.lo(), a.hi()+b.hi()); }
  template <typename T>
  auto operator+ (SIMD<T,1> a, SIMD<T,1> b) { return SIMD<T,1> (a.val()()+b.val()()); }


  template <typename T, size_t S>
  auto operator* (SIMD<T,S> a, SIMD<T,S> b) { return SIMD<T,S> (a.lo()*b.lo(), a.hi()*b.hi()); }
  template <typename T>
  auto operator* (SIMD<T,1> a, SIMD<T,1> b) { return SIMD<T,1> (a.val()()*b.val()()); }
  
  template <typename T, size_t S>
  auto operator* (double a, SIMD<T,S> b) { return SIMD<T,S> (a*b.lo(), a*b.hi()); }
  template <typename T>
  auto operator* (double a, SIMD<T,1> b) { return SIMD<T,1> (a*b.val()()); }

  template <typename T, size_t S>
  auto operator+= (SIMD<T,S> & a, SIMD<T,S> b) { a = a+b; return a; }
  
  template <typename T, size_t S>
  auto FMA(SIMD<T,S> a, SIMD<T,S> b, SIMD<T,S> c)
  { return SIMD<T,S> (FMA(a.lo(),b.lo(),c.lo()), FMA(a.hi(),b.hi(),c.hi())); }    
  template <typename T>
  auto FMA(SIMD<T,1> a, SIMD<T,1> b, SIMD<T,1> c)
  { return SIMD<T,1> (a.val()()*b.val()()+c.val()()); }



  // ****************** Horizontal sums *****************************
  
  template <typename T, size_t S>
  auto HSum (SIMD<T,S> a) { return HSum(a.lo())+HSum(a.hi()); }

  template <typename T>
  auto HSum (SIMD<T,1> a) { return a.val(); }
  
  
  template <typename T, size_t S>
  auto HSum (SIMD<T,S> a0, SIMD<T,S> a1)
  { return HSum(a0.lo(), a1.lo())+HSum(a0.hi(), a1.hi()); }

  template <typename T>
  auto HSum(SIMD<T,1> a0, SIMD<T,1> a1)
  { return SIMD<T,2> (a0.val()(), a1.val()()); }


  
  // ****************** Select   *************************************

  template <typename T>
  auto Select (SIMD<mask64,1> mask, SIMD<T,1> a, SIMD<T,1> b)
  { return mask.val() ? a : b; }
  
  template <typename T, size_t S>
  auto Select (SIMD<mask64,S> mask, SIMD<T,S> a, SIMD<T,S> b)
  { return SIMD<T,S> (Select (mask.lo(), a.lo(), b.lo()),
                      Select (mask.hi(), a.hi(), b.hi())); }

  // ****************** IndexSequence ********************************
  
  template <typename T, size_t S, T first=0>
  class IndexSequence : public SIMD<T,S>
  {
    using SIMD<T,S>::S1;
    using SIMD<T,S>::S2;
  public:
    IndexSequence()
      : SIMD<T,S> (IndexSequence<T,S1,first>(),
                   IndexSequence<T,S2,first+S1>()) { }
  };

  template <typename T, T first>
  class IndexSequence<T,1,first> : public SIMD<T,1>
  {
  public:
    IndexSequence() : SIMD<T,1> (first) { }
  };


  template <typename T, size_t S>
  auto operator>= (SIMD<T,S> a, SIMD<T,S> b)
  { return SIMD<mask64,S>(a.lo()>=b.lo(), a.hi()>=b.hi()); }

  template <typename T>
  auto operator>= (SIMD<T,1> a, SIMD<T,1> b)
  { return SIMD<mask64,1>(a.val()>=b.val()); }

  template <typename TA, typename T, size_t S>
  auto operator>= (TA a, const SIMD<T,S> & b)
  { return SIMD<T,S>(a) >= b; }
  
}
  
  
#ifdef __AVX__
#include "simd_avx.hpp"
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#include "simd_arm64.hpp"
#endif



#endif
