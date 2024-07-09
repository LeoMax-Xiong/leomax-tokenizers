import sys
sys.path.append('/Users/admin/xiongxinlei/task/leomax-tokenizers/python')
import leomax_tokenizer

if __name__ == '__main__':
    print("Hello World!!!")
    model_dir = '/Users/admin/xiongxinlei/model/qwen1.5-0.5b'
    tokenizer = Qwen2Tokenizer.from_pretrained("model_dir")