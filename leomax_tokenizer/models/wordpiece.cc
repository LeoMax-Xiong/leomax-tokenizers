//
// Created by xiongxinlei on 5/29/24.
//

#include <fstream>
#include "../utils/utf8.h"
#include "wordpiece.h"
#include "glog/logging.h"

namespace leomax_tokenizer {
namespace models {

static bool check_if_string_is_alphanum(const std::string& str) {
    return std::count_if(str.begin(), str.end(), [](char ch) {
        // 如果是字母或者数字，返回非零，否则返回0
        return std::isalnum(ch) > 0;
    }) == str.length();
}

const std::string WHITESPACE = " \n\r\t\f\v";

WordPiece::WordPiece() :
    unk_token_("[UNK]"),
    continuing_subword_prefix_("##"),
    max_input_chars_per_word_(100),
    unk_token_id_(0),
    handle_chinese_chars_(true) {
    VLOG(6) << "WordPiece default constructor";

}

WordPiece::WordPiece(const core::Vocab &vocab,
          const std::string& unk_token,
          size_t max_input_chars_per_word,
          const std::string& continuing_subword_prefix,
          bool handle_chinese_chars) :
       vocab_(vocab),
       unk_token_(unk_token),
       max_input_chars_per_word_(max_input_chars_per_word),
       continuing_subword_prefix_(continuing_subword_prefix),
       handle_chinese_chars_(handle_chinese_chars) {
    VLOG(6) << "WordPiece constructor";
    for (const auto& item : vocab) {
        vocab_reversed_[item.second] = item.first;
    }
    
    VLOG(6) << "WordPiece vocab size: " << this->vocab_.size();
    // 计算得到 unk 的id，可能找不到，没有抛出异常
    // NOTE: todo: 考虑抛出异常，如果没有 unk_token，则抛出异常
    unk_token_id_ = vocab_.at(unk_token);
}

/*****************************************************************************/
/****************************使用wordpiece进行分词******************************/
/*****************************************************************************/
std::vector<core::Token> WordPiece::tokenize(const std::string &text) {
    VLOG(6) << "WordPiece tokenize";
    std::vector<core::Token> all_tokens;
    size_t unicode_len = utils::get_unicode_len_from_utf8(text.data(), 
                                                          text.length());

    VLOG(6) << "WordPiece tokenize, unicode_len: " << unicode_len;
    if (unicode_len > this->max_input_chars_per_word_) {
        // 如果超过了最大的长度，那么直接返回unk
        all_tokens.emplace_back(this->vocab_.at(this->unk_token_), 
                               this->unk_token_, core::Offset{0, text.length()});
    } else {
        bool found_token = true;
        uint32_t start = 0;

        while (start < text.length()) {
            uint32_t end = text.length();
            core::Token cur_token;
            bool match_cur_token = false;
            // 遍历到序列的最后
            while(start < end) {
                // 获取完整的字符串
                std::string sub_str = text.substr(start, end - start);
                VLOG(6) << "WordPiece tokenize, start: "
                        << start << ", end: " << end 
                        << ", sub_str: [" << sub_str << "]";
                // 非首token，则加上前缀，且全部是字母或者数字
                // 将数字字母切开，加上前缀
                if (start > 0 && 
                    (this->handle_chinese_chars_ || check_if_string_is_alphanum(sub_str))) {
                    sub_str = this->continuing_subword_prefix_ + sub_str;
                }
                // 判断是否在vocab中，如果找到，那么将这个词切开，作为一个token处理
                const auto& vocab_iter = this->vocab_.find(sub_str);
                if (vocab_iter != this->vocab_.end()) {
                    VLOG(6) << "WordPiece tokenize, found sub_str: " << sub_str;
                    cur_token = {vocab_iter->second, sub_str, core::Offset{start, end}};
                    match_cur_token = true;
                    break;
                }

                // 如果没找到待匹配的内容，那么更新end到下一个词的边界
                for (auto it = sub_str.rbegin(); it != sub_str.rend(); ++it) {
                    --end;
                    // 寻找下一个词的边界   
                    if (utils::is_char_begin_boundary(*it)) {
                        break;
                    }
                }
            }

            if (!match_cur_token) {
                VLOG(6) << "word not find any matching token";
                found_token = false;
                break;
            }

            VLOG(6) << "cur token is: " << cur_token.value_;
            all_tokens.emplace_back(cur_token);
        }

        if (!found_token) {
            all_tokens.clear();
            VLOG(6) << "vocab size: " << this->vocab_.size() 
                    << ", unk_token: " << this->unk_token_;
            all_tokens.emplace_back(this->vocab_.at(this->unk_token_),
                                   this->unk_token_, 
                                   core::Offset{0, text.length()});
        }
    }


    return all_tokens;
}

core::Vocab WordPiece::get_vocab_from_file(const std::string &vocab_file) {
    std::cout << "WordPiece get_vocab_from_file, file name: " << vocab_file << std::endl;
    std::ifstream fin(vocab_file);
    core::Vocab vocab;
    if (!fin.is_open()) {
        std::cerr << "Can't open file: " << vocab_file << std::endl;
        return vocab;
    }

    constexpr int MAX_BUFFER_SIZE = 256;
    char word[MAX_BUFFER_SIZE];
    int idx = 0;
    while (fin.getline(word, MAX_BUFFER_SIZE)) {
        std::string word_str = word;
        // 去到句首的空格内容
        auto leading_spaces = word_str.find_first_not_of(WHITESPACE);
        if (std::string::npos != leading_spaces) {
            leading_spaces = std::min(leading_spaces, word_str.length() - 1);
            word_str = word_str.substr(leading_spaces); // 去掉句首空格
        }

        auto trailing_spaces = word_str.find_last_not_of(WHITESPACE);
        if (std::string::npos != trailing_spaces) {
            word_str = word_str.substr(0, trailing_spaces + 1); // 去掉句尾空格
        }

        // NOTE: TODO: 如果word_str之前存在过，则直接跳过
        if (!word_str.empty()) {
            vocab[word_str] = idx++;
        }
    }

    fin.close();
    std::cout << "WordPiece get_vocab_from_file done" << std::endl;
    return vocab;
}

WordPiece WordPiece::get_wordpiece_from_file( 
                    const std::string& vocab_file,
                    const std::string& unk_token,
                    size_t max_input_chars_per_word,
                    const std::string& continuing_subword_prefix) {
    std::cout << "WordPiece get_wordpiece_from_file" << std::endl;
    auto vocab = get_vocab_from_file(vocab_file);
    return WordPiece(vocab, 
                     unk_token, 
                     max_input_chars_per_word, 
                     continuing_subword_prefix);
}

bool WordPiece::token_to_id(const std::string& token, uint32_t *id) const {
    return true;
}

size_t WordPiece::get_vocab_size() const {
    return vocab_.size();
}

}   // namespace models
}   // namespace leomax_tokenizer