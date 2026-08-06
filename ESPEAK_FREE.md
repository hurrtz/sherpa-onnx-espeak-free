# sherpa-onnx-espeak-free

A fork of [k2-fsa/sherpa-onnx](https://github.com/k2-fsa/sherpa-onnx) that can
build **without eSpeak NG**, for proprietary applications that cannot carry
GPL-3.0 code.

## Why

Upstream sherpa-onnx compiles eSpeak NG (GPL-3.0-or-later) and
piper-phonemize into every TTS-enabled binary. Statically linking GPL code
into a closed-source app obligates releasing the app's source under the GPL —
a blocker for App Store / Play Store distribution of proprietary apps. This
fork adds one build option that removes the GPL surface entirely while
keeping the full sherpa-onnx API, so downstream bindings (JNI, Swift, React
Native wrappers) compile unchanged.

## Usage

```
cmake -DSHERPA_ONNX_ENABLE_ESPEAK=OFF ...
```

- `SHERPA_ONNX_ENABLE_ESPEAK=ON` (default): identical to upstream.
- `SHERPA_ONNX_ENABLE_ESPEAK=OFF`: eSpeak NG and piper-phonemize are neither
  fetched, compiled, nor linked. Header-only stubs in `espeak-free-stubs/`
  satisfy the API surface the TTS sources compile against.

## Behavior of an espeak-free build

- **Unaffected:** all STT (Whisper, Zipformer, Paraformer, …), speaker
  diarization, VAD, keyword spotting, and TTS models with non-espeak front
  ends (character/lexicon-based VITS, e.g. Chinese models).
- **Unavailable:** phonemization for espeak-backed TTS routes — Piper voices,
  Kokoro (espeak path), Matcha/Kokoro OOV fallback. These produce empty
  output with a one-time stderr diagnostic instead of crashing; gate those
  models out in your application layer.
- A permissive phonemizer replacement
  ([libphonemize](https://github.com/hurrtz/libphonemize), Apache-2.0) is
  planned to restore those routes behind the same call sites.

## Verifying a binary is clean

```
nm -gU <lib> | grep -ci espeak     # expect 0
strings <lib> | grep -i espeak     # expect no matches
```

## Divergence from upstream

Kept intentionally minimal so upstream tags merge cleanly:

- `CMakeLists.txt`: `SHERPA_ONNX_ENABLE_ESPEAK` option; gated
  espeak/piper-phonemize includes; stub include path when OFF.
- `sherpa-onnx/csrc/CMakeLists.txt`: gated `piper_phonemize` link and the
  piper-phonemize test.
- `espeak-free-stubs/`: new, Apache-2.0 original headers (~90 lines total).
  No eSpeak NG or piper-phonemize code was copied; the stubs reproduce only
  the function/type names sherpa-onnx call sites require.

## Licensing

Upstream sherpa-onnx is Apache-2.0 (see `LICENSE`); this fork's additions are
Apache-2.0. An espeak-free build's third-party set contains **no GPL or LGPL
components from the espeak/piper stack**. Other bundled dependencies
(onnxruntime, kaldi-native-fbank, …) keep their upstream licenses — audit
them for your distribution as usual.
