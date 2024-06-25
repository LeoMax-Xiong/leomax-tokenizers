//
// Created by xiongxinlei on 5/28/24.
//

#ifndef LEOMAX_TOKENIZER_BPE_H
#define LEOMAX_TOKENIZER_BPE_H
#include "models.h"
#include "vector"
#include "../core/base.h"
#include "../core/cache.h"
namespace leomax_tokenizer {
namespace models {

class BPE : public Model {
public:
    BPE();
    BPE(const core::Vocab& vocab,
        const core::Merges& merges = {},
        size_t cache_capacity = core::DEFAULT_CACHE_CAPACITY,
        const std::vector<float>& dropout = {},
        const std::vector<std::string>& unk_token = {},
        const std::vector<std::string>& continuing_subword_prefix = {},
        const std::vector<std::string>& end_of_word_suffix = {},
        bool fuse_unk = false);

    virtual std::vector<core::Token> tokenize(const std::string& text) override;
    static void get_vocab_and_merges_from_file(const std::string& vocab_json_path,
                                        const std::string& merge_path,
                                        core::Vocab* vocab,
                                        core::Merges* merges);
    static core::Vocab get_vocab_from_file(const std::string& vocab_json_path);
    static core::Merges get_merges_from_file(const std::string& merge_path);
    static void construct_merges_pair(const std::string& word_line,
                               std::pair<std::string, std::string>* pair);

private:
    void init(const core::Merges& merges);
    void tokenize_with_cache(const std::string& text, std::vector<core::Token>* tokens);
    void word_to_tokens(const core::BPEWord& bpe_word, std::vector<core::Token>* tokens);
    void merge_word(const std::string& word, core::BPEWord* bpe_word);
private:
    core::Vocab vocab_;
    core::VocabReversed vocab_reversed_;
    core::MergeMap merges_;
    core::Cache<std::string, core::BPEWord> cache_;     // 记录已经编码过的字符串，避免重复编码，提高效率
    std::vector<float> dropout_;
    std::vector<std::string> unk_token_;
    std::vector<uint32_t> unk_token_id_;
    std::vector<std::string> continuing_subword_prefix_;
    std::vector<std::string> end_of_word_suffix_;
    bool fuse_unk_;

};

} // namespace models
} // namespace leomax_tokenizer

#endif  // LEOMAX_TOKENIZER_BPE_H
