#ifndef LEOMAX_TOKENIZER_NORMALIZERS_NORMALIZER_H_
#define LEOMAX_TOKENIZER_NORMALIZERS_NORMALIZER_H_
#include <string>
#include <vector>
#include "../core/base.h"
#include <algorithm>
#include "re2/re2.h"
namespace leomax_tokenizer {
namespace normalizers {

struct OffsetMapping {
    std::u32string u32normalized;
    std::vector<int> changes;  // Same size as normalized
};

/*****************************规整之后的字符串********************************/
class NormalizedString {
public:
    NormalizedString() = default;
    NormalizedString(const std::string& original);
    const std::string& get_str() const {
        return this->normalized_;
    }

    NormalizedString(NormalizedString&& other);
    NormalizedString(const NormalizedString& other) = default;
    NormalizedString& operator=(NormalizedString&& other);
    NormalizedString& operator=(const NormalizedString& other) = default;
    const std::string& get_original_str() const {
        return this->original_;
    }

    bool slice(core::Range range, NormalizedString* normalized, bool origin_range) const;
    bool convert_offsets(core::Range* range, bool origin_range) const;
    size_t get_len() const {
        return this->normalized_.length();
    }

    size_t get_original_len() const {   
        return this->original_.length();
    }

    NormalizedString& filter_char(std::function<bool(char32_t)> keep_char_fn);

    void update_normalized(const OffsetMapping& new_normalized,
                                        uint32_t initial_offset) {
        update_normalized_range(new_normalized, initial_offset, 
                               {0, this->get_len()}, true);
        
    }

    void update_normalized_range(const OffsetMapping& new_normalized,
                                uint32_t initial_offset, core::Range range,
                                bool origin_range);
    NormalizedString& map_char(std::function<char32_t(char32_t)> map_char_fn);
    NormalizedString& lower_case();

    NormalizedString& NFD();
    NormalizedString& NFKC();

    void run_normalization(const std::string& mode);

    NormalizedString& replace(const re2::RE2& pattern,
                              const std::string& content);
public:
    std::string original_;      // 原始字符串
    std::string normalized_;    // 规整之后的字符串
    std::vector<core::Range> alignments_;   // 规整之后的字符串与原始字符串的对应关系
    uint32_t original_shift_;   // 
};

struct Normalizer { 
    virtual void operator()(NormalizedString* mut_str) const = 0;
};

class NFKDNormalizer : public Normalizer {
public: 
    virtual void operator()(NormalizedString* input) const override;
};

}       // namespace normalizers
}       // namespace leomax_tokenizer
#endif