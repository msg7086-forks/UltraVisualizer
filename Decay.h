#pragma once

#include <cstdint>

struct PixelRGBA
{
	std::uint8_t r, g, b, a;
};
static_assert(sizeof(PixelRGBA) == 4, "Decay kernels require packed RGBA pixels");

// Read from the full source frame and write only destination rows [begin, end).
using DecayRows = void(const PixelRGBA* src, PixelRGBA* dst, int width, int height, int begin, int end);
DecayRows* SelectDecayRows(int cpu_flags);
