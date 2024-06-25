from abc import ABC
from typing import List

class Model(ABC):
    def tokenizer(self, tokens: List[str]):
        return self._model.tokenizer(tokens)