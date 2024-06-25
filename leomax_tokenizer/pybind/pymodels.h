//
// Created by xiongxinlei on 5/27/24.
//

#ifndef PYBIND_PYMODELS_H
#define PYBIND_PYMODELS_H
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../models/models.h"
#include "../models/bpe.h"
#include "../models/wordpiece.h"

namespace leomax_tokenizer {
namespace pybind {

class PyModel : public models::Model {
public:
    virtual std::vector<core::Token> tokenize(const std::string& tokens) override {
        PYBIND11_OVERRIDE_PURE(
          std::vector<core::Token>,
          models::Model,
          tokenize,
          tokens);
    }
};


class PyBPE : public models::BPE {
virtual std::vector<core::Token> tokenize (
    const std::string& tokens) override {
    PYBIND11_OVERRIDE_PURE(
        std::vector<core::Token>,
        models::BPE,
        tokenize,
        tokens);
}
};

class PyWordPiece : public models::WordPiece {
public:
    using WordPiece::WordPiece;
    virtual std::vector<core::Token> tokenize(const std::string& text) override {
        PYBIND11_OVERRIDE_PURE(
          std::vector<core::Token>,
          models::WordPiece,
          tokenize,
          text
          );
    }
};

void bind_models(pybind11::module* m);

}
}
#endif  // PYBIND_PYMODELS_H
