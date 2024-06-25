#ifndef LEOMAX_TOKENIZER_NORMALIZERS_BERT_NORMALIZER_H_
#define LEOMAX_TOKENIZER_NORMALIZERS_BERT_NORMALIZER_H_

#include "normalizer.h"

namespace leomax_tokenizer {
namespace normalizers {

class BertNormalizer : public Normalizer {
public:
    BertNormalizer(bool clean_text = true, 
                   bool handle_chinese_chars = true,
                   bool strip_accents = true,
                   bool lowercase = true);
    virtual void operator() (NormalizedString *input) const override;

private:
    void do_clean_text(NormalizedString *input) const;
    void do_handle_chinese_chars(NormalizedString *input) const;
private:
    bool clean_text_;
    bool handle_chinese_chars_;
    bool strip_accents_;
    bool lowercase_;
};

}
}

#endif