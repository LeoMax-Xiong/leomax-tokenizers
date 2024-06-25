//
// Created by xiongxinlei on 5/28/24.
//
#include "unordered_map"
#include "map"
#include "glog/logging.h"
#include <utility>
#include <queue>
#include <random>
#include <algorithm>
#ifndef LEOMAX_TOKENIZER_BASE_H
#define LEOMAX_TOKENIZER_BASE_H
namespace leomax_tokenizer {
namespace core {

enum OffsetType {CHAR, BYTE};

using Offset = std::pair<uint32_t, uint32_t>;
using Range = std::pair<uint32_t, uint32_t>;
using Vocab = std::unordered_map<std::string, uint32_t>;
using VocabReversed = std::unordered_map<uint32_t, std::string>;
using Merges = std::vector<std::pair<std::string, std::string>>;
using Pair = std::pair<uint32_t, uint32_t>;
using MergeMap = std::map<Pair, std::pair<uint32_t, uint32_t>>;


struct Token {
  uint32_t id_;             // token id
  std::string value_;       // token value
  Offset offset_;

  Token() = default;
  Token(uint32_t id, const std::string& value, const Offset& offset) :
      id_(id), value_(value), offset_(offset) {
  }
};

struct Merge {
    size_t pos_;
    uint32_t rank_;
    uint32_t new_id_;

    bool operator==(const Merge& other) const {
        return pos_ == other.pos_ && rank_ == other.rank_;
    }

    bool operator<(const Merge& other) const {
        if (rank_ != other.rank_) {
            return rank_ > other.rank_;
        }

        return pos_ > other.pos_;
    }
};

struct Symbol {
    uint32_t ch_;   // 符号
    int prev_;      // 指向上一个符号的序号
    int next_;      // 指向下一个符号的序号
    size_t len_;    // 该符号占据的字节长度
    Symbol() = default;
    Symbol(uint32_t ch, int prev, int next, size_t len):
        ch_(ch), prev_(prev), next_(next), len_(len) {
    }

    void merge_with(const Symbol& other, uint32_t ch) {
        ch_ = ch;
        next_ = other.next_;
        len_ += other.len_;
    }
};

struct BPEWord {
    BPEWord() = default;
    BPEWord(size_t capacity) {
    }

    void reserve(size_t capacity) {
        this->symbols_.reserve(capacity);
    }

    void add(uint32_t ch, size_t byte_len) {
        int len = this->symbols_.size();
        int next = -1;      // -1 表示没有具体的位置
        int prev = -1;
        if (len >= 1) {
            this->symbols_.back().next_ = len;      // 最后一个符号的指针指向的序号为len
            prev = len - 1;
        }

        this->symbols_.emplace_back(ch, prev, next, byte_len);
    }
    std::vector<Symbol> symbols_;

    void merge_all(const MergeMap& merges, const std::vector<float>& dropout) {
        std::priority_queue<core::Merge> queue;
        std::vector<core::Merge> skip;
        skip.reserve(this->symbols_.size());
        for (int i = 0; i < this->symbols_.size() - 1; ++i) {
            auto& first = this->symbols_[i];
            auto& second = this->symbols_[i + 1];
            if (merges.find({first.ch_, second.ch_}) != merges.end()) {
                auto new_merge_info = merges.at({first.ch_, second.ch_});
                core::Merge new_merge{static_cast<size_t>(i),
                                      new_merge_info.first,
                                      new_merge_info.second};
                queue.push(new_merge);
            }
        }

        VLOG(6) << "queue size: " << queue.size();

        std::random_device
            rd;  // Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd());  // Standard mersenne_twister_engine seeded with
                                // rd()
        std::uniform_real_distribution<float> distrib(0.0, 1.0);
        bool can_skip = (dropout.size() > 0);
        while (!queue.empty()) {
            // Can't use reference there, because the pop operation will change the
            // top value
            auto top = queue.top();
            queue.pop();
            if (can_skip && distrib(gen) < dropout[0]) {
                // May dropout some merges
                skip.push_back(top);
            } else {
                for (auto& skip_merge : skip) {
                    queue.push(skip_merge);
                }
                skip.clear();
                if (symbols_[top.pos_].len_ == 0) {
                    continue;
                }
                if (symbols_[top.pos_].next_ == -1) {
                    continue;
                }
                size_t next_pos = symbols_[top.pos_].next_;
                auto& right = symbols_[next_pos];
                // Make sure we are not processing an expired queue entry
                auto target_new_pair = Pair{symbols_[top.pos_].ch_, right.ch_};
                if (merges.find(target_new_pair) == merges.end() ||
                    merges.at(target_new_pair).second != top.new_id_) {
                    continue;
                }
                // Otherwise, let's merge
                symbols_[top.pos_].merge_with(right, top.new_id_);
                // Tag the right part as removed
                symbols_[next_pos].len_ = 0;
                // Update `prev` on the new `next` to the current pos
                if (right.next_ > -1 && (right.next_ < symbols_.size())) {
                    symbols_[right.next_].prev_ = top.pos_;
                }
                // Insert the new pair formed with the previous symbol
                auto& current = symbols_[top.pos_];
                if (current.prev_ >= 0) {
                    auto prev = current.prev_;
                    auto& prev_symbol = symbols_[prev];
                    auto new_pair = Pair{prev_symbol.ch_, current.ch_};
                    if (merges.find(new_pair) != merges.end()) {
                        auto new_merge = merges.at(new_pair);
                        queue.push({static_cast<size_t>(current.prev_),
                                    new_merge.first,
                                    new_merge.second});
                    }
                }

                // Insert the new pair formed with the next symbol
                size_t next = current.next_;
                if (next < symbols_.size()) {
                    auto& next_symbol = symbols_[next];
                    auto next_pair = Pair{current.ch_, next_symbol.ch_};
                    if (merges.find(next_pair) != merges.end()) {
                        auto new_merge = merges.at(next_pair);
                        queue.push({top.pos_, new_merge.first, new_merge.second});
                    }
                }
            }
        }
        symbols_.erase(
            std::remove_if(symbols_.begin(),
                        symbols_.end(),
                        [](const Symbol& symbol) { return symbol.len_ == 0; }),
            symbols_.end());

    }
};



}
}

#endif  // LEOMAX_TOKENIZER_BASE_H
