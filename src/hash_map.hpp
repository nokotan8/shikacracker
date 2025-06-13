/**
 * Hash map implementation using MurmurHash3
 * as the hash function. Allows external computation
 * of the key hash when looking up and deleting elements
 * for better performance, especially with long strings.
 *
 * Only supports char arrays (char *) as the key.
 */

#ifndef INC_HASH_MAP_H
#define INC_HASH_MAP_H

#include "hashes/murmurhash3/murmurhash3.h"

#include <cstddef>
#include <cstring>
#include <stdint.h>
#include <vector>

// It doesn't really matter what the seed is
#define MURMURHASH_SEED 12345

template <typename T> class hash_map {
  public:
    hash_map(size_t starting_buckets = 32) {
        buckets = std::vector(starting_buckets, nullptr);
    }

    ~hash_map() {
        for (node *curr : buckets) {
            while (curr != nullptr) {
                node *tmp = curr;
                curr = curr->next;
                delete tmp;
            }
        }
    }

    void insert(char *key, T value) {
        uint32_t idx = get_key_hash(key) % buckets.size();

        node *curr = buckets[idx];
        while (curr != nullptr) {
            if (strcmp(key, curr->key) == 0) {
                curr->value = value;
                return;
            }
            curr = curr->next;
        }

        node *new_node = new node(key, value);
        new_node->next = buckets[idx];
        buckets[idx] = new_node;

        size++;
        double load_factor = (1.0 * size) / buckets.size();

        if (load_factor > MAX_LOAD_FACTOR) {
            rehash();
        }
    }

    bool exists(char *key, uint32_t bucket_idx) {
        bucket_idx %= buckets.size();

        node *curr = buckets[bucket_idx];
        while (curr != nullptr) {
            if (strcmp(key, curr->key) == 0) {
                return true;
            }
        }

        return false;
    }

    bool exists(char *key) {
        uint32_t idx = get_key_hash(key);

        return exists(key, idx);
    }

    /**
     * Get the value associated with a given key.
     * If it does not exist, return def (default)
     */
    T get(char *key, uint32_t bucket_idx, T def) {
        bucket_idx %= buckets.size();

        node *curr = buckets[bucket_idx];
        while (curr != nullptr) {
            if (strcmp(key, curr->key) == 0) {
                return curr->value;
            }
        }

        return def;
    }

    T get(char *key, T def) {
        uint32_t idx = get_key_hash(key);

        return get(key, idx, def);
    }

    bool erase(char *key, uint32_t bucket_idx) {
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
                size--;
                return true;
            }
        }

        return false;
    }

    bool erase(char *key) {
        uint32_t idx = get_key_hash(key);
        return erase(key, idx);
    }

    /**
     * Compute the hash of a string, used to index into buckets.
     * Does not mod the result with the length of buckets.
     */
    uint32_t get_key_hash(char *key) {
        uint32_t idx;
        murmurhash3_32(key, strlen(key), MURMURHASH_SEED, &idx);

        return idx;
    }

  private:
    class node {
      public:
        char *key;
        T value;
        node *next;

        node(char *k, T v) : key(k), value(v), next(nullptr) {}
    };

    std::vector<node *> buckets;
    int size;
    double MAX_LOAD_FACTOR = 0.75;

    void rehash() {
        std::vector<node *> temp_buckets = buckets;
        buckets.resize(2 * temp_buckets.size());

        for (size_t i = 0; i < buckets.size(); i++) {
            buckets[i] = nullptr;
        }
        size = 0;

        for (node *curr : temp_buckets) {
            while (curr != nullptr) {
                insert(curr->key, curr->value);

                node *tmp = curr;
                curr = curr->next;
                delete tmp;
            }
        }
    }
};

#endif // INC_HASH_MAP_H
