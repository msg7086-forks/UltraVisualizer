cmake_minimum_required(VERSION 3.20)
project(UltraVisualizer LANGUAGES CXX)

find_path(AVISYNTH_INCLUDE_DIR avisynth.h REQUIRED)
add_library(decay STATIC Decay.cpp)
target_compile_features(decay PUBLIC cxx_std_17)
target_include_directories(decay PRIVATE "${AVISYNTH_INCLUDE_DIR}")
set_target_properties(decay PROPERTIES UNITY_BUILD OFF INTERPROCEDURAL_OPTIMIZATION OFF)

foreach(plugin IN ITEMS UltraVisualizer UltraVisualizerMT)
  add_library(${plugin} MODULE ${plugin}.cpp)
  target_link_libraries(${plugin} PRIVATE decay)
  target_include_directories(${plugin} PRIVATE "${AVISYNTH_INCLUDE_DIR}")
endforeach()

include(CheckCXXSourceCompiles)
check_cxx_source_compiles("
  #if defined(_M_ARM64EC) || !(defined(_M_IX86) || defined(_M_X64) || defined(__i386__) || defined(__x86_64__))
  #error Not an x86 target
  #endif
  int main() { return 0; }
" ULTRAVIS_TARGET_X86)

if(ULTRAVIS_TARGET_X86)
  target_sources(decay PRIVATE DecaySse2.cpp DecayAvx2.cpp)
  target_compile_definitions(decay PRIVATE ULTRAVIS_X86_KERNELS)
  if(MSVC)
    set_source_files_properties(DecayAvx2.cpp PROPERTIES COMPILE_OPTIONS "/arch:AVX2")
    if(CMAKE_SIZEOF_VOID_P EQUAL 4)
      set_source_files_properties(DecaySse2.cpp PROPERTIES COMPILE_OPTIONS "/arch:SSE2")
      set_source_files_properties(Decay.cpp UltraVisualizer.cpp UltraVisualizerMT.cpp
        PROPERTIES COMPILE_OPTIONS "/arch:IA32")
    endif()
  else()
    set_source_files_properties(DecaySse2.cpp PROPERTIES COMPILE_OPTIONS "-msse2")
    set_source_files_properties(DecayAvx2.cpp PROPERTIES COMPILE_OPTIONS "-mavx2")
  endif()
endif()
