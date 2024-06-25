#ifndef LEOMAX_TOKENIZER_UTILS_UNICODE_UTILS_H_
#define LEOMAX_TOKENIZER_UTILS_UNICODE_UTILS_H_

#include "unicode/uchar.h"
#include "unicode/unistr.h"
namespace leomax_tokenizer {
namespace unicode {

inline bool is_control(int ch) {
    if (ch == '\t' || ch == '\n' || ch == '\r') {
        return false;// 这里是false???
    }

    return !u_isprint(ch);
}

inline bool is_whitespace(int ch) {
    const std::string WHITESPACE = " \n\r\t\f\v";
    for (int i = 0; i < WHITESPACE.length(); ++i) {
        if (ch == WHITESPACE[i]) return true;
    }
    return u_isspace(ch);
}

inline bool is_chinese_char(int ch) {
  return (
      (ch >= 0x4E00 && ch <= 0x9FFF) || (ch >= 0x3400 && ch <= 0x4DBF) ||
      (ch >= 0x20000 && ch <= 0x2A6DF) || (ch >= 0x2A700 && ch <= 0x2B73F) ||
      (ch >= 0x2B740 && ch <= 0x2B81F) || (ch >= 0x2B820 && ch <= 0x2CEAF) ||
      (ch >= 0xF900 && ch <= 0xFAFF) || (ch >= 0x2F800 && ch <= 0x2FA1F));
}

}
}

#endif