#ifndef ADDED_VOCABULARY_H_
#define ADDED_VOCABULARY_H_

#include <string>
#include <memory>
#include <utility>
#include <unordered_set>
#include "re2/re2.h"
#include "base.h"
#include "../pretokenizers/pretokenizer.h"
#include "../normalizers/normalizer.h"
#include "../models/models.h"

namespace leomax_tokenizer {
namespace core {
using re2::RE2;
using MatchSet = std::pair<std::shared_ptr<re2::RE2>, Vocab>;
using MatchResult = std::tuple<uint32_t, bool/*UNK flag*/, core::Offset>;

class AddedToken {
public:
    AddedToken();
    AddedToken(const std::string& content, 
                bool is_special = false,
                bool single_word = false,
                bool lstrip = false,
                bool rstrip = false);

    std::string get_content() const;
    bool get_is_special() const;
private:
    std::string content_;
    bool is_single_word_;
    bool is_special_;
};

class AddedVocabulary {
public:
    AddedVocabulary();

    void extract_and_normalize(const normalizers::Normalizer* normalizers,
                               const std::string& text,
                               pretokenizers::PreTokenizedString* pretokenized);
    bool split_with_indices(const normalizers::NormalizedString& normalized,
                            const MatchSet& pattern,
                            std::vector<pretokenizers::StringSplit>* split_results);
    bool find_match(const std::string& text,
                    const MatchSet& pattern, 
                    std::vector<MatchResult>* match_results);
    bool is_special_token(const std::string& token);
    bool token_to_id(const std::string& token, 
                     const models::Model& model,
                     uint32_t *id) const;

    size_t add_special_tokens(const std::vector<AddedToken>& tokens,
                              const models::Model& model);
    size_t add_tokens(const std::vector<AddedToken>& tokens,
                      const models::Model& model);

    size_t get_len() const {return this->vocab_.size();}

private:
    core::Vocab vocab_;
    MatchSet split_trie_;
    MatchSet split_normalized_trie_;
    std::vector<AddedToken> added_tokens_;
    std::vector<AddedToken> special_tokens_;
    std::unordered_set<std::string> special_tokens_set_;
};

}       // namespace core
}       // namespace leomax_tokenizer

#endif