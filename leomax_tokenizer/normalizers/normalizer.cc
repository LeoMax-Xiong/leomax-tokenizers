#include <iostream>
#include "normalizer.h"
#include <codecvt>
#include <locale>
#include <cstring>
#include "glog/logging.h"
#include "../utils/utf8.h"
#include "unicode/edits.h"
#include "unicode/errorcode.h"
#include "unicode/normalizer2.h"
#include "unicode/uchar.h"
#include "unicode/utypes.h"
namespace leomax_tokenizer {
namespace normalizers {
NormalizedString::NormalizedString(const std::string& original) :
                original_(original),
                normalized_(original),
                original_shift_(0) { 
    std::cout << "==================== Normalized String Constructor ===============" << std::endl;
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    std::u32string u32normalized = conv.from_bytes(normalized_);
    std::cout << "u32normalized: " << u32normalized.length() << std::endl;
    for (int i = 0; i < u32normalized.length(); ++i) {
        auto new_normalized_char_len = utils::get_utf8_char_len(u32normalized[i]);
        uint32_t start = 0;
        uint32_t end = 0;
        if (i != 0) {
            start = alignments_.back().second;
        }
        end = start + new_normalized_char_len;
        std::cout << "u32 normalized start: " << start << ", end: " << end << std::endl;
        for (int j = 0; j < new_normalized_char_len; ++j) {
            alignments_.push_back({start, end});
        }
    }

    std::cout << "alignments: " << alignments_.size() << std::endl;
    for (auto itr = alignments_.begin(); itr != alignments_.end(); ++itr){
        std::cout << "alignments: " << itr->first << ", " << itr->second << std::endl;
    }
    
}
NormalizedString::NormalizedString(NormalizedString&& other):
    original_(std::move(other.original_)),
    normalized_(std::move(other.normalized_)),
    alignments_(std::move(other.alignments_)),
    original_shift_(other.original_shift_) {

}

NormalizedString&  NormalizedString::operator=(NormalizedString&& other) {
    original_ = std::move(other.original_);
    normalized_ = std::move(other.normalized_);
    alignments_ = std::move(other.alignments_);
    original_shift_ = other.original_shift_;
    return *this;
}

bool NormalizedString::slice(core::Range range, NormalizedString* normalized, bool origin_range) const {
    std::cout << "normalized string slice " << std::endl;
    core::Range normalized_range = range;
    core::Range original_range = range;
    std::cout << "slice origin range: " << origin_range << std::endl;
    if (origin_range){ 
        convert_offsets(&normalized_range, true);
    } else {
        convert_offsets(&original_range, false);
    }

    uint32_t n_shift = original_range.first;
    // 限制 original_range.first 不能超过原始长度
    original_range.first =
        (std::min)(original_range.first,
                   static_cast<uint32_t>(this->original_.length() - 1));
    std::string old_original = normalized->original_;
    normalized->original_ = this->original_.substr(
                    original_range.first, original_range.second - original_range.first);
    VLOG(6) << "updated the normalized string from: [ " 
             << old_original << " ] to : [ " 
             << normalized->original_ << " ]";

    // 重新设置 normalized_range 的first
    normalized_range.first =
        (std::min)(normalized_range.first,
                   static_cast<uint32_t>(this->normalized_.length() - 1));

    normalized->normalized_ = this->normalized_.substr(
                                    normalized_range.first,
                                    normalized_range.second - normalized_range.first);
    VLOG(6) << "updated the normalized string range second: " << normalized_range.second;
    normalized->alignments_.reserve(normalized_range.second -
                                    normalized_range.first);

    for (uint32_t i = normalized_range.first; i < normalized_range.second;
         ++i) {
        normalized->alignments_.emplace_back(
                        this->alignments_[i].first - n_shift,
                        this->alignments_[i].second - n_shift);
    }
    VLOG(6) << "update the alignments size: " << normalized->alignments_.size();
    normalized->original_shift_ = this->original_shift_ + original_range.first;
    VLOG(6) << "update the original shift: " << normalized->original_shift_;
    return true;

}

bool NormalizedString::convert_offsets(core::Range* range, 
                                       bool origin_range) const {
    std::cout << "convert offsets" << std::endl;
    auto len_original = get_original_len();
    auto len_normalized = get_len();
    std::cout << "len original: " << len_original << std::endl;
    std::cout << "len normalized: " << len_normalized << std::endl;
    std::cout << "range: [ " << range->first << ", " << range->second << " ]" << std::endl;
    VLOG(6) << "origin range: " << origin_range;
    if (range->first == range->second) {
        return true;
    }

    if (range->first > range->second) {
        return false;
    }

    if (origin_range && original_.empty() && 
        (range->first == 0 && range->second == 0)) {
        range->second = len_normalized;
        return true;
    }

    if (!origin_range && normalized_.empty() && 
        (range->first == 0 && range->second == 0)) {
        range->second = len_original;
        return true;
    }

    if (origin_range) {
        int start = -1;
        int end = -1;
        for (int i = 0; i < alignments_.size(); ++i) {
            if (range->second >= alignments_[i].second) {
                if (start < 0 && range->first <= alignments_[i].first) {
                    if (alignments_[i].first != alignments_[i].second) {
                        start = i;
                    }
                }
                if (range->second >= alignments_[i].second) {
                    end = i + 1;
                }
            }
        }
        if (start > 0 && end < 0) {
            *range = {start, start};
        } else if (start < 0 && end > 0) {
            *range = {end, end};
        } else if (start > 0 && end > 0) {
            *range = {start, end};
        } else {
            return false;
        }
    } else {
        std::cout << "convert offsets alignments size: " << alignments_.size() << std::endl;
        range->first = alignments_[range->first].first;
        range->second = alignments_[range->second - 1].second; // [start, end)
    }

    std::cout << "convert offsets finished" << std::endl;
    return true;
}

NormalizedString& NormalizedString::filter_char(std::function<bool(char32_t)> keep_char_fn) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    std::u32string u32new_normalized;
    u32new_normalized.reserve(this->normalized_.length());
    uint32_t removed_start;
    uint32_t removed = 0;
    std::vector<int> changes;

