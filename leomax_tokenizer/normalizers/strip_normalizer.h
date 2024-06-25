#ifndef LEOMAX_TOKENIZER_NORMALIZERS_STRIP_NORMALIZER_H
#define LEOMAX_TOKENIZER_NORMALIZERS_STRIP_NORMALIZER_H

#include "normalizer.h"
namespace leomax_tokenizer {
namespace normalizers {

class StripAccentsNormalizer : public Normalizer {
public: 
    virtual void operator()(NormalizedString* input) const override;
};

}
    
}


#endif