from .base_tokenizer import BaseFastTokenizer
from ..c_wrap import Tokenizer
from ..models import BPE
from ..normalizers import NFKCNormalizer
from ..pretokenizers import MetaSpacePreTokenizer

__all__ = ["SentencePieceBPELeoMaxTokenizer"]

class SentencePieceBPELeoMaxTokenizer(BaseFastTokenizer):
    def __init__(self,
                 vocab=None,
                 merges=None,
                 unk_token="<unk>",
                 replacement="▁",
                 add_prefix_space=True,
                 dropout=None,
                 fuse_unk=False) -> None:
        if vocab is not None and merges is not None:
            model = BPE(vocab=vocab, merges=merges,
                        dropout=dropout, unk_token=unk_token, fuse_unk=fuse_unk)
            tokenizer = Tokenizer(model)
        else:
            tokenizer = Tokenizer(BPE())

        tokenizer.normalizer = NFKCNormalizer()
        tokenizer.pretokenizer = MetaSpacePreTokenizer()
        parameters = {
            "model": "SentencePieceBPE",
            "unk_token": unk_token,
            "replacement": replacement,
            "add_prefix_space": add_prefix_space,
            "dropout": dropout,
        }
        super().__init__(tokenizer, param_dict={})
        
            
    @staticmethod
    def from_file(vocab_filename, merges_filename, **kwargs):
        print(f"vocab filename: {vocab_filename}")
        print(f"merges filename: {merges_filename}")
        vocab, merges = BPE.read_file(vocab_filename, merges_filename)
        return SentencePieceBPELeoMaxTokenizer(vocab=vocab, merges=merges, **kwargs)


