#include "strip_normalizer.h"

namespace leomax_tokenizer {
namespace normalizers {

void StripAccentsNormalizer::operator()(NormalizedString* input) const {
    input->NFD();
}

}

}