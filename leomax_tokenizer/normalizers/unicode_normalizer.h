#ifndef LEOMAX_TOKENIZER_NORMALIZERS_UNICODE_NORMALIZER_H_
#define LEOMAX_TOKENIZER_NORMALIZERS_UNICODE_NORMALIZER_H_

#include "normalizer.h"
namespace leomax_tokenizer {
namespace normalizers {

class NFKCNormalizer : public Normalizer {
public:
    virtual void operator()(NormalizedString* input) const override;
};
}
}


#endif