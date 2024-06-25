#include "utils.h"

namespace leomax_tokenizer {
namespace pybind {
PyObject* to_pyobject(uint32_t value) {
    return PyLong_FromUnsignedLong(value);
}
}
}