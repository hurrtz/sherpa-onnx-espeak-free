// espeak-free stub for piper-phonemize's phonemize.hpp
//
// Part of sherpa-onnx-espeak-free (Apache-2.0). Provides the minimal piper
// namespace surface sherpa-onnx compiles against without building the
// GPL-linked piper-phonemize/eSpeak NG stack.
//
// Two modes:
//   default            — phonemization returns empty output with a one-time
//                        diagnostic; espeak-backed models are unavailable.
//   SHERPA_ONNX_USE_LIBPHONEMIZE — requests route into libphonemize
//                        (github.com/hurrtz/libphonemize, Apache-2.0), which
//                        loads its language packs from the same data
//                        directory the application already passes to
//                        espeak_Initialize (see speak_lib.h stub).

#ifndef ESPEAK_FREE_STUB_PHONEMIZE_HPP_
#define ESPEAK_FREE_STUB_PHONEMIZE_HPP_

#include <cstdio>
#include <string>
#include <vector>

#include "espeak-ng/speak_lib.h"

#if defined(SHERPA_ONNX_USE_LIBPHONEMIZE)
#include <mutex>
#include <unordered_map>

#include "phonemize.h"  // libphonemize C API
#endif

namespace piper {

using Phoneme = char32_t;

struct eSpeakPhonemeConfig {
  std::string voice = "en-us";
};

#if defined(SHERPA_ONNX_USE_LIBPHONEMIZE)

namespace detail {

inline std::vector<Phoneme> Utf8ToPhonemes(const char* utf8) {
  std::vector<Phoneme> out;
  size_t i = 0;
  const std::string s = utf8 ? utf8 : "";
  while (i < s.size()) {
    const unsigned char lead = static_cast<unsigned char>(s[i]);
    char32_t cp = 0;
    size_t len = 1;
    if ((lead & 0xF8) == 0xF0) { cp = lead & 0x07; len = 4; }
    else if ((lead & 0xF0) == 0xE0) { cp = lead & 0x0F; len = 3; }
    else if ((lead & 0xE0) == 0xC0) { cp = lead & 0x1F; len = 2; }
    else { cp = lead; }
    for (size_t k = 1; k < len && i + k < s.size(); ++k) {
      cp = (cp << 6) | (static_cast<unsigned char>(s[i + k]) & 0x3F);
    }
    out.push_back(cp);
    i += len;
  }
  return out;
}

struct ContextCache {
  std::mutex mutex;
  std::unordered_map<std::string, phonemize_context*> by_voice;

  static phonemize_context* Create(const std::string& dir,
                                   const std::string& language,
                                   phonemize_status* status) {
    phonemize_config config{};
    config.data_dir = dir.c_str();
    config.language = language.c_str();
    return phonemize_create(&config, status);
  }

  phonemize_context* Get(const std::string& voice) {
    std::lock_guard<std::mutex> lock(mutex);
    auto found = by_voice.find(voice);
    if (found != by_voice.end()) {
      return found->second;
    }
    const std::string dir = espeak_free::data_dir();
    phonemize_status status = PHONEMIZE_OK;
    phonemize_context* context = Create(dir, voice, &status);
    if (context == nullptr) {
      // espeak voice ids often carry a region (fr-fr, pt-pt); fall back to
      // the primary subtag pack, and en to en-us.
      const size_t dash = voice.find('-');
      if (dash != std::string::npos) {
        context = Create(dir, voice.substr(0, dash), &status);
      } else if (voice == "en") {
        context = Create(dir, "en-us", &status);
      }
    }
    if (context == nullptr) {
      static bool warned = false;
      if (!warned) {
        warned = true;
        std::fprintf(stderr,
                     "sherpa-onnx-espeak-free: libphonemize has no pack for "
                     "voice '%s' in '%s' (status %d)\n",
                     voice.c_str(), dir.c_str(), static_cast<int>(status));
      }
    }
    by_voice.emplace(voice, context);
    return context;
  }
};

inline ContextCache& cache() {
  // Deliberately leaked: contexts hold an onnxruntime environment whose
  // destruction order against other static teardown is undefined; a
  // process-lifetime singleton must not run destructors at exit.
  static ContextCache* instance = new ContextCache;
  return *instance;
}

}  // namespace detail

// Internal linkage, matching the non-routed stub: a weak external
// `piper::phonemize_eSpeak` in a static archive is indistinguishable from
// the real GPL library to a binary audit.
[[maybe_unused]] static void phonemize_eSpeak(
    const std::string& text,
    eSpeakPhonemeConfig& config,
    std::vector<std::vector<Phoneme>>& phonemes) {
  phonemes.clear();
  phonemize_context* context = detail::cache().Get(config.voice);
  if (context == nullptr) {
    return;
  }
  char* result = nullptr;
  const phonemize_status status =
      phonemize_text(context, text.c_str(), &result);
  if ((status == PHONEMIZE_OK || status == PHONEMIZE_PARTIAL) &&
      result != nullptr && result[0] != '\0') {
    phonemes.push_back(detail::Utf8ToPhonemes(result));
  }
  phonemize_free_string(result);
}

#else  // !SHERPA_ONNX_USE_LIBPHONEMIZE

[[maybe_unused]] static void phonemize_eSpeak(
    const std::string& /*text*/,
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

#endif  // SHERPA_ONNX_USE_LIBPHONEMIZE

}  // namespace piper

#endif  // ESPEAK_FREE_STUB_PHONEMIZE_HPP_
