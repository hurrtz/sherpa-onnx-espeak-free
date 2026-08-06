// espeak-free stub for piper-phonemize's phonemize.hpp
//
// Part of sherpa-onnx-espeak-free (Apache-2.0). Provides the minimal piper
// namespace surface sherpa-onnx compiles against without building the
// GPL-linked piper-phonemize/eSpeak NG stack. Phonemization requests return
// empty results with a one-time diagnostic; a permissive front end
// (libphonemize) plugs in behind these call sites in a later milestone.

#ifndef ESPEAK_FREE_STUB_PHONEMIZE_HPP_
#define ESPEAK_FREE_STUB_PHONEMIZE_HPP_

#include <cstdio>
#include <string>
#include <vector>

namespace piper {

using Phoneme = char32_t;

struct eSpeakPhonemeConfig {
  std::string voice = "en-us";
};

inline void phonemize_eSpeak(const std::string& /*text*/,
                             eSpeakPhonemeConfig& /*config*/,
                             std::vector<std::vector<Phoneme>>& phonemes) {
  static bool warned = false;
  if (!warned) {
    warned = true;
    std::fprintf(
        stderr,
        "sherpa-onnx-espeak-free: this build excludes eSpeak NG (GPL). "
        "Espeak-backed phonemization returns no output; models that rely on "
        "it (Piper, Kokoro via espeak, Matcha OOV) are unavailable.\n");
  }
  phonemes.clear();
}

}  // namespace piper

#endif  // ESPEAK_FREE_STUB_PHONEMIZE_HPP_