    changes.reserve(normalized_.length());
    bool has_init_ch = false;
    uint32_t last_char;
    uint32_t curr_char;

    size_t utf8_len = 0;
    while (utf8_len < normalized_.length()) {
        auto chwidth =
            utils::utf8_to_uint32(normalized_.data() + utf8_len, &curr_char);
        curr_char = utils::utf8_to_unicode(curr_char);
        if (keep_char_fn(curr_char)) {
            if (has_init_ch) {
                u32new_normalized.push_back(last_char);
                changes.push_back(-removed);
            } else {
                has_init_ch = true;
                removed_start = removed;
            }
            last_char = curr_char;
            removed = 0;
        } else {
            removed += 1; 
        }
        utf8_len += chwidth;
    }

    if (has_init_ch) {
        u32new_normalized.push_back(last_char);
        changes.push_back(-removed);
    }
    OffsetMapping new_normalized_offset{u32new_normalized, changes};
    // Update normalized_ and alignments_
    // UpdateNormalized(new_normalized_offset, removed_start);
    update_normalized(new_normalized_offset, removed_start);
    return *this;

}

NormalizedString& NormalizedString::map_char(std::function<char32_t(char32_t)> map_char_fn) {
    size_t utf8_len = 0;
    std::u32string u32normalized;
    uint32_t curr_char;
    u32normalized.reserve(normalized_.length());
    while (utf8_len < normalized_.length()) {
        auto chwidth =
            utils::utf8_to_uint32(normalized_.data() + utf8_len, &curr_char);
        curr_char = utils::utf8_to_unicode(curr_char);
        curr_char = map_char_fn(curr_char);
        u32normalized.push_back(curr_char);
        utf8_len += chwidth;
    }
    std::vector<int> changes(u32normalized.size(), 0);
    update_normalized({u32normalized, changes}, 0);
    return *this;
}

void NormalizedString::update_normalized_range(const OffsetMapping& new_normalized,
                            uint32_t initial_offset, core::Range range,
                            bool origin_range) {
    auto n_range = range;
    if (origin_range) {
        convert_offsets(&n_range, origin_range);
    }

    // Retrieve the original characters that are being replaced. This let us
    // compute the change in byte sizes along the way.
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    n_range.first = (std::min)(n_range.first,
                                static_cast<uint32_t>(normalized_.length() - 1));

    std::u32string u32replaced_normalized = conv.from_bytes(
        normalized_.substr(n_range.first, n_range.second - n_range.first));
    uint32_t initial_removed = 0;

    // calculate initial_removed
    for (int i = 0; i < initial_offset; ++i) {
        size_t chwidth = utils::get_utf8_char_len(u32replaced_normalized[i]);
        initial_removed += chwidth;
    }

    uint32_t offset = initial_removed + n_range.first;
    std::vector<core::Range> alignments;
    alignments.reserve(n_range.second - n_range.first);

    int replaced_normalized_idx = initial_removed;
    // Calculate the new alignments
    for (int i = 0; i < new_normalized.u32normalized.length(); ++i) {
        auto idx = offset;
        core::Range align;
        int curr_changes = new_normalized.changes[i];
        if (curr_changes > 0) {
            // Insert a char
            if (idx < 1) {
                align = {0, 0};
            } else {
                align = alignments_[idx - 1];
            }
        } else {
            align = alignments_[idx];
        }
        char32_t new_normalized_char = new_normalized.u32normalized[i];
        auto new_normalized_char_len = utils::get_utf8_char_len(new_normalized_char);
        char32_t replaced_char = -1;
        if (curr_changes <= 0) {
            replaced_char = u32replaced_normalized[replaced_normalized_idx++];
        }
        uint32_t replaced_char_size =
            (replaced_char == -1) ? 0 : utils::get_utf8_char_len(replaced_char);

        uint32_t total_bytes_to_remove = 0;
        if (curr_changes < 0) {
            for (int j = 0; j < -curr_changes; ++j) {
                replaced_char = u32replaced_normalized[replaced_normalized_idx++];
                total_bytes_to_remove += utils::get_utf8_char_len(replaced_char);
            }
        }
        offset += replaced_char_size + total_bytes_to_remove;
        alignments.insert(alignments.end(), new_normalized_char_len, align);
    }
    // Replace the old alignments in n_range
    if (n_range.second - n_range.first >= alignments.size()) {
        std::memcpy(alignments_.data() + n_range.first,
                    alignments.data(),
                    alignments.size() * sizeof(core::Range));
        alignments_.erase(alignments_.begin() + n_range.first + alignments.size(),
                        alignments_.begin() + n_range.second);
    } else {
        std::vector<core::Range> new_alignments;
        auto third_len = 0;
        if (alignments_.size() > n_range.second) {
            third_len = alignments_.size() - n_range.second;
        }
        new_alignments.resize(n_range.first + alignments.size() + third_len);
        if (n_range.first > 0) {
            std::copy_n(alignments_.begin(), n_range.first, new_alignments.begin());
        }
        std::copy_n(alignments.begin(),
                    alignments.size(),
                    new_alignments.begin() + n_range.first);
        if (third_len > 0) {
            std::copy_n(alignments_.begin() + n_range.second,
                    third_len,
                    new_alignments.begin() + n_range.first + alignments.size());
        }
        alignments_ = std::move(new_alignments);
    }
    // Unicode -> UTF8
    uint32_t normalized_utf8_size = 0;
    for (auto& ch : new_normalized.u32normalized) {
        normalized_utf8_size += utils::get_utf8_char_len(ch);
    }
    std::vector<char> utf8_str(normalized_utf8_size + 1);
    utils::get_utf8_str(new_normalized.u32normalized.data(),
                        utf8_str.data(),
                        new_normalized.u32normalized.length());

    // Update normalized_
    auto normalized_iter = normalized_.begin();
    normalized_.replace(normalized_iter + n_range.first,
                        normalized_iter + n_range.second,
                        utf8_str.data(),
                        normalized_utf8_size);
    
} 


NormalizedString& NormalizedString::lower_case() {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    std::u32string u32normalized = conv.from_bytes(normalized_);
    // Can cover all single char covert cases
    for (int i = 0; i < u32normalized.length(); ++i) {
        u32normalized[i] = u_tolower(u32normalized[i]);
    }
    // No need to update normalized range
    normalized_ = conv.to_bytes(u32normalized);
    return *this;
}


NormalizedString& NormalizedString::NFD() {
    run_normalization("NFD");
    return *this;
}

void NormalizedString::run_normalization(const std::string& mode) {
    icu::ErrorCode icu_error;
    const icu::Normalizer2* normalizer = nullptr;
    if ("NFKC" == mode) {
        normalizer = icu::Normalizer2::getNFKCInstance(icu_error);
    }

    std::string normalized_result;
    icu::Edits edits;
    icu::StringByteSink<std::string> byte_sink(&normalized_result);
    normalizer->normalizeUTF8(
        0,
        icu::StringPiece(normalized_.data(), normalized_.length()),
        byte_sink,
        &edits,
        icu_error);
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    std::u32string u32new_normalized = conv.from_bytes(normalized_result);

    // Set changes
    std::vector<int> changes;
    changes.reserve(u32new_normalized.length());
    auto iter = edits.getFineIterator();
    int old_offset = 0;
    int new_offset = 0;

    // The edits record the byte level modification, so need to transform to char
    // level
    // using GetUnicodeLenFromUTF8
    while (iter.next(icu_error)) {
        auto old_length = iter.oldLength();
        auto new_length = iter.newLength();
        auto new_unicode_len = utils::get_unicode_len_from_utf8(
            normalized_result.data() + new_offset, new_length);
        auto old_unicode_len = utils::get_unicode_len_from_utf8(
            normalized_.data() + old_offset, old_length);
        old_offset += old_length;
        new_offset += new_length;
        if (old_unicode_len == new_unicode_len) {
            // Just replace the char
            changes.insert(changes.end(), old_unicode_len, 0);
        } else if (old_unicode_len < new_unicode_len) {
            // Insert the char
            changes.insert(changes.end(), old_unicode_len, 0);
            changes.insert(changes.end(), new_unicode_len - old_unicode_len, 1);
        } else /* old_length > new_length */ {
            // Remove the char
            if (new_unicode_len > 1) {
                changes.insert(changes.end(), new_unicode_len - 1, 0);
            }
            changes.push_back(new_unicode_len - old_unicode_len);
        }
    }
    OffsetMapping new_normalized_offset{u32new_normalized, changes};
    // Update normalized_ and alignments_
    update_normalized(new_normalized_offset, 0);    
}

void NFKDNormalizer::operator()(NormalizedString* input) const {
  input->filter_char([](char32_t ch) -> bool {
    if ((ch >= 0x0001 && ch <= 0x0008) || (ch == 0x000B) ||
        (ch >= 0x000E && ch <= 0x001F) || (ch == 0x007F) || (ch == 0x008F) ||
        (ch == 0x009F)) {
      return false;
    }
    return true;
  });

  input->map_char([](char32_t ch) -> char32_t {
    if ((ch == 0x0009) || (ch == 0x000A) || (ch == 0x000C) || (ch == 0x000D) ||
        (ch == 0x1680) || (ch >= 0x200B && ch <= 0x200F) || (ch == 0x2028) ||
        (ch == 0x2029) || (ch == 0x2581) || (ch == 0xFEFF) || (ch == 0xFFFD)) {
      return ' ';
    }
    return ch;
  });
}

