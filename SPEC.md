---
status: active
code_paths:
  - CMakeLists.txt
  - sherpa-onnx/csrc/CMakeLists.txt
  - espeak-free-stubs/**
  - scripts/**
  - build-android-*.sh
  - build-ios*.sh
dependencies:
  - upstream k2-fsa/sherpa-onnx (merged by tag)
  - onnxruntime and other upstream third-party components
validations:
  - desktop build with -DSHERPA_ONNX_ENABLE_ESPEAK=OFF
  - scripts/verify-espeak-free.sh on every produced binary
provenance:
  intent: owner-confirmed
  validation: build-and-binary-verified
last_validated_sha: HEAD
---

# sherpa-onnx-espeak-free Specification

## Purpose

A minimal-divergence fork of k2-fsa/sherpa-onnx that can produce binaries
containing **no eSpeak NG or piper-phonemize code**, so proprietary
applications can ship sherpa-onnx TTS-capable builds without GPL-3.0
obligations.

**Decision:** This fork changes builds, not behavior. With
`SHERPA_ONNX_ENABLE_ESPEAK=ON` (default) the build is byte-for-byte upstream
semantics. All divergence is opt-in through one flag.

## Boundary

The fork owns:

- the `SHERPA_ONNX_ENABLE_ESPEAK` CMake option and its gating in the root and
  `sherpa-onnx/csrc` CMake files;
- the Apache-2.0 header stubs in `espeak-free-stubs/` that satisfy the
  espeak/piper API surface at compile time;
- forwarding of the flag through the Android/iOS build scripts;
- `scripts/verify-espeak-free.sh`, the binary-level proof of absence.

Everything else is upstream and must stay untouched so upstream tags merge
cleanly.

## Stable Invariants

- An `ESPEAK=OFF` build **never** fetches, compiles, or links eSpeak NG or
  piper-phonemize. `scripts/verify-espeak-free.sh` must pass on every
  distributed binary; it checks real-code markers (`espeak_*` API symbols, a
  defined `piper::phonemize_eSpeak`, the `ESPEAK_DATA_PATH` string), not
  naive substring matches, and is validated to FAIL on upstream espeak-linked
  binaries.
- The public API surface (C, C++, JNI, Swift) is identical in both modes so
  downstream wrappers compile unchanged.
- Espeak-backed phonemization in an `ESPEAK=OFF` build returns empty output
  with a one-time stderr diagnostic. It must never crash or exit the process;
  the stub `espeak_Initialize` reports success precisely because upstream
  exits on failure.
- The stubs contain no code or data derived from eSpeak NG or
  piper-phonemize — only the function/type names sherpa-onnx call sites
  require.

## Behavior Matrix (`ESPEAK=OFF`)

- Unaffected: all STT, VAD, keyword spotting, speaker diarization, and TTS
  front ends that do not consult espeak (character/lexicon-based models).
- Degraded to empty output: Piper voices, Kokoro's espeak path, Matcha/Kokoro
  OOV fallback. Applications gate these models out at the catalogue layer.
- Planned: [libphonemize](https://github.com/hurrtz/libphonemize) replaces
  the stubbed call sites with a permissive phonemizer, restoring these routes.

## Upstream Tracking

**Decision:** Track upstream by merging release tags (currently `v1.12.34`,
matching the react-native-sherpa-onnx wrapper's pin). After every merge:
rebuild with `ESPEAK=OFF`, run the verifier, and re-check that no new source
file gained an espeak dependency (`grep -rl espeak sherpa-onnx/csrc`).

## Non-Goals

- Rewriting or improving upstream sherpa-onnx.
- Providing a replacement phonemizer here (that is libphonemize's job).
- Supporting espeak-free builds of upstream's example binaries and app demos;
  only the library targets are in scope.
