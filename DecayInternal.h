#pragma once

#include "Decay.h"
#include <algorithm>
#include <cstddef>

DecayRows DecayRowsSse2;
DecayRows DecayRowsAvx2;

// Give each translation unit its own traversal so its ISA stays local to that kernel.
namespace {
struct DecayNeighbors
{
	const PixelRGBA *center, *up1, *down1, *up2, *down2;
};

PixelRGBA DecayPixel(const DecayNeighbors& rows, int x, int left1, int right1, int left2, int right2)
{
	auto channel = [&](std::uint8_t PixelRGBA::* value, int near_divisor, int far_divisor) {
		const int near_sum = rows.up1[x].*value + rows.down1[x].*value
			+ rows.center[left1].*value + rows.center[right1].*value;
		const int far_sum = rows.up2[x].*value + rows.down2[x].*value
			+ rows.center[left2].*value + rows.center[right2].*value;
		return static_cast<std::uint8_t>(near_sum / near_divisor + far_sum / far_divisor);
	};
	return { channel(&PixelRGBA::r, 8, 8), channel(&PixelRGBA::g, 8, 8),
		channel(&PixelRGBA::b, 8, 8), channel(&PixelRGBA::a, 10, 12) };
}

template<int block_width, auto block>
void DecayRange(const PixelRGBA* src, PixelRGBA* dst, int width, int height, int begin, int end)
{
	for (int y = begin; y < end; ++y) {
		auto row = [&](int index) { return src + static_cast<std::size_t>(index) * width; };
		const DecayNeighbors rows{ row(y), row((std::max)(y - 1, 0)), row((std::min)(y + 1, height - 1)),
			row((std::max)(y - 2, 0)), row((std::min)(y + 2, height - 1)) };
		PixelRGBA* out = dst + static_cast<std::size_t>(y) * width;
		auto edge = [&](int x) {
			return DecayPixel(rows, x, (std::max)(x - 1, 0), (std::min)(x + 1, width - 1),
				(std::max)(x - 2, 0), (std::min)(x + 2, width - 1));
		};
		int x = 0;
		for (; x < (std::min)(2, width); ++x)
			out[x] = edge(x);
		const int inner_end = width - 2;
		for (; x <= inner_end - block_width; x += block_width)
			block(rows, x, out + x);
		for (; x < inner_end; ++x)
			out[x] = DecayPixel(rows, x, x - 1, x + 1, x - 2, x + 2);
		for (; x < width; ++x)
			out[x] = edge(x);
	}
}
} // namespace
