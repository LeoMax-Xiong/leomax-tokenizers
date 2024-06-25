#include "unicode_normalizer.h"
#include "glog/logging.h"
namespace leomax_tokenizer {
namespace normalizers {
void NFKCNormalizer::operator()(NormalizedString* input) const {
    VLOG(6) << "NFKCNormalizer::operator()"; 
    input->NFKC();
}
}
}