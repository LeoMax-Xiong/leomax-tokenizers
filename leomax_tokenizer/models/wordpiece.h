//
// Created by xiongxinlei on 5/29/24.
//

#ifndef LEOMAX_TOKENIZER_WORDPIECE_H
#define LEOMAX_TOKENIZER_WORDPIECE_H
#include "models.h"
#include "../core/base.h"

namespace leomax_tokenizer {
namespace models {
class WordPiece : public Model {
public:
    WordPiece();
    WordPiece(const core::Vocab& vocab,
              const std::string& unk_token = "[UNK]",
              size_t max_input_chars_per_word = 100,
              const std::string& continuing_subword_prefix = "##",
              bool handle_chinese_chars = true);
    virtual std::vector<core::Token> tokenize(const std::string &text) override;
    virtual bool token_to_id(const std::string& token, uint32_t *id) const override;
    virtual size_t get_vocab_size() const override;
    
    static core::Vocab get_vocab_from_file(const std::string &vocab_file);
    static WordPiece get_wordpiece_from_file( 
        const std::string& vocab_file,
        const std::string& unk_token="[UNK]",
        size_t max_input_chars_per_word=100,
        const std::string& continuing_subword_prefix="##");
private:
    core::Vocab vocab_;
    core::VocabReversed vocab_reversed_;
    size_t max_input_chars_per_word_;
    std::string continuing_subword_prefix_;
    bool handle_chinese_chars_;
    std::string unk_token_;
    uint32_t unk_token_id_;
};

}  // namespace models
}  // namespace leomax_tokenizer
#endif  // LEOMAX_TOKENIZER_WORDPIECE_H
