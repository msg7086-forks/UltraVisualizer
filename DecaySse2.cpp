#include "DecayInternal.h"
#include <emmintrin.h>

namespace {
__m128i BlendSums(const __m128i& near_sum, const __m128i& far_sum)
{
	const __m128i alpha_mask = _mm_set_epi16(-1, 0, 0, 0, -1, 0, 0, 0);
	// Preserve the separate integer truncation of the two four-neighbor sums.
	const __m128i rgb = _mm_add_epi16(_mm_srli_epi16(near_sum, 3), _mm_srli_epi16(far_sum, 3));
	// Unsigned multiply-high by 0xCCCD / 0xAAAB, then >> 3: exact / 10 and / 12.
	const __m128i near_alpha = _mm_srli_epi16(_mm_mulhi_epu16(near_sum, _mm_set1_epi16(-13107)), 3);
	const __m128i far_alpha = _mm_srli_epi16(_mm_mulhi_epu16(far_sum, _mm_set1_epi16(-21845)), 3);
	return _mm_or_si128(_mm_andnot_si128(alpha_mask, rgb),
		_mm_and_si128(alpha_mask, _mm_add_epi16(near_alpha, far_alpha)));
}

void DecayBlock(const DecayNeighbors& rows, int x, PixelRGBA* out)
{
	const __m128i zero = _mm_setzero_si128();
	const __m128i top = _mm_loadu_si128(reinterpret_cast<const __m128i*>(rows.up1 + x));
	const __m128i bottom = _mm_loadu_si128(reinterpret_cast<const __m128i*>(rows.down1 + x));
	const __m128i left = _mm_loadu_si128(reinterpret_cast<const __m128i*>(rows.center + x - 1));
	const __m128i right = _mm_loadu_si128(reinterpret_cast<const __m128i*>(rows.center + x + 1));
	const __m128i near_lo = _mm_add_epi16(
		_mm_add_epi16(_mm_unpacklo_epi8(top, zero), _mm_unpacklo_epi8(bottom, zero)),
		_mm_add_epi16(_mm_unpacklo_epi8(left, zero), _mm_unpacklo_epi8(right, zero)));
	const __m128i near_hi = _mm_add_epi16(
		_mm_add_epi16(_mm_unpackhi_epi8(top, zero), _mm_unpackhi_epi8(bottom, zero)),
		_mm_add_epi16(_mm_unpackhi_epi8(left, zero), _mm_unpackhi_epi8(right, zero)));
	const __m128i top2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(rows.up2 + x));
	const __m128i bottom2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(rows.down2 + x));
	const __m128i left2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(rows.center + x - 2));
	const __m128i right2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(rows.center + x + 2));
	const __m128i far_lo = _mm_add_epi16(
		_mm_add_epi16(_mm_unpacklo_epi8(top2, zero), _mm_unpacklo_epi8(bottom2, zero)),
		_mm_add_epi16(_mm_unpacklo_epi8(left2, zero), _mm_unpacklo_epi8(right2, zero)));
	const __m128i far_hi = _mm_add_epi16(
		_mm_add_epi16(_mm_unpackhi_epi8(top2, zero), _mm_unpackhi_epi8(bottom2, zero)),
		_mm_add_epi16(_mm_unpackhi_epi8(left2, zero), _mm_unpackhi_epi8(right2, zero)));
	_mm_storeu_si128(reinterpret_cast<__m128i*>(out),
		_mm_packus_epi16(BlendSums(near_lo, far_lo), BlendSums(near_hi, far_hi)));
}
} // namespace

void DecayRowsSse2(const PixelRGBA* src, PixelRGBA* dst, int width, int height, int begin, int end)
{
	DecayRange<4, DecayBlock>(src, dst, width, height, begin, end);
}
