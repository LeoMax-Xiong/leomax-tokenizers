//
// Created by xiongxinlei on 5/27/24.
//

#include "pymodels.h"
#include "pytokens.h"
#include "../core/cache.h"
#include "glog/logging.h"
namespace py = pybind11;
namespace leomax_tokenizer {
namespace pybind {

void bind_models(pybind11::module* m) {
    auto submodule = m->def_submodule("models", "The models module");
    py::class_<models::Model, PyModel/*辅助类*/>(submodule, "Model")
        .def(py::init<>())
        .def("tokenize", &models::Model::tokenize);


    /*********************************************************************/
    /**********************************BPE模型*****************************/
    /*********************************************************************/

    py::class_<models::BPE, PyBPE/*辅助类*/>(submodule, "BPE")
        .def(py::init<>())
        .def(py::init([](const py::object& py_vocab, 
                         const py::object& py_merges,
                         const py::object& py_cache_capacity,
                         const py::object& py_dropout,
                         const py::object& py_unk_token,
                         const py::object& py_continuing_subword_prefix,
                         const py::object& py_end_of_word_suffix,
                         const py::object& py_fuse_unk){
                // 如果词典非空，则将py::object转为core::Vocab
                core::Vocab vocab;
                if (!py_vocab.is(py::none())) {
                    vocab = py_vocab.cast<core::Vocab>();
                }

                // 如果合并规则费控，则将py::object转为core::Merges
                core::Merges merges;
                if (!py_merges.is(py::none())) {
                    merges = py_merges.cast<core::Merges>();
                }

                // 如果缓存容量非空，则将py::object转为size_t
                size_t cache_capacity = core::DEFAULT_CACHE_CAPACITY;
                if (!py_cache_capacity.is(py::none())) {
                    cache_capacity = py_cache_capacity.cast<size_t>();
                }

                // 如果丢失率非空，则将py::object转为float
                std::vector<float> dropout;
                if (!py_dropout.is(py::none())) {
                    dropout.emplace_back(py_dropout.cast<float>());
                }

                // 如果未知词非空，则将py::object转为std::string
                std::vector<std::string> unk_token;
                if (!py_unk_token.is(py::none())) {
                    unk_token.emplace_back(py_unk_token.cast<std::string>());
                }

                // 如果连续子词前缀非空，则将py::object转为std::string
                std::vector<std::string> continuing_subword_prefix;
                if (!py_continuing_subword_prefix.is(py::none())) {
                    continuing_subword_prefix.emplace_back(
                        py_continuing_subword_prefix.cast<std::string>()
                    );
                }

                // 如果单词结尾后缀非空，则将py::object转为std::string
                std::vector<std::string> end_of_word_suffix;
                if (!py_end_of_word_suffix.is(py::none())) {
                    end_of_word_suffix.emplace_back(
                        py_end_of_word_suffix.cast<std::string>()
                    );
                }

                // 如果融合未知词非空，则将py::object转为bool
                bool fuse_unk = false;
                if (!py_fuse_unk.is(py::none())) {
                    fuse_unk = py_fuse_unk.cast<bool>();
                }

                models::BPE self(vocab, merges, cache_capacity,
                                 dropout, unk_token, continuing_subword_prefix, 
                                 end_of_word_suffix, fuse_unk);
                return self;
            }),
             py::arg("vocab") = py::none(),
             py::arg("merges") = py::none(),
             py::arg("cache_capacity") = py::none(),
             py::arg("dropout") = py::none(),
             py::arg("unk_token") = py::none(),
             py::arg("continuing_subword_prefix") = py::none(),
             py::arg("end_of_word_suffix") = py::none(),
             py::arg("fuse_unk") = py::none())
        .def("tokenize", &models::BPE::tokenize)
        .def_static("read_file",
                   [](const std::string& vocab_path, const std::string& merges_path) {
                core::Vocab vocab;
                core::Merges merges;
                models::BPE::get_vocab_and_merges_from_file(
                vocab_path, merges_path, &vocab, &merges);
                return py::make_tuple(vocab, merges);
            },
        py::arg("vocab"),
        py::arg("merges")
        );

    py::class_<models::WordPiece, PyWordPiece/*辅助类*/>(submodule, "WordPiece")
        .def(py::init<>())
        .def(py::init<const core::Vocab&,
                      const std::string&, 
                      size_t,
                      const std::string&,
                      bool>(),
            py::arg("vocab"),
            py::arg("unk_token") = "[UNK]",
            py::arg("max_input_chars_per_word") = 100,
            py::arg("continuing_subword_prefix") = "##",
            py::arg("handle_chinese_chars") = true) 
        .def("tokenize", &models::WordPiece::tokenize)
        .def_static("from_file",
                    &models::WordPiece::get_wordpiece_from_file, 
                    py::arg("vocab"), 
                    py::arg("unk_token") = "[UNK]",
                    py::arg("max_input_chars_per_word") = 100,
                    py::arg("continuing_subword_prefix") = "##")
        .def_static("read_file", 
                    &models::WordPiece::get_vocab_from_file, 
                    py::arg("vocab"));
}

} //pybind
} // leomax_tokenizer