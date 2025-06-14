/**
 * Concurrent hash map implementation with fine-grained locking,
 * building upon hash_map.hpp.
 */

#ifndef INC_CONCURRENT_HASH_MAP_H
#define INC_CONCURRENT_HASH_MAP_H

#include "hash_map.hpp"

#include <cstddef>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <stdint.h>
#include <vector>

template <typename T> class concurrent_hash_map : public hash_map<T> {
  public:
    // Constructor is not thread safe. Don't change
    // things before the constructor completes.
    concurrent_hash_map(size_t starting_buckets = 32)
        : hash_map<T>(starting_buckets) {
        for (size_t i = 0; i < starting_buckets; i++) {
            mutexes.push_back(std::make_unique<std::shared_mutex>());
        }
    }

    void insert(const char *key, T value, uint32_t bucket_idx) {
        double load_factor;
        {
            bucket_idx %= hash_map<T>::num_buckets();
            std::shared_lock map_lock(map_mutex);
            std::unique_lock lock(*mutexes[bucket_idx]);

            hash_map<T>::insert_no_rehash(key, value, bucket_idx);

            load_factor =
                (1.0 * hash_map<T>::size()) / hash_map<T>::num_buckets();
        }

        if (load_factor > hash_map<T>::MAX_LOAD_FACTOR) {
            rehash();
        }
    }

    void insert(const char *key, T value) {
        uint32_t idx =
            hash_map<T>::get_key_hash(key) % hash_map<T>::num_buckets();

        insert(key, value, idx);
    }

    bool exists(const char *key, uint32_t bucket_idx) const {
        bucket_idx %= hash_map<T>::num_buckets();
        std::shared_lock map_lock(map_mutex);
        std::shared_lock lock(*mutexes[bucket_idx]);

        hash_map<T>::exists(key, bucket_idx);
    }

    bool exists(const char *key) const {
        uint32_t idx = hash_map<T>::get_key_hash(key);

        return exists(key, idx);
    }

    T get(const char *key, uint32_t bucket_idx, T default_) const {
        bucket_idx %= hash_map<T>::num_buckets();
        std::shared_lock map_lock(map_mutex);
        std::shared_lock lock(*mutexes[bucket_idx]);

        return hash_map<T>::get(key, bucket_idx, default_);
    }

    T get(const char *key, T default_) const {
        uint32_t idx = hash_map<T>::get_key_hash(key);

        return get(key, idx, default_);
    }

    bool erase(const char *key, uint32_t bucket_idx) {
        bucket_idx %= hash_map<T>::num_buckets();
        std::shared_lock map_lock(map_mutex);
        std::unique_lock lock(*mutexes[bucket_idx]);

        return hash_map<T>::erase(key, bucket_idx);
    }

    bool erase(const char *key) {
        uint32_t idx = hash_map<T>::get_key_hash(key);
        return erase(key, idx);
    }

    size_t size() const {
        std::unique_lock lock(map_mutex);

        return hash_map<T>::size();
    }

    bool empty() const {
        std::unique_lock lock(map_mutex);

        return hash_map<T>::empty();
    }

  private:
    mutable std::shared_mutex map_mutex; // Locks the entire map
    mutable std::vector<std::unique_ptr<std::shared_mutex>>
        mutexes; // Locks a particular bucket

    void rehash() {
        std::unique_lock lock(map_mutex);
        size_t old_buckets = hash_map<T>::num_buckets();

        hash_map<T>::rehash();

        size_t new_buckets = hash_map<T>::num_buckets();
        if (new_buckets > old_buckets) {
            for (size_t i = 0; i < new_buckets - old_buckets; i++) {
                mutexes.push_back(std::make_unique<std::shared_mutex>());
            }
        }
    }
};

#endif // INC_CONCURRENT_HASH_MAP_H
