#ifndef LEOMAX_TOKENIZER_PRETOKENIZERS_PRETOKENIZER_H_
#define LEOMAX_TOKENIZER_PRETOKENIZERS_PRETOKENIZER_H_
#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include "../core/encoding.h"
#include "../core/base.h"
#include "../normalizers/normalizer.h"

namespace leomax_tokenizer {
namespace pretokenizers {

/*****************************被分词的文本**********************************/
class StringSplit {
public:
    StringSplit() = default;
    StringSplit(normalizers::NormalizedString&& normalized) :
            normalized_(std::move(normalized)) {
        std::cout << "StringSplit(normalizers::NormalizedString&& normalized): "
                  << normalized_.get_original_str() << std::endl;
    }

    StringSplit(const normalizers::NormalizedString& normalized) :
            normalized_(normalized) {

    }

    StringSplit(const StringSplit& other) = default;
    StringSplit(const normalizers::NormalizedString& normalized, 
                const std::vector<core::Token>& tokens) :
            normalized_(normalized),
            tokens_(tokens) {
    }

    // 所有涉及到右值引用的类，都要构建右值引用构造函数与赋值运算符
    StringSplit(StringSplit&& other)
      : tokens_(std::move(other.tokens_)),
        normalized_(std::move(other.normalized_)) {

    }

    StringSplit& operator=(const StringSplit& other) = default;

    StringSplit& operator=(StringSplit&& other) {
        this->tokens_ = std::move(other.tokens_);
        this->normalized_ = std::move(other.normalized_);

        return *this;
    }

public:
    normalizers::NormalizedString normalized_;
    std::vector<core::Token> tokens_;       // 分词结果
};

class PreTokenizedString {
public:
    PreTokenizedString() = default;
    PreTokenizedString(const std::string& original);
    PreTokenizedString(const normalizers::NormalizedString& normalized);
    // 使用了StringSplit有右值operator=的重载，这里也一定要有右值重载运算符
    PreTokenizedString& operator=(PreTokenizedString&& other);

    void set_original_str(const std::string& orignal);
    const std::string& get_original_str() const {
        return this->original_;
    }
    void split(std::function<void(int idx, 
                                  normalizers::NormalizedString*,
                                  std::vector<StringSplit>*)> split_fn);
    void tokenize(
        std::function<std::vector<core::Token>(normalizers::NormalizedString*)>
         tokenize_fn);

    StringSplit get_split(int idx);

    size_t get_split_size() const {
        return this->splits_.size();
    }
    std::vector<std::tuple<std::string, core::Offset, std::vector<core::Token>>> 
       get_splits(bool is_original, const core::OffsetType& offset_type) const;

    bool transform_to_encoding(const std::vector<uint32_t>& input_word_idx,
                              uint32_t type_id,
                              core::OffsetType offset_type,
                              core::Encoding* encoding) const;

public:
    std::string original_;                  // 分词前的原始文本
    std::vector<StringSplit> splits_;
};

struct PreTokenizer {
    virtual void operator()(PreTokenizedString* pre_tokenized) const = 0;
};



}       // namespace pretokenizers
}       // namespace leomax_tokenizer
#endif