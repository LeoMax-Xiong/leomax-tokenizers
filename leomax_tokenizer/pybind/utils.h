#ifndef LEOMAX_TOKENIZER_PYBIND_UTILS_H_
#define LEOMAX_TOKENIZER_PYBIND_UTILS_H_

#include <Python.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace leomax_tokenizer {
namespace pybind {

PyObject* to_pyobject(uint32_t value);

}
}
#endif