#!/usr/bin/env python3
# -*- coding: utf-8 -*-
from abc import ABC
from .. import core_tokenizers as C
class Normalizer(ABC):
    pass

class BertNormalizer:
    def __init__(self,
                 clean_text: bool=True,
                 handle_chinese_chars: bool=True,
                 strip_accents: bool=True,
                 lowercase: bool=True) -> None:
        self._normalizer = C.normalizers.BertNormalizer(clean_text,
                                                       handle_chinese_chars,
                                                       strip_accents,
                                                       lowercase)
        
class NFKCNormalizer(Normalizer):
    def __init__(self):
        super().__init__()
        self._normalizer = C.normalizers.NFKCNormalizer()