from typing import Dict
from .base_model import Model
from .. import core_tokenizers as C
class WordPiece(Model):
    def __init__(self,
                 vocab: Dict[str, int],
                 unk_token: str = "[UNK]",
                 max_input_chars_per_word: int=100,
                 continuing_subword_prefix: str="##",
                 handle_chinese_chars: bool=True):
        self._model = None
        if vocab is not None:
            self._model = C.models.WordPiece(vocab,
                                             unk_token,
                                             max_input_chars_per_word,
                                             continuing_subword_prefix,
                                             handle_chinese_chars)
    
    @staticmethod
    def read_file(vocab):
        return C.models.WordPiece.read_file(vocab)

    @staticmethod
    def from_file(vocab,
                  unk_token = "[UNK]",
                  max_input_chars_per_word: int=100,
                  continuing_subword_prefix: str="continuing_subword_prefix"):
        print("read the file")
        wp = WordPiece(None)
        wp._model = C.models.WordPiece.from_file(vocab,
                                                 unk_token,
                                                 max_input_chars_per_word,
                                                 continuing_subword_prefix)
        return wp
