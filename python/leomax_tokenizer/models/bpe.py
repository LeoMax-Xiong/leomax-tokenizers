#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from .base_model import Model
from .. import core_tokenizers as C

class BPE(Model):
    def __init__(self,
               vocab=None,
               merges=None,
               cache_capacity: int=None,
               dropout=None,
               unk_token=None,
               continuing_subword_prefix=None,
               end_of_word_suffix=None,
               fuse_unk=None):
        self._model = C.models.BPE(vocab, merges, 
                    cache_capacity, dropout, 
                    unk_token, continuing_subword_prefix, 
                    end_of_word_suffix, fuse_unk)
    
    @staticmethod
    def read_file(vocab_path: str, merges_path: str):
        return C.models.BPE.read_file(vocab_path, merges_path)