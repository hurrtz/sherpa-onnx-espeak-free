# Agent instructions — sherpa-onnx-espeak-free

Read `SPEC.md` before changing anything; update it in the same change when a
decision or invariant moves. `ESPEAK_FREE.md` is the user-facing summary and
must stay consistent with `SPEC.md`.

## Prime directive

**Minimal divergence.** This fork exists to stay mergeable with upstream
k2-fsa/sherpa-onnx. Never refactor, reformat, or "improve" upstream files.
Every divergence must be attributable to the espeak-free purpose and must be
listed in `ESPEAK_FREE.md`'s divergence section.

## Licensing rules (hard)

- Never copy code, tables, or data from eSpeak NG or piper-phonemize into
  this repository — not even snippets in comments. The stubs reproduce API
  *names* only.
- Additions are Apache-2.0, consistent with upstream.

## Workflow

- Build (desktop validation):
  `cmake -B build-espeak-free -DCMAKE_BUILD_TYPE=Release -DSHERPA_ONNX_ENABLE_ESPEAK=OFF -DSHERPA_ONNX_ENABLE_WEBSOCKET=OFF -DSHERPA_ONNX_ENABLE_PYTHON=OFF -DSHERPA_ONNX_ENABLE_TESTS=OFF -DBUILD_SHARED_LIBS=ON && cmake --build build-espeak-free -j8`
- Verify every produced binary:
  `scripts/verify-espeak-free.sh <libs...>` — must print OK for espeak-free
  builds; sanity-check the verifier still FAILs on an upstream espeak-linked
  binary when you change it.
- Android: `build-android-<abi>.sh` with `SHERPA_ONNX_ENABLE_ESPEAK=OFF` and
  `ANDROID_NDK` set. iOS: `build-ios.sh` (static xcframework) with the same
  env var.
- Upstream upgrade: `git fetch upstream tag vX.Y.Z --no-tags`,
  `git merge vX.Y.Z`, resolve only in the gated blocks, rebuild, re-verify,
  re-grep for new espeak consumers in `sherpa-onnx/csrc`, update
  `SPEC.md`/`ESPEAK_FREE.md` if the divergence list changed.

## Landmines

- `grep -i espeak` on binaries false-positives: `…OfflineSpeaker…` contains
  the substring, and sherpa's own log strings mention espeak-ng-data. Use the
  verifier's markers, never naive grep, when judging cleanliness.
- The stub `espeak_Initialize` must keep returning 22050: upstream
  `SHERPA_ONNX_EXIT(-1)`s on any other value.
- New upstream releases may add espeak call sites; the stub surface in
  `espeak-free-stubs/` must grow with them (compile errors are the detector).
