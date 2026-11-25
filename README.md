# UltraVisualizer
Visualizer as translucent video overlay filter for Avisynth+, based on a preset for the Advanced Visualizer Studio for WinAmp

Made with the help of wonkey_monkey (doom9), Columbo (doom9), Gemini (Google), Claude (Anthropic), and Copilot (Microsoft)

Selur contributed an internally multi-threaded version with a pool of buffered overlay frames.

## Syntax
`visovl = base.UltraVis()` *# RGB24 in, RGB24 out; Prefetch(threads>1) will cause flickering*   \
`visovl = base.UltraVisMT(threads)` *# internal blur frame buffer avoids flickering*

## Blur and CPU support

`UltraVis()` and `UltraVisMT()` share the same trail blur. Each filter instance uses the CPU flags reported by AviSynth+ to select AVX2, SSE2, or scalar code at creation. To test a particular path, call `SetMaxCPU("sse2")` or `SetMaxCPU("none")` before creating the filter. Thread count is independent of this choice.

## Building with MSVC

Set `AVISYNTH_INCLUDE_DIR` to the AviSynth+ SDK directory containing `avisynth.h`. For a Visual Studio x64 build, run

```bat
cmake -S . -B build-x64 -A x64 -DAVISYNTH_INCLUDE_DIR="C:/path/to/FilterSDK/include"
cmake --build build-x64 --config Release
```

Use `-A Win32` and a separate build directory for 32-bit AviSynth+. Each build produces `UltraVisualizer.dll` and `UltraVisualizerMT.dll`. CMake applies `/arch:AVX2` only to the AVX2 kernel. Keep project-wide compiler settings at the baseline for the target architecture.

### Example videos on YouTube:

Richard Hoffmann: Dreams of Infinity
[![Richard Hoffmann • Dreams Of Infinity • ReBirth RB-338 DancePack 2 (1440p, 60 Hz)](https://github.com/user-attachments/assets/e570a56d-a253-48a9-8664-f7a469a27209)](https://www.youtube.com/watch?v=JDSTbye8MPQ)

Jeroen Tel: Stranglehold
[![Jeroen Tel (Maniacs of Noise) • Stranglehold (1440p, 60 Hz](https://github.com/user-attachments/assets/d37980d9-8cae-4b40-8311-0fd92733c8ea)](https://www.youtube.com/watch?v=wN-KTnshKbk)
