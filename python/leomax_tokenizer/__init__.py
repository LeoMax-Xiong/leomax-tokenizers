# Copyright (c) 2022 PaddlePaddle Authors. All Rights Reserved.
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
__version__ = "1.0.0"
import os
from .tokenizer_impl import (
    SentencePieceBPELeoMaxTokenizer
)

try:
    # 获取当前文件夹路径
    current_path = os.path.abspath(os.path.dirname(__file__))

except:
    pass

# 引入c++的库core_tokenizers
from . import core_tokenizers as C
from .c_wrap import *
from .tokenizer_impl import ErnieFastTokenizer
from .models import WordPiece