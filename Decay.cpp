#include "DecayInternal.h"
#include "avs/cpuid.h"

namespace {
void DecayBlock(const DecayNeighbors& rows, int x, PixelRGBA* out)
{
	*out = DecayPixel(rows, x, x - 1, x + 1, x - 2, x + 2);
}

void DecayRowsScalar(const PixelRGBA* src, PixelRGBA* dst, int width, int height, int begin, int end)
{
	DecayRange<1, DecayBlock>(src, dst, width, height, begin, end);
}
} // namespace

DecayRows* SelectDecayRows(int cpu_flags)
{
#if defined(ULTRAVIS_X86_KERNELS)
	if (cpu_flags & CPUF_AVX2)
		return DecayRowsAvx2;
	if (cpu_flags & CPUF_SSE2)
		return DecayRowsSse2;
#endif
	return DecayRowsScalar;
}
