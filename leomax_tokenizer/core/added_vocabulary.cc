#include <iostream>
#include "glog/logging.h"
#include "added_vocabulary.h"
#include "../pretokenizers/pretokenizer.h"
namespace leomax_tokenizer {
namespace core {
AddedToken::AddedToken(): 
    content_(""),
    is_single_word_(false),
    is_special_(false) {

}

AddedToken::AddedToken(const std::string& content, 
            bool is_special,
            bool single_word,
            bool lstrip,
            bool rstrip) :
    content_(content),
    is_single_word_(single_word),
    is_special_(is_special) {

}

std::string AddedToken::get_content() const {
    return this->content_;
}

bool AddedToken::get_is_special() const {
    return is_special_;
}

AddedVocabulary::AddedVocabulary() :
    split_trie_({std::make_shared<re2::RE2>(""), Vocab()}),
    split_normalized_trie_({std::make_shared<re2::RE2>(""), Vocab()}) {
    
}

void AddedVocabulary::extract_and_normalize(const normalizers::Normalizer* normalizers,
                                            const std::string& text,
                                            pretokenizers::PreTokenizedString* pretokenized) {
    VLOG(6) << "AddedVocabulary::extract_and_normalize, text = " << text;
    // 构建预处理信息
    // step1: 设置预处理前的原始字符串
    pretokenized->set_original_str(text);  

    VLOG(6) << "extract and normalize, pretokenized split size: " 
                << pretokenized->get_split_size();
    VLOG(6) << "splits[0].normalized.alignments size: " 
              << pretokenized->get_split(0).normalized_.alignments_.size();

    // step2: 通过预处理对原始字符串进行分词     
    //        通过lambda表达式传递实际的分词函数
    pretokenized->split([&](int idx,
                            normalizers::NormalizedString* normalized,
                            std::vector<pretokenizers::StringSplit>* string_splits) {
                    // 预先进行分词处理
                    this->split_with_indices(*normalized, this->split_trie_, string_splits);         
                });

    pretokenized->split(
                [&](int idx, 
                    normalizers::NormalizedString* normalized,  // 预处理之前的数据
                    std::vector<pretokenizers::StringSplit>* string_splits) {
                    if(normalizers != nullptr) {
                        // 预处理, 调用operator() 函数
                        (*normalizers)(normalized); 
                        VLOG(6) << "After normalization, normalized str = " << normalized->get_str();
                        this->split_with_indices(*normalized, 
                                                this->split_normalized_trie_, 
                                                string_splits);
                    } else {
                        VLOG(6) << "normalizers == nullput !!!!!!!!!!!!";
                    }
                } );
}

bool AddedVocabulary::find_match(const std::string& text,
                                const MatchSet& pattern, 
                                std::vector<MatchResult>* match_results) {
    std::cout << "AddedVocabulary::find_match" << std::endl;
    std::cout << "text: " << text << std::endl;
    if (text.empty()) {
        return false;
    }

    std::vector<MatchResult> splits;
    size_t start = 0;
    size_t start_offset = 0;
    size_t end = text.length();
    re2::StringPiece result_str;
    std::cout << "start = " << start 
              << ", end = " << end 
              << ", text = " << text 
              << ", pattern = " << pattern.first->pattern() 
              << std::endl;
    
    // 从头开始遍历整个文本
    // 先用特殊符号进行分割
    while (pattern.first->Match(
            text, start, end, RE2::UNANCHORED, &result_str, 1) && 
            !result_str.empty()) {
        std::cout << "result_str = " << result_str.as_string() 
                 << ", " << pattern.first->pattern() << std::endl;
        size_t curr_start = result_str.data() - text.data();
        size_t curr_end = curr_start + result_str.length();

        uint32_t id = pattern.second.at(result_str.ToString());
    }

    if (start != text.length()) {
        splits.push_back({0, false, {start, text.length()}});
    }

    *match_results = std::move(splits);
    return true;
}

bool AddedVocabulary::split_with_indices(const normalizers::NormalizedString& normalized,
                                        const MatchSet& pattern,
                                        std::vector<pretokenizers::StringSplit>* split_results) {
    std::vector<MatchResult> match_results;
    // 根据 pattern 匹配 normalized
    bool status = find_match(normalized.get_str(), 
                             pattern, 
                             &match_results);

    for (auto& match_result : match_results) {
        normalizers::NormalizedString slice;
        auto id = std::get<0>(match_result);
        auto is_not_unk = std::get<1>(match_result);
        auto offsets = std::get<2>(match_result);

        std::cout << "AddedVocabulary::split_with_indices, id = " << id << std::endl;
        VLOG(6) << "AddedVocabulary::split_with_indices, is_not_unk = " << is_not_unk;
        normalized.slice(offsets, &slice, false);
        std::vector<core::Token> tokens;
        if (is_not_unk) {
            tokens.emplace_back(core::Token{id, slice.get_str(), {0, slice.get_len()}});
        }

        split_results->emplace_back(slice, tokens);
    }

    VLOG(6) << "AddedVocabulary::split_with_indices, split_results size = " 
              << split_results->size();
    return status;
}

bool AddedVocabulary::token_to_id(const std::string& token, 
                                 const models::Model& model,
                                 uint32_t *id) const {
    // 判断词表中是否有该token，如果存在则返回该token的id
    std::cout << "AddedVocabulary::token_to_id, token = "
              << token << std::endl;
    if (this->vocab_.find(token) != this->vocab_.end()) {
        *id = this->vocab_.at(token);
        return true;
    }

    // 判断外部传入的词表中是否有该token，如果存在则返回该token的id
    return model.token_to_id(token, id);
}


size_t AddedVocabulary::add_special_tokens(const std::vector<AddedToken>& tokens,
                                          const models::Model& model) {
    return this->add_tokens(tokens, model);
}

size_t AddedVocabulary::add_tokens(const std::vector<AddedToken>& tokens,
                                   const models::Model& model) {
    for (const auto& token: tokens) {
        std::cout << "AddedVocabulary::add_tokens, process token = " 
                  << token.get_content() << std::endl;
        if (token.get_is_special() && 
            !token.get_content().empty() && 
            !is_special_token(token.get_content())) {
            std::cout << "AddedVocabulary::词典中添加特殊符号 " << token.get_content() << std::endl;
            special_tokens_.push_back(token);
            special_tokens_set_.insert(token.get_content());
        }
    }

    int ignored_tokens_num = 0;
    for (const auto& token: tokens) {
        if (token.get_content().empty()) {
            ignored_tokens_num++;
            continue;
        }

        uint32_t id = vocab_.size();
        if (this->token_to_id(token.get_content(), model, &id)) {
            std::cout << "AddedVocabulary::已经在词典中了: token = " << token.get_content() << std::endl;
            ignored_tokens_num++;
        } else {
            // 新的符号
            uint32_t new_id = model.get_vocab_size() + this->get_len();
            this->vocab_[token.get_content()] = new_id;
            if (this->special_tokens_set_.count(token.get_content()) == 0) {
                std::cout << "AddedVocabulary::添加新的符号, token = " << token.get_content() << std::endl;
                this->added_tokens_.push_back(token);
            }
        }
    }
    return 0;
}

bool AddedVocabulary::is_special_token(const std::string& token) {
    return special_tokens_set_.find(token) != special_tokens_set_.end();
}

}
}