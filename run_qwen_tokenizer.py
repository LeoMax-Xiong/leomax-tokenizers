#!/usr/bin/env python3
# -* coding:utf-8 -*-
from leo_tokenizers.qwen.tokenization_qwen2 import Qwen2Tokenizer

if __name__ == '__main__':
    print("Hello World!!!")
    model_dir = '/Users/admin/xiongxinlei/model/qwen1.5-0.5b'
    prompt = "我爱中国"
    tokenizer = Qwen2Tokenizer.from_pretrained(model_dir)
    output = tokenizer(prompt, return_tensors="pt")
    print(output)