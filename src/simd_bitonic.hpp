#ifndef SIMD_BITONIC_HPP
#define SIMD_BITONIC_HPP

#include <immintrin.h>


namespace ASC_HPC
{
	// ---- Bitonic merge over an array of SIMD registers (lane-wise) ----
	template<bool UP, typename T, size_t S>
	inline void BitonicMerge(SIMD<T,S>* a, size_t n)
	{
		if (n <= 1) return;

		const size_t n2 = n >> 1;

		// Pairwise compare–exchange between the first and second halves
		for (size_t i = 0; i < n2; ++i) {
			// Lane-wise min/max between the two SIMD registers
			auto mi = min(a[i], a[i + n2]);
			auto ma = max(a[i], a[i + n2]);

			if constexpr (UP) {
				a[i]       = mi;  // keep smaller in the left half
				a[i + n2]  = ma;  // larger in the right half
			} else {
				a[i]       = ma;  // descending: larger on the left
				a[i + n2]  = mi;  // smaller on the right
			}
		}

		// Recursively merge both halves
		BitonicMerge<UP>(a,       n2);
		BitonicMerge<UP>(a + n2,  n2);
	}

	// ---- Bitonic sort over an array of SIMDs (lane-wise, requires that S is a power of 2 for fast execution) ----
	template<bool UP, typename T, size_t S> // UP: ascending order
	inline void BitonicSort(SIMD<T,S>* a, size_t n) {
		if (n <= 1) return;

		const size_t n2 = n >> 1;

		// Sort first half in UP direction, second half in the opposite direction
		BitonicSort<UP>(a, n2);
		BitonicSort<!UP>(a + n2, n2);

		// Merge into one bitonic sequence in UP direction
		BitonicMerge<UP>(a, n);
	}

}
#endif
