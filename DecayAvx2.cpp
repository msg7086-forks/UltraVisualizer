#include "DecayInternal.h"
#include <immintrin.h>

namespace {
__m256i BlendSums(const __m256i& near_sum, const __m256i& far_sum)
{
	const __m256i alpha_mask = _mm256_set_epi16(-1, 0, 0, 0, -1, 0, 0, 0, -1, 0, 0, 0, -1, 0, 0, 0);
	const __m256i rgb = _mm256_add_epi16(_mm256_srli_epi16(near_sum, 3), _mm256_srli_epi16(far_sum, 3));
	const __m256i near_alpha = _mm256_srli_epi16(_mm256_mulhi_epu16(near_sum, _mm256_set1_epi16(-13107)), 3);
	const __m256i far_alpha = _mm256_srli_epi16(_mm256_mulhi_epu16(far_sum, _mm256_set1_epi16(-21845)), 3);
	return _mm256_or_si256(_mm256_andnot_si256(alpha_mask, rgb),
		_mm256_and_si256(alpha_mask, _mm256_add_epi16(near_alpha, far_alpha)));
}

void DecayBlock(const DecayNeighbors& rows, int x, PixelRGBA* out)
{
	const __m256i zero_avx2 = _mm256_setzero_si256();
	const __m256i top = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(rows.up1 + x));
	const __m256i bottom = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(rows.down1 + x));
	const __m256i left = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(rows.center + x - 1));
	const __m256i right = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(rows.center + x + 1));
	const __m256i near_lo = _mm256_add_epi16(
		_mm256_add_epi16(_mm256_unpacklo_epi8(top, zero_avx2), _mm256_unpacklo_epi8(bottom, zero_avx2)),
		_mm256_add_epi16(_mm256_unpacklo_epi8(left, zero_avx2), _mm256_unpacklo_epi8(right, zero_avx2)));
	const __m256i near_hi = _mm256_add_epi16(
		_mm256_add_epi16(_mm256_unpackhi_epi8(top, zero_avx2), _mm256_unpackhi_epi8(bottom, zero_avx2)),
		_mm256_add_epi16(_mm256_unpackhi_epi8(left, zero_avx2), _mm256_unpackhi_epi8(right, zero_avx2)));
	const __m256i top2 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(rows.up2 + x));
	const __m256i bottom2 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(rows.down2 + x));
	const __m256i left2 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(rows.center + x - 2));
	const __m256i right2 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(rows.center + x + 2));
	const __m256i far_lo = _mm256_add_epi16(
		_mm256_add_epi16(_mm256_unpacklo_epi8(top2, zero_avx2), _mm256_unpacklo_epi8(bottom2, zero_avx2)),
		_mm256_add_epi16(_mm256_unpacklo_epi8(left2, zero_avx2), _mm256_unpacklo_epi8(right2, zero_avx2)));
	const __m256i far_hi = _mm256_add_epi16(
		_mm256_add_epi16(_mm256_unpackhi_epi8(top2, zero_avx2), _mm256_unpackhi_epi8(bottom2, zero_avx2)),
		_mm256_add_epi16(_mm256_unpackhi_epi8(left2, zero_avx2), _mm256_unpackhi_epi8(right2, zero_avx2)));
	_mm256_storeu_si256(reinterpret_cast<__m256i*>(out),
		_mm256_packus_epi16(BlendSums(near_lo, far_lo), BlendSums(near_hi, far_hi)));
}
} // namespace

void DecayRowsAvx2(const PixelRGBA* src, PixelRGBA* dst, int width, int height, int begin, int end)
{
	DecayRange<8, DecayBlock>(src, dst, width, height, begin, end);
}
