
from .bert_pretokenizer import PreTokenizer
from .bert_pretokenizer import BertPreTokenizer
from .. import core_tokenizers as C

class MetaSpacePreTokenizer(PreTokenizer):
    def __init__(self,
                 replacement: str="_",
                 add_prefix_space: bool=True):
        self._pretokenizer = C.pretokenizers.MetaSpacePreTokenizer(
                                    replacement, add_prefix_space)