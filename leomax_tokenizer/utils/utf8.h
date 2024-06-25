#ifndef LEOMAX_TOKENIZER_UTILS_UTF8_H_
#define LEOMAX_TOKENIZER_UTILS_UTF8_H_
#include <string>
#include <cstdint>
#include <cstring>
#include "glog/logging.h"
namespace leomax_tokenizer {
namespace utils {

inline uint32_t unicode_to_utf8(uint32_t unchr) {
    uint32_t utf8 = 0;
    if (unchr < 0x00000080) {
        utf8 = unchr;
    } else if (unchr < 0x00000800) {
        utf8 = (unchr << 2) & 0x1F00;
        utf8 |= (unchr & 0x3F);
        utf8 |= 0x0000C080;
    } else if (unchr < 0x00010000) {
        utf8 = (unchr << 4) & 0x0F0000;   // upper 4 bits
        utf8 |= (unchr << 2) & 0x003F00;  // next 6 bits
        utf8 |= (unchr & 0x3F);           // last 6 bits
        utf8 |= 0x00E08080;
    } else if (unchr < 0x00110000) {      // 3-byte unicode
        utf8 = (unchr << 6) & 0x07000000;   // upper 3 bits
        utf8 |= (unchr << 4) & 0x003F0000;  // next 6 bits
        utf8 |= (unchr << 2) & 0x00003F00;  // next 6 bits
        utf8 |= (unchr & 0x3F);             // last 6 bits
        utf8 |= static_cast<uint32_t>(0xF0808080);
    }
    return utf8;
}

inline uint32_t bytes_in_unicode_char(uint32_t chr) {
    uint32_t count = 1;
    // no if-statements means no divergence
    count += static_cast<int>((chr & static_cast<uint32_t>(0x0000FF00)) > 0);
    count += static_cast<int>((chr & static_cast<uint32_t>(0x00FF0000)) > 0);
    count += static_cast<int>((chr & static_cast<uint32_t>(0xFF000000)) > 0);
    return count;
}

inline uint32_t bytes_in_utf8_char(uint8_t byte) {
    unsigned int count = 1;
    // no if-statements means no divergence
    count += static_cast<int>((byte & 0xF0) == 0xF0);
    count += static_cast<int>((byte & 0xE0) == 0xE0);
    count += static_cast<int>((byte & 0xC0) == 0xC0);
    count -= static_cast<int>((byte & 0xC0) == 0x80);
    return count;
}

inline uint32_t utf8_to_unicode(uint32_t utf8) {
    uint32_t unchr = 0;
    if (utf8 < 0x00000080) {
        unchr = utf8;
    } else if (utf8 < 0x0000E000) {
        unchr = (utf8 & 0x1F00) >> 2;
        unchr |= (utf8 & 0x003F);
    } else if (utf8 < 0x00F00000) {
        unchr = (utf8 & 0x0F0000) >> 4;
        unchr |= (utf8 & 0x003F00) >> 2;
        unchr |= (utf8 & 0x00003F);
    } else if (utf8 <= static_cast<uint32_t>(0xF8000000)) {
        unchr = (utf8 & 0x03000000) >> 6;
        unchr |= (utf8 & 0x003F0000) >> 4;
        unchr |= (utf8 & 0x00003F00) >> 2;
        unchr |= (utf8 & 0x0000003F);
    }
    return unchr;
}


inline uint32_t get_utf8_char_len(uint32_t u32chr) {
    return bytes_in_unicode_char(unicode_to_utf8(u32chr));
}

inline uint32_t utf8_to_uint32(const char* psrc, uint32_t* chr) {
    uint32_t chwidth = bytes_in_utf8_char(static_cast<uint32_t>(*psrc));
    *chr = static_cast<uint32_t>(*psrc++) & 0xFF;
    if (chwidth > 1) {
        *chr = (*chr) << 8;
        *chr |= (static_cast<uint32_t>(*psrc++) & 0xFF);  // << 8;
        if (chwidth > 2) {
            *chr = (*chr) << 8;
            *chr |= (static_cast<uint32_t>(*psrc++) & 0xFF);  // << 16;
            if (chwidth > 3) {
                *chr = (*chr) << 8;
                *chr |= (static_cast<uint32_t>(*psrc++) & 0xFF);  // << 24;
            }
        }
    }
    return chwidth;
}

inline uint32_t unicode_to_utf8_char(uint32_t chr, char* dst) {
    uint32_t chwidth = bytes_in_unicode_char(chr);
    for (uint32_t idx = 0; idx < chwidth; ++idx) {
        dst[chwidth - idx - 1] = static_cast<char>(chr & 0xFF);
        chr = chr >> 8;
    }
    return chwidth;
}

inline void get_utf8_str(const char32_t* unicode_str,
                    char* utf8_str,
                    size_t unicode_len) {
    char dst_char[5] = {0};
    for (size_t i = 0; i < unicode_len; ++i) {
        uint32_t utf8_uint32 = unicode_to_utf8 (unicode_str[i]);
        uint32_t utf8_char_count = unicode_to_utf8_char (utf8_uint32, dst_char);
        dst_char[utf8_char_count] = '\0';
        memcpy(utf8_str, dst_char, utf8_char_count);
        utf8_str += utf8_char_count;
    }
    *utf8_str = '\0';
}

inline uint32_t get_unicode_len_from_utf8(const char* psrc, size_t length) {
    VLOG(6) << "get the unicode data: " << psrc << ", length:" << length;
    size_t unicode_len = 0;
    size_t start = 0;
    while (start < length && psrc[start] != '\0') {
        size_t chwidth = bytes_in_utf8_char(psrc[start]);
        start += chwidth;
        ++unicode_len;
        VLOG(6) << "chwidth: " << chwidth 
                << ", start: " << start
                << ", unicode_len: " << unicode_len
                << ", length: " << length;
    } 
    VLOG(6) << "unicode_len: " << unicode_len;
    return unicode_len;
}

inline bool is_char_begin_boundary(char ch) {
    return ((~ch) >> 7) || ((ch & 0xC0) == 0xC0);
}

}
}
#endif