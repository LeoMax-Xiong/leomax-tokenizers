#include "metaspace.h"
#include "../utils/utf8.h"
namespace leomax_tokenizer {
namespace pretokenizers {

static re2::RE2 pattern(" ");

MetaSpacePreTokenizer::MetaSpacePreTokenizer(const std::string& replacement,
                                            bool add_prefix_space) :
                replacement_(replacement), 
                add_prefix_space_(add_prefix_space) {
    update_replacement_char();
    
}
void MetaSpacePreTokenizer::operator()(PreTokenizedString* pretokenized) const {
    std::vector<normalizers::NormalizedString> normalized_splits;
    pretokenized->split([&](int idx, 
                            normalizers::NormalizedString* normalized,
                            std::vector<StringSplit>* string_splits) {
        normalized->replace(pattern, replacement_);
        
    });
}

void MetaSpacePreTokenizer::update_replacement_char() {
    uint32_t ch;
    utils::utf8_to_uint32(replacement_.data(), &ch);
    replacement_char_ = ch;
}


}
}