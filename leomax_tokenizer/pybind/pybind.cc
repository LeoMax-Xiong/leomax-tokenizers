#include <Python.h>
#include <pybind11/pybind11.h>

#include "pytokens.h"
#include "pymodels.h"
#include "pytokenizers.h"
#include "pynormalizer.h"
#include "pypretokenizers.h"
namespace leomax_tokenizer {
namespace pybind {
//namespace py = pybind11;

int add(int i, int j) {
  return i + j;
}

PYBIND11_MODULE(core_tokenizers, m) {
    m.doc() = "pybind11 leomax tokenizer core module";
	m.def("add", &add, "A function which adds two number");
    
	bind_normalizers(&m);

    bind_pretokenizers(&m);

    bind_models(&m);

    bind_tokenizers(&m);
}

} // namespace pybind
} // namespace leomax_tokenizer


