#!/usr/bin/env python3
# -*- coding: utf-8 -*-
from abc import ABC
from typing import Any
from .. import core_tokenizers as C

class PreTokenizer(ABC):
    def __call__(self, pretokenizerd):
        pass
    
class BertPreTokenizer(PreTokenizer):
    def __init__(self):
        print("start to construct bert pre tokenizer\n\n")
        self._pretokenizer = C.pretokenizers.BertPreTokenizer()
        