//
// Created by xiongxinlei on 5/27/24.
//

#ifndef MODELS_MODELS_H
#define MODELS_MODELS_H
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include "../core/base.h"

namespace leomax_tokenizer {
namespace models {
class Model {
public:
    virtual std::vector<core::Token> tokenize(const std::string& tokens) = 0;
    virtual bool token_to_id(const std::string& token, uint32_t* id) const {
        return true;
    }

    virtual size_t get_vocab_size() const { return 0;}
};

}
}
#endif  // MODELS_MODELS_H
