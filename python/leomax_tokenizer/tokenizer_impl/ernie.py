#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright (c) 2024 PaddlePaddle Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
from ..models import WordPiece, FastWordPiece
from ..c_wrap import Tokenizer
from .base_tokenizer import BaseFastTokenizer
from ..normalizers import BertNormalizer
from ..pretokenizers import BertPreTokenizer

class ErnieFastTokenizer(BaseFastTokenizer):
    def __init__(self,
                 vocab=None,
                 unk_token="[UNK]",
                 sep_token="[SEP]",
                 cls_token="[CLS]",
                 pad_token="[PAD]",
                 mask_token="[MASK]",
                 clean_text=True,
                 handle_chinese_chars=True,
                 strip_accents=True,
                 lowercase=True,
                 wordpieces_prefix="##",
                 max_sequence_len=None,
                 max_input_chars_per_word=100,
                 use_fast_wordpiece=False,
                 use_fast_wordpiece_with_pretokenization=False):
        print("start to construct the ErnieFastTokenizer")
        tokenizer_model = FastWordPiece if use_fast_wordpiece else WordPiece
        model_kwargs = {
            "unk_token": str(unk_token),
            "continuing_subword_prefix": wordpieces_prefix,
            "max_input_chars_per_word": max_input_chars_per_word
        }
        
        # 配置 fast wordpiece 的参数
        if use_fast_wordpiece:
            model_kwargs["with_pretokenization"] = use_fast_wordpiece_with_pretokenization
        else:
            model_kwargs["handle_chinese_chars"] = handle_chinese_chars

        # 如果 vocab 不为空，则使用词表进行初始化，实例化一个 tokenizer 对象
    
        if vocab is not None: 
            print("vocab is not None")
            tokenizer = Tokenizer(tokenizer_model(vocab=vocab, **model_kwargs))
        else:
            print("vocab is None")
            tokenizer = Tokenizer(tokenizer_model(**model_kwargs)) 
        
        # 实例化 tokenizer 的时候，词典并没有转换到 tokenizer 内部，需要手动转换
        if tokenizer.token_to_id(str(unk_token)) is not None:
            tokenizer.add_special_tokens([str(unk_token)])

        if tokenizer.token_to_id(str(sep_token)) is not None:
            tokenizer.add_special_tokens([str(sep_token)])
        
        if tokenizer.token_to_id(str(cls_token)) is not None:
            tokenizer.add_special_tokens([str(cls_token)])
            
        if tokenizer.token_to_id(str(pad_token)) is not None:
            tokenizer.add_special_tokens([str(pad_token)])
            
        if tokenizer.token_to_id(str(mask_token)) is not None:
            tokenizer.add_special_tokens([str(mask_token)])
            
        tokenizer.normalizer = BertNormalizer(
                clean_text=clean_text,
                handle_chinese_chars=handle_chinese_chars,
                strip_accents=strip_accents,
                lowercase=lowercase,
            )
        print(f"use fast word piece :{use_fast_wordpiece}")
        print(f"use fast word piece pretokernization :{use_fast_wordpiece_with_pretokenization}")
        if not use_fast_wordpiece or not use_fast_wordpiece_with_pretokenization:
            tokenizer.pretokenizer = BertPreTokenizer()
            
        
        super().__init__(tokenizer, model_kwargs)
        