 NormalizedString& NormalizedString::NFKC() {
    run_normalization("NFKC");
    return *this;
 }


NormalizedString& NormalizedString::replace(const re2::RE2& pattern,
                                            const std::string& content) {
    re2::StringPiece result;
    size_t start = 0;
    size_t end = normalized_.length();
    int64_t offset = 0;

    std::u32string u32content;
    u32content.reserve(content.size());
    std::vector<int> changes;
    changes.reserve(content.size());

    size_t content_utf8_len = 0;
    while (content_utf8_len < content.length()) {
        uint32_t content_char;
        auto content_char_width =
            utils::utf8_to_uint32 (content.data() + content_utf8_len, &content_char);
        content_char = utils::utf8_to_unicode(content_char);
        u32content.push_back(content_char);
        changes.push_back(1);
        content_utf8_len += content_char_width;
    }
    size_t new_len = content.length();

    OffsetMapping new_normalized{u32content, changes};

    while (pattern.Match(normalized_, start, end, RE2::UNANCHORED, &result, 1)) {
        size_t curr_start = result.data() - normalized_.data();
        size_t old_len = result.length();
        size_t curr_end = curr_start + old_len;
        size_t removed_chars =
            utils::get_unicode_len_from_utf8(normalized_.data() + curr_start, old_len);
        update_normalized_range(
            new_normalized, removed_chars, {curr_start, curr_end}, false);
        offset = new_len - old_len;
        // update start
        start = curr_end;
        if (offset >= 0) {
            start = curr_end + offset;
        } else {
            size_t uoffset = -offset;
            start = (curr_end >= uoffset) ? curr_end - uoffset : 0;
        }
        end = normalized_.length();
    }
    return *this;
}

}
}