#include "bert_normalizer.h"
#include "../utils/unicode_utils.h"
#include <cstdint>
#include <codecvt>
#include <cstring>
#include <locale>
#include "glog/logging.h"

namespace leomax_tokenizer {
namespace normalizers {

BertNormalizer::BertNormalizer(bool clean_text,
                             bool handle_chinese_chars,
                             bool strip_accents,
                             bool lowercase) :
    clean_text_(clean_text),
    handle_chinese_chars_(handle_chinese_chars),
    strip_accents_(strip_accents),
    lowercase_(lowercase) {

}    

void BertNormalizer::operator() (NormalizedString *input) const {
    if (this->clean_text_) {
        // 移除不可见符号
        do_clean_text(input);
    }

    if (this->handle_chinese_chars_) {
        // 将中文符号前后添加空格
        do_handle_chinese_chars(input);
    }

    if (this->strip_accents_) {
        
    }

    if (this->lowercase_) {
        input->lower_case();
    }
}

void BertNormalizer::do_clean_text(NormalizedString *input) const {
    (*input).filter_char([](char32_t ch) -> bool {
        return !(ch == 0 || ch == 0xffd || unicode::is_control(ch));
    })
    .map_char([](char32_t ch) -> char32_t { // 将不可见符号转换为空格
        if (unicode::is_whitespace(ch)) {
            return ' ';
        }

        return ch;
    });
}

void BertNormalizer::do_handle_chinese_chars(NormalizedString *input) const {
    VLOG(6) << "start to hanlde chinese chars";
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    std::u32string u32input = conv.from_bytes(input->get_str());
    std::u32string u32output;
    std::vector<int> changes;
    u32output.reserve(u32input.length() * 3);
    changes.reserve(u32input.length() * 3);
    for (int i = 0; i < u32input.length(); ++i) {
        VLOG(6) << "current index: " << i << ", current char: ";
        if (unicode::is_chinese_char(u32input[i])) {
            u32output.push_back(' ');
            u32output.push_back(u32input[i]);
            u32output.push_back(' ');
            changes.push_back(0);
            changes.push_back(1);
            changes.push_back(1);
        } else {
            u32output.push_back(u32input[i]);
            changes.push_back(0);
        }
    }
    OffsetMapping new_normalized_offset{u32output, changes};
    input->update_normalized(new_normalized_offset, 0);
}

}
}