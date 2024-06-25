__ALL__ = ['BaseFastTokenizer']

class BaseFastTokenizer:
    def __init__(self, tokenizer_impl, param_dict):
        self._tokenizer = tokenizer_impl
        self._param_dict = param_dict
        
    def encode(self, 
               text,
               pair=None):
        if text is None:
            raise ValueError('encode: `text` cannot be None.')

        return self._tokenizer.encode(text, pair)
    
    @property
    def normalizer(self):
        return self._tokenizer.normalizer
    
    @normalizer.setter
    def normalizer(self, normalizer):
        # 调用 Tokenizer().normalizer = some_normalizer 时，会调用这个 setter
        self._tokenizer.normalizer = normalizer