#include "pytokenizers.h"
#include "Python.h"
#include "../models/wordpiece.h"
#include "../models/bpe.h"
#include "../core/added_vocabulary.h"
#include "../normalizers/bert_normalizer.h"
#include "../normalizers/unicode_normalizer.h"
#include "utils.h"
#include "tl/optional.hpp"
namespace py = pybind11;    // 简化pybind11的命名空间

namespace leomax_tokenizer {
namespace pybind {

// void set_normalizer_wrapper(tokenizers::Tokenizer& self, 
//                             py::object *normalizer){
//     if (py::isinstance<normalizers::BertNormalizer>(*normalizer)) {
//         self.set_normalizer((*normalizer).cast<normalizers::BertNormalizer>());
//     }
// }

// void set_normalizer_wrapper(tokenizers::Tokenizer& self, 
//                             PyObject* normalizer) {
//     py::handle py_obj(normalizer);
//     if (py::type::of(py_obj).is(py::type::of<normalizers::BertNormalizer>())) {
//         const auto& normalizer = py_obj.cast<const normalizers::BertNormalizer&>();
//         self.set_normalizer(normalizer);
//     }
// }


void bind_tokenizers(pybind11::module* m) {
    auto submodule = m->def_submodule("tokenizers", "The tokenizers module");
    py::class_<tokenizers::Tokenizer>(*submodule, "Tokenizer")
        .def(py::init<>(), "Create tokenizer")
        .def(py::init<const models::WordPiece&>(), "Create WordPiece tokenizer")
        .def(py::init<const models::BPE&>(), "Create BPE tokenizer")
        .def("encode", 
             &tokenizers::Tokenizer::encode_single_text,
             py::arg("text"))
        .def("token_to_id", [](tokenizers::Tokenizer& self, 
                               const std::string& token)-> tl::optional<uint32_t> {
            uint32_t token_id;
            bool success = self.token_to_id(token, &token_id);
            if (success) {
                return token_id;
            } 

             return tl::nullopt;  // Return None
        })
        .def("add_special_tokens", [](tokenizers::Tokenizer& self, 
                                      const std::vector<std::string>& tokens) {
            std::vector<core::AddedToken> added_tokens;
            for (const auto& token : tokens) {
                added_tokens.push_back(core::AddedToken(token));
            }
            self.add_special_tokens(added_tokens);
        })
        .def_property("normalizer", 
                        [](const tokenizers::Tokenizer& self) {
                            Py_RETURN_NONE;
                        },
                        [](tokenizers::Tokenizer& self, py::handle normalizers) {
                            
                            if (py::isinstance<normalizers::BertNormalizer>(normalizers)) {
                                self.set_normalizer((normalizers).cast<normalizers::BertNormalizer>());
                            } else if (py::isinstance<normalizers::NFKCNormalizer>(normalizers)) {
                                self.set_normalizer((normalizers).cast<normalizers::NFKCNormalizer>());
                            }
                        });
    } //bind_tokenizers



}   // namespace pybind
}   // namespace leomax_tokenizer