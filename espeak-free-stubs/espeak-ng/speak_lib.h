// espeak-free stub for espeak-ng/speak_lib.h
//
// Part of sherpa-onnx-espeak-free: satisfies the eSpeak NG API surface that
// sherpa-onnx compiles against, without linking any GPL-licensed eSpeak NG
// code. Espeak-backed phonemization is unavailable in this build; models that
// require it produce no output instead of failing the process.
//
// This header is original work (Apache-2.0). It reproduces only the minimal
// function/enum names required by sherpa-onnx call sites — API names are not
// copyrightable subject matter — and none of eSpeak NG's implementation.

#ifndef ESPEAK_FREE_STUB_SPEAK_LIB_H_
#define ESPEAK_FREE_STUB_SPEAK_LIB_H_

typedef enum {
  AUDIO_OUTPUT_PLAYBACK,
  AUDIO_OUTPUT_RETRIEVAL,
  AUDIO_OUTPUT_SYNCHRONOUS,
  AUDIO_OUTPUT_SYNCH_PLAYBACK,
} espeak_AUDIO_OUTPUT;

// The real function returns the sample rate on success; sherpa-onnx exits the
// process on any other value, so the stub reports success and lets the
// phonemizer stub produce empty output instead.
inline int espeak_Initialize(espeak_AUDIO_OUTPUT /*output*/,
                             int /*buflength*/,
                             const char* /*path*/,
                             int /*options*/) {
  return 22050;
}

#endif  // ESPEAK_FREE_STUB_SPEAK_LIB_H_
