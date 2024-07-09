add_subdirectory(models)

if (WITH_PYTHON)
    cc_library(core_tokenizers SHARED 
        SRCS pybind/pybind.cc pybind/utils.cc pybind/pynormalizer.cc pybind/pypretokenizers.cc
            pybind/pytoken.cc pybind/pymodels.cc models/bpe.cc models/wordpiece.cc
            pybind/pytokenizers.cc tokenizers/tokenizer.cc
            core/added_vocabulary.cc normalizers/unicode_normalizer.cc
            normalizers/normalizer.cc normalizers/bert_normalizer.cc normalizers/strip_normalizer.cc
            pretokenizers/pretokenizer.cc pretokenizers/bert_pretokenizer.cc pretokenizers/metaspace.cc
            DEPS pybind re2 icuuc icudata python)

    # 设置编译出来的库没有 "lib"前缀
    set_target_properties(core_tokenizers PROPERTIES PREFIX "")
    set_target_properties(core_tokenizers PROPERTIES SUFFIX ".so")
endif()