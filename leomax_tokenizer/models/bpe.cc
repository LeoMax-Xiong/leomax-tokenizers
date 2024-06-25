//
// Created by xiongxinlei on 5/28/24.
//
#include "bpe.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "nlohmann/json.hpp"
#include "glog/logging.h"
#include "../utils/utf8.h"
namespace leomax_tokenizer {
namespace models {

const std::string WHITESPACE = " \n\r\t\f\v";

BPE::BPE() :
    fuse_unk_(false) {
}

BPE::BPE(const core::Vocab& vocab,
         const core::Merges& merges,
         size_t cache_capacity,
         const std::vector<float>& dropout,
         const std::vector<std::string>& unk_token,
         const std::vector<std::string>& continuing_subword_prefix,
         const std::vector<std::string>& end_of_word_suffix,
         bool fuse_unk) :
        vocab_(vocab),
        dropout_(dropout),
        unk_token_(unk_token),
        continuing_subword_prefix_(continuing_subword_prefix),
        end_of_word_suffix_(end_of_word_suffix),
        fuse_unk_(fuse_unk) {
    init(merges);
}

void BPE::init(const core::Merges& merges) {
    if (!dropout_.empty()) {
        if (dropout_[0] > 1.0 || dropout_[0] < 0.0) {
          throw std::runtime_error("dropout probability must be in range [0,1]");
        }
    }

    // 构建token id到token的映射关系
    for (auto& item: vocab_) {
        vocab_reversed_[item.second] = item.first;
    }

    // 去掉连接符号
    int prefix_len = 0;
    if (continuing_subword_prefix_.size() > 0) {
        prefix_len = continuing_subword_prefix_[0].size();
    }

    // 要求外部将merges传进来
    for (int i = 0; i < merges.size(); i++) {
        // 获取合并规则
        auto&& merge = merges[i];
        try {
            // 合并规则中内容必须要在词典中
            auto a_id = vocab_.at(merge.first);
            auto b_id = vocab_.at(merge.second);

            // 合并规则中内容必须要在词典中
            auto new_token = merge.first + merge.second.substr(prefix_len);
            auto new_id = vocab_.at(new_token);

            merges_[core::Pair(a_id, b_id)] = {i, new_id};
        } catch (...) {
            throw std::runtime_error("Can't merge token out of the vocabulary ");
        }
    }

    VLOG(6) << "unk token size= " << unk_token_.size(); 
    if (unk_token_.size() > 0) {
        try {
            // 添加 unk_token 到unk_token_id_ 中
            VLOG(6) << "start to fetch the unk id";
            unk_token_id_.emplace_back(vocab_.at(unk_token_.front()));
            VLOG(6) << "unk_token_id_ back= " << unk_token_id_.back();
        } catch (...) {
            std::ostringstream oss;
            oss << "Unk token " << unk_token_.front() << " not found in vocab";
            throw std::runtime_error(oss.str());
        }
    }
}

void BPE::tokenize_with_cache(const std::string& text, std::vector<core::Token>* tokens) {
    core::BPEWord bpe_word;
    VLOG(6) << "BPE::tokeniz_with_cache, text= " << text;
    // 判断文本是否在缓存中,如果改文本在合并规则中,那么直接获取拆分之后的词
    if (cache_.get_value(text, &bpe_word)) {
        // 将词转换为token
        VLOG(6) << "BPE::tokeniz_with_cache, get from cache, text= " << text;
        word_to_tokens(bpe_word, tokens);
    } else {
        // 根据合并规则，将文本拆分成多个词
        VLOG(6) << "BPE::tokeniz_with_cache, start to merge the text, text= " << text;
        merge_word(text, &bpe_word);
        
        // 将每个词转换为token
        word_to_tokens(bpe_word, tokens);
        // 将词加入缓存
        cache_.set_value(text, bpe_word);
    }
}

void BPE::word_to_tokens(const core::BPEWord& bpe_word, std::vector<core::Token>* tokens) {

}

void BPE::merge_word(const std::string& word, core::BPEWord* bpe_word) {
    VLOG(6) << "BPE::merge_word, word1= " << word;
    std::vector<std::pair<uint32_t, size_t>> unk;
    bpe_word->reserve(word.length());       // bpe_word 的最大长度为文本word的长度
    uint32_t start = 0;
    while (start < word.length()) {
        uint32_t content_char;
        uint32_t content_char_width = 
                 utils::utf8_to_uint32(word.data() + start, &content_char);
        content_char = utils::utf8_to_unicode(content_char);
        VLOG(6) << "start= " << start 
                << ", content_char_width= " << content_char_width 
                << ", content_char= " << content_char;
        uint32_t end = start + content_char_width;
        
        bool is_first = (start == 0);   // 是否是开头的第一个字符
        bool is_last = (end >= word.length()); // 是否已经到了最后的长度
        std::string curr_str = word.substr(start, content_char_width);      // 截取一个字符串

        VLOG(6) << "is_first= " << is_first 
                << ", is_last= " << is_last
                << ", curr_str= " << curr_str;
        if (!is_first) {    // 不是开头字符，可以加入链接的字符串
            if (this->continuing_subword_prefix_.size() > 0) {
                curr_str = this->continuing_subword_prefix_.front() + curr_str;
            }
        }

        if (is_last) {  // 最后的字符串要加上结束符号
            if (this->end_of_word_suffix_.size() > 0) {
                curr_str = curr_str + this->end_of_word_suffix_.front();
            }
        }

        if (this->vocab_.find(curr_str) != this->vocab_.end()) {
            VLOG(6) << "curr_str= " << curr_str << " in the vocab";
            if (unk.size() > 0) {
                bpe_word->add(unk.front().first, unk.front().second);
                unk.clear();
            }

            auto id = this->vocab_.at(curr_str);
            bpe_word->add(id, content_char_width);
        } else {
            VLOG(6) << "curr_str= " << curr_str << " not in the vocab !!!!!";
            VLOG(6) << "unk_token_id_= " << unk_token_id_.size();
            if (unk_token_id_.size() > 0) {
                if (unk.size() == 0) {
                    VLOG(6) << "unk size=0";
                    unk.push_back({unk_token_id_.front(), content_char_width});
                    VLOG(6) << "unk_token_id_.front= " << unk_token_id_.front();
                } else {
                    VLOG(6) << "fuse_unk= " << fuse_unk_;
                    if (fuse_unk_) {
                        unk[0] = {unk[0].first, unk[0].second + content_char_width};
                    } else {
                        bpe_word->add(unk[0].first, unk[0].second);
                        VLOG(6) << "unk_token_id_.front= " << unk_token_id_.front();
                        unk[0] = {unk_token_id_.front(), content_char_width};
                    }
                }
            }
        }

        start = end;
    }
    if (unk.size() > 0) {
        bpe_word->add(unk.front().first, unk.front().second);
    }
    bpe_word->merge_all(merges_, dropout_);
}

std::vector<core::Token> BPE::tokenize(const std::string& text) {
    std::vector<core::Token> tokens;
    VLOG(6) << "BPE tokenizer, text= " << text;

    if (text.empty()) {
        return tokens;
    }

    if (dropout_.empty()) {
        tokenize_with_cache(text, &tokens);
        return tokens;
    } else {
        VLOG(6) << "BPE tokenizer with dropout";
    }


    return tokens;
}

void BPE::get_vocab_and_merges_from_file(const std::string& vocab_json_path,
                                const std::string& merge_path,
                                core::Vocab* vocab,
                                core::Merges* merges) {
    *vocab = BPE::get_vocab_from_file(vocab_json_path);
    *merges = BPE::get_merges_from_file(merge_path);
}

core::Vocab BPE::get_vocab_from_file(const std::string& vocab_json_path) {
    std::ifstream fin(vocab_json_path);
    core::Vocab vocab;
    nlohmann::json vocab_json;
    fin >> vocab_json;
    for (auto it = vocab_json.begin(); it != vocab_json.end(); ++it) {
        vocab[it.key()] = it.value();
    }

    VLOG(6) << "vocab size: " << vocab.size();
    return vocab;

}
core::Merges BPE::get_merges_from_file(const std::string& merge_path) {
    core::Merges merges;
    std::ifstream fin(merge_path);
    std::string word_str;
    while (std::getline(fin, word_str)) {
        // 句首，表示该merges.txt 由哪一个版本的tokenizer生成
        if (word_str.find("#version") == 0) {
            continue;
        }

        std::pair<std::string, std::string> result;
        construct_merges_pair(word_str, &result);
        merges.emplace_back(result);
    }

    return merges;
}

void BPE::construct_merges_pair(const std::string& word_line,
                                std::pair<std::string, std::string>* pair_result) {
    // 找到第一个词的开始和结束位置
    auto pair_a_begin = word_line.find_first_not_of(WHITESPACE); 
    auto pair_a_end = word_line.find_first_of(WHITESPACE, pair_a_begin); 

    // 找到第二个词的开始和结束位置
    auto pair_b_begin = word_line.find_first_not_of(WHITESPACE, pair_a_end);
    auto pair_b_end = word_line.find_first_of(WHITESPACE, pair_b_begin);
    *pair_result = {word_line.substr(pair_a_begin, pair_a_end - pair_a_begin),
               word_line.substr(pair_b_begin, pair_b_end - pair_b_begin)};

}

}  // namespace models
}  // namespace leomax_tokenizer
