#include "pynormalizer.h"

namespace py = pybind11;

namespace leomax_tokenizer {
namespace pybind {
    
void bind_normalizers(pybind11::module *m) {
    auto submodule = m->def_submodule("normalizers");
    py::class_<normalizers::Normalizer, PyNormalizer>(submodule, "Normalizer")
        .def(py::init<>())
        .def("__call__", 
            &normalizers::Normalizer::operator());

    
	py::class_<normalizers::BertNormalizer, PyBertNormalizer>(submodule,
																"BertNormalizer")
		.def(py::init<bool, bool, bool, bool>(),
			py::arg("clean_text") = true,
			py::arg("handle_chinese_chars") = true,
			py::arg("strip_accents") = true,
			py::arg("lowercase") = true)
        .def(py::init([](bool clean_text, 
                         bool handle_chinese_chars,
                         bool strip_accents,
                         bool lowercase) {
            return std::unique_ptr<normalizers::BertNormalizer>(
                 new normalizers::BertNormalizer(clean_text, 
                                                handle_chinese_chars, 
                                                strip_accents, 
                                                lowercase));
            } ),
             py::arg("clean_text") = true,
             py::arg("handle_chinese_chars") = true,
             py::arg("strip_accents") = true,
             py::arg("lowercase") = true)
        .def("__call__",
             &normalizers::BertNormalizer::operator());

    py::class_<normalizers::NFKDNormalizer, PyNFKDNormalizer>(submodule, "NFKDNormalizer")
        .def(py::init<>())
        .def("__call__", &normalizers::NFKDNormalizer::operator());

    py::class_<normalizers::NFKCNormalizer, PyNFKCNormalizer>(submodule, "NFKCNormalizer") 
        .def(py::init<>())
        .def("__call__", &normalizers::NFKCNormalizer::operator());
}

} // namespace name

}