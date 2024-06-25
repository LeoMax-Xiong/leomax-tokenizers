#ifndef LEOMAX_TOKENIZER_PYBIND_PYNORMALIZER_H_
#define LEOMAX_TOKENIZER_PYBIND_PYNORMALIZER_H_

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../normalizers/normalizer.h"
#include "../normalizers/bert_normalizer.h"
#include "../normalizers/unicode_normalizer.h"

namespace leomax_tokenizer {
namespace pybind {

class PyNormalizer : public normalizers::Normalizer {
public:
    virtual void operator() (normalizers::NormalizedString* mut_str) const override {
        PYBIND11_OVERLOAD_PURE_NAME(
            void, 
            normalizers::Normalizer, 
            "__call__", 
            operator(), 
            mut_str);
    }

};

class PyBertNormalizer : public normalizers::BertNormalizer {
public:
	using normalizers::BertNormalizer::BertNormalizer;
	virtual void operator()(
		normalizers::NormalizedString* mut_str) const override {
		PYBIND11_OVERLOAD_NAME(
			void, normalizers::BertNormalizer, "__call__", operator(), mut_str);
	}
};


class PyNFKDNormalizer : public normalizers::NFKDNormalizer {
public:
    using NFKDNormalizer::NFKDNormalizer;
    virtual void operator()(
        normalizers::NormalizedString* mut_str) const override {
        PYBIND11_OVERLOAD_NAME(
            void,
            NFKDNormalizer,
            "__call__",
            operator(),
            mut_str
        );
    }

};

class PyNFKCNormalizer : public normalizers::NFKCNormalizer {
public:
    using NFKCNormalizer::NFKCNormalizer;
    virtual void operator()(
        normalizers::NormalizedString* mut_str) const override {
        PYBIND11_OVERLOAD_NAME(
            void,
            NFKCNormalizer,
            "__call__",
            operator(),
            mut_str
        );
    }
};

void bind_normalizers(pybind11::module *m);

}
}

#endif 