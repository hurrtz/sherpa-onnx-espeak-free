#!/usr/bin/env bash
# Verifies a built sherpa-onnx binary contains no eSpeak NG / piper-phonemize
# code. Sherpa's own Apache-licensed sources legitimately mention "espeak" in
# symbol names (…OfflineSpeaker… substring hits) and log strings (data-dir
# hints, the stub's diagnostic), so naive `grep -i espeak` false-positives;
# this script checks markers that only exist in the real GPL code:
#
#   1. `_espeak_` C API symbols (espeak_Initialize, espeak_SetVoiceByName, …)
#      — the stub build inlines its only lookalike, the real library exports
#      dozens.
#   2. A defined `piper::phonemize_eSpeak` implementation symbol.
#   3. The `ESPEAK_DATA_PATH` string — internal to eSpeak NG's loader, absent
#      from every sherpa source file.
#
# Usage: scripts/verify-espeak-free.sh <lib-or-archive> [...]

set -euo pipefail

if [ "$#" -lt 1 ]; then
  echo "usage: $0 <lib-or-archive> [...]" >&2
  exit 2
fi

status=0
for lib in "$@"; do
  if [ ! -f "$lib" ]; then
    echo "FAIL $lib: file not found" >&2
    status=1
    continue
  fi

  api_symbols=$(nm -g "$lib" 2>/dev/null | grep -c "[ _]_\{0,1\}espeak_" || true)
  phonemize_impl=$(nm -g "$lib" 2>/dev/null | c++filt | grep -c "piper::phonemize_eSpeak" || true)
  data_path=$(strings "$lib" 2>/dev/null | grep -c "ESPEAK_DATA_PATH" || true)

  if [ "$api_symbols" -eq 0 ] && [ "$phonemize_impl" -eq 0 ] && [ "$data_path" -eq 0 ]; then
    echo "OK   $lib: no eSpeak NG / piper-phonemize markers"
  else
    echo "FAIL $lib: espeak_* symbols=$api_symbols phonemize_eSpeak=$phonemize_impl ESPEAK_DATA_PATH=$data_path" >&2
    status=1
  fi
done

exit "$status"
