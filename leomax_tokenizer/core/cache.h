//
// Created by xiongxinlei on 5/29/24.
//

#ifndef LEOMAX_TOKENIZER_CACHE_H
#define LEOMAX_TOKENIZER_CACHE_H

#include <unordered_map>

namespace leomax_tokenizer {
namespace core {

static size_t DEFAULT_CACHE_CAPACITY = 100000;
template <typename K, typename V>
class Cache {
public:
    Cache(size_t capacity = DEFAULT_CACHE_CAPACITY)
      :capacity_(capacity) {
        fresh();
    }

    void fresh() {
        create_cache_map(capacity_);
    }

    void create_cache_map(size_t capacity) {
        map_ = std::unordered_map<K, V>(capacity);
    }

    bool get_value(const K& key, V* value) {
        if (map_.find(key) == map_.end()) {   // 没有找到
            return false;
        }
        *value = map_.at(key);
        return true;
    }

    bool set_value(const K& key, const V& value) {
        if (map_.size() >= capacity_) {
            return false;
        }

        map_.insert(std::make_pair(key, value));
        return true;
    }
public:
  std::unordered_map<K, V> map_;
  size_t capacity_;
private:

};
}
}

#endif  // LEOMAX_TOKENIZER_CACHE_H
