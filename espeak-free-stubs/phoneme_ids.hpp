// espeak-free stub for piper-phonemize's phoneme_ids.hpp
//
// Part of sherpa-onnx-espeak-free (Apache-2.0). sherpa-onnx maps phonemes to
// ids with its own tables; the only symbol it consumes from this header is
// the id type.

#ifndef ESPEAK_FREE_STUB_PHONEME_IDS_HPP_
#define ESPEAK_FREE_STUB_PHONEME_IDS_HPP_

#include <cstdint>

namespace piper {

using PhonemeId = int64_t;

}  // namespace piper

#endif  // ESPEAK_FREE_STUB_PHONEME_IDS_HPP_
