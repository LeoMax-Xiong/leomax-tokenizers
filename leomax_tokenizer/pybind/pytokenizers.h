#ifndef LEOMAX_TOKENIZER_PYTOKENIZERS_H_
#define LEOMAX_TOKENIZER_PYTOKENIZERS_H_
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../tokenizers/tokenizer.h"
namespace leomax_tokenizer {
namespace pybind {


void bind_tokenizers(pybind11::module* m);
}   // namespace pybind
}   // namespace leomax_tokenizer


#endif