#pragma once

#include <tuple>
#include <cmath>
#include "simd.hpp"

namespace ASC_HPC
{

  // ==================  Polynomial coefficients  ==================

  static constexpr double ln2_hi = 0.693359375;            // high part of ln(2)
  static constexpr double ln2_lo = -2.12194440e-4;         // low part of ln(2)

  static constexpr double exp_coeff[] = {
      1.9875691500E-4,
      1.3981999507E-3,
      8.3334519073E-3,
      4.1665795894E-2,
      1.6666665459E-1,
      5.0000001201E-1
  };

  static constexpr double sincof[] = {
      1.58962301576546568060E-10,
     -2.50507477628578072866E-8,
      2.75573136213857245213E-6,
     -1.98412698295895385996E-4,
      8.33333333332211858878E-3,
     -1.66666666666666307295E-1,
  };

  static constexpr double coscof[] = {
     -1.13585365213876817300E-11,
      2.08757008419747316778E-9,
     -2.75573141792967388112E-7,
      2.48015872888517045348E-5,
     -1.38888888888730564116E-3,
      4.16666666666665929218E-2,
  };


  // ==================  sincos_reduced: |x| <= pi/4  ==================

  template<typename T>
  inline std::tuple<T,T> sincos_reduced(T x)
  {
      T x2 = x * x;

      // Polynomial approximation for sin on [-pi/4, pi/4]
      T s = (((((sincof[0] * x2 + sincof[1]) * x2
               + sincof[2]) * x2
               + sincof[3]) * x2
               + sincof[4]) * x2
               + sincof[5]);

      s = x + x * x2 * s;

      // Polynomial approximation for cos on [-pi/4, pi/4]
      T c = (((((coscof[0] * x2 + coscof[1]) * x2
               + coscof[2]) * x2
               + coscof[3]) * x2
               + coscof[4]) * x2
               + coscof[5]);

      c = T(1.0) - T(0.5) * x2 + x2 * x2 * c;

      return { s, c };
  }


  // ==================  scalar sincos (full range)  ==================

  inline std::tuple<double,double> sincos(double x)
  {
      // 1) how many (pi/2) fit into x?
      double y = std::round((2.0 / M_PI) * x);
      int    q = std::lround(y);

      // 2) reduce to [-pi/4, pi/4]
      auto [s1, c1] = sincos_reduced(x - y * (M_PI / 2.0));

      // 3) quadrant logic (q mod 4)
      double s2 = ((q & 1) == 0) ? s1 : c1;
      double s  = ((q & 2) == 0) ? s2 : -s2;

      double c2 = ((q & 1) == 0) ? c1 : -s1;
      double c  = ((q & 2) == 0) ? c2 : -c2;

      return { s, c };
  }


  // ==================  scalar sin / cos wrappers  ==================

  inline double sin(double x)
  {
      auto [s, c] = sincos(x);
      return s;
  }

  inline double cos(double x)
  {
      auto [s, c] = sincos(x);
      return c;
  }


  // ==================  SIMD sincos base case: S = 1  ==================

  inline std::tuple<SIMD<double,1>, SIMD<double,1>>
  sincos(SIMD<double,1> x)
  {
      double v = x[0];
      auto [s, c] = sincos(v);             // use scalar version
      return { SIMD<double,1>(s), SIMD<double,1>(c) };
  }


  // ==================  SIMD sincos recursive: general S  ==================

  template<size_t S>
  inline std::tuple<SIMD<double,S>, SIMD<double,S>>
  sincos(SIMD<double,S> x)
  {
      auto x_lo = x.lo();
      auto x_hi = x.hi();

      auto [s_lo, c_lo] = sincos(x_lo);
      auto [s_hi, c_hi] = sincos(x_hi);

      SIMD<double,S> s(s_lo, s_hi);
      SIMD<double,S> c(c_lo, c_hi);

      return { s, c };
  }

  // ==================  SIMD sin / cos wrappers  ==================

  template<size_t S>
  inline SIMD<double,S> sin(SIMD<double,S> x)
  {
      auto [s, c] = sincos(x);
      return s;
  }

  template<size_t S>
  inline SIMD<double,S> cos(SIMD<double,S> x)
  {
      auto [s, c] = sincos(x);
      return c;
  }

  template<typename T>
  T exp_reduced(T r)
  {
      T r2 = r * r;

      // Horner polynomial for exp(r) - 1
      T p = exp_coeff[0];
      p = p * r + exp_coeff[1];
      p = p * r + exp_coeff[2];
      p = p * r + exp_coeff[3];
      p = p * r + exp_coeff[4];
      p = p * r + exp_coeff[5];

      // exp(r) ≈ 1 + r + r² * p
      return T(1.0) + r + r2 * p;
  }

  inline double custom_exp(double x)
  {
      // reduce: x = n*ln2 + r
      double n_f = std::floor(x / ln2_hi + 0.5);
      int n = int(n_f);

      double r = x - n_f * ln2_hi - n_f * ln2_lo;

      double e = exp_reduced(r);

      // reconstruct: exp(x) = 2^n * exp(r)
      return std::ldexp(e, n);
  }

} // namespace ASC_HPC
