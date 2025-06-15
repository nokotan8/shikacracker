/**
 * Hash map implementation using MurmurHash3
 * as the hash function. Allows external computation
 * of the key hash for better performance, especially
 * with long strings.
 *
 * Only supports char arrays (char *) as the key.
 */

#ifndef INC_HASH_MAP_H
#define INC_HASH_MAP_H

#include "hashes/murmurhash3/murmurhash3.h"

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <new>
#include <stdint.h>
#include <vector>

// It doesn't really matter what the seed is
#define MURMURHASH_SEED 12345

template <typename T> class hash_map {
  public:
    hash_map(size_t starting_buckets = 1024) {
        buckets = std::vector<node *>(starting_buckets, nullptr);
        bucket_status_ = std::vector<short>(starting_buckets, false);
    }

    ~hash_map() {
        for (node *curr : buckets) {
            while (curr != nullptr) {
                free(curr->key);

                node *tmp = curr;
                curr = curr->next;
                delete tmp;
            }
        }
    }

    void insert(const char *key, T value, uint32_t bucket_idx) {
        bucket_idx %= buckets.size();

        node *curr = buckets[bucket_idx];
        while (curr != nullptr) {
            if (strcmp(key, curr->key) == 0) {
                curr->value = value;
                return;
            }
            curr = curr->next;
        }

        node *new_node = new node(key, value);
        new_node->next = buckets[bucket_idx];

        buckets[bucket_idx] = new_node;
        bucket_status_[bucket_idx] = true;
        num_entries++;

        double load_factor = (1.0 * num_entries) / buckets.size();
        if (load_factor > MAX_LOAD_FACTOR) {
            rehash();
        }
    }

    void insert(const char *key, T value) {
        uint32_t bucket_idx = get_key_hash(key);

        insert(key, value, bucket_idx);
    }

    bool exists(const char *key, uint32_t bucket_idx) const {
        bucket_idx %= buckets.size();

        node *curr = buckets[bucket_idx];
        while (curr != nullptr) {
            if (strcmp(key, curr->key) == 0) {
                return true;
            }
            curr = curr->next;
        }

        return false;
    }

    bool exists(const char *key) const {
        uint32_t idx = get_key_hash(key);

        return exists(key, idx);
    }

    /**
     * Get the value associated with a given key.
     * If it does not exist, return def (default)
     */
    T get(const char *key, uint32_t bucket_idx, T default_) const {
        bucket_idx %= buckets.size();

        node *curr = buckets[bucket_idx];
        while (curr != nullptr) {
            if (strcmp(key, curr->key) == 0) {
                return curr->value;
            }
            curr = curr->next;
        }

        return default_;
    }

    T get(const char *key, T default_) const {
        uint32_t idx = get_key_hash(key);

        return get(key, idx, default_);
    }

    bool erase(const char *key, uint32_t bucket_idx) {
        bucket_idx %= buckets.size();

        node *curr = buckets[bucket_idx];
        node *prev = curr;
        while (curr != nullptr) {
            if (strcmp(key, curr->key) == 0) {
                if (curr == buckets[bucket_idx]) {
                    buckets[bucket_idx] = curr->next;
                } else {
                    prev->next = curr->next;
                }
                delete curr;
                num_entries--;

                if (buckets[bucket_idx] == nullptr) {
                    bucket_status_[bucket_idx] = false;
                }

                return true;
            }

            prev = curr;
            curr = curr->next;
        }

        return false;
    }

    bool erase(const char *key) {
        uint32_t idx = get_key_hash(key);
        return erase(key, idx);
    }

    size_t size() const {
        return num_entries;
    }

    bool empty() const {
        return num_entries == 0;
    }

    size_t num_buckets() const {
        return buckets.size();
    }

    /**
     * Returns a vector<short> of the same length as buckets,
     * representing whether each bucket is empty or not.
     */
    const std::vector<short> bucket_status() const {
        return bucket_status_;
    }

    /**
     * Compute the hash of a string, used to index into buckets.
     * Does not mod the result with the length of buckets.
     */
    static uint32_t get_key_hash(const char *key) {
        uint32_t idx;
        murmurhash3_32(key, strlen(key), MURMURHASH_SEED, &idx);

        return idx;
    }

  protected:
    void rehash() {
        std::vector<node *> temp_buckets = buckets;
        buckets.resize(2 * temp_buckets.size());
        bucket_status_.resize(2 * temp_buckets.size());

        for (size_t i = 0; i < buckets.size(); i++) {
            buckets[i] = nullptr;
            bucket_status_[i] = false;
        }
        num_entries = 0;

        for (node *curr : temp_buckets) {
            while (curr != nullptr) {
                insert(curr->key, curr->value);

                node *tmp = curr;
                curr = curr->next;
                delete tmp;
            }
        }
    }

    void insert_no_rehash(const char *key, T value, uint32_t bucket_idx) {
        bucket_idx %= buckets.size();

        node *curr = buckets[bucket_idx];
        while (curr != nullptr) {
            if (strcmp(key, curr->key) == 0) {
                curr->value = value;
                return;
            }
            curr = curr->next;
        }

        node *new_node = new node(key, value);
        new_node->next = buckets[bucket_idx];

        buckets[bucket_idx] = new_node;
        bucket_status_[bucket_idx] = true;
        num_entries++;
    }

    double MAX_LOAD_FACTOR = 0.75;

  private:
    class node {
      public:
        char *key;
        T value;
        node *next;

        node(const char *k, T v) : value(v), next(nullptr) {
            size_t len = strlen(k);
            key = (char *)malloc(sizeof(char) * (len + 1));
            if (key == nullptr) {
                throw std::bad_alloc();
            }

            strncpy(key, k, len + 1);
        }
    };

    std::vector<node *> buckets;
    std::vector<short> bucket_status_;
    int num_entries;
};

#endif // INC_HASH_MAP_H
