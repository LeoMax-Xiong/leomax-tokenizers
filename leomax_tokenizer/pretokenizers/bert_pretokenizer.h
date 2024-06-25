#ifndef LEOMAX_TOKENIZER_PRETOKENIZERS_BERT_PRETOKENIZER_H_
#define LEOMAX_TOKENIZER_PRETOKENIZERS_BERT_PRETOKENIZER_H_
#include "pretokenizer.h"
namespace leomax_tokenizer {
namespace pretokenizers {

class BertPreTokenizer : public PreTokenizer {
public:
    virtual void operator()(PreTokenizedString* pretokenizerd) const override;
};

}
}

#endif