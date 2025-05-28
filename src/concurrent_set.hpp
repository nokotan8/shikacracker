#include <shared_mutex>
#include <unordered_set>

// Simple wrapper around std::unordered_set
// to provide thread safety for a few methods.
template <typename T> class concurrent_set {
  public:
    // Set and mutex are public if you want
    // to do other things with the set.
    std::unordered_set<T> set;
    mutable std::shared_mutex mtx;

    int size() {
        mtx.lock_shared();
        int size = set.size();
        mtx.unlock_shared();

        return size;
    }

    bool empty() {
        mtx.lock_shared();
        int is_empty = set.empty();
        mtx.unlock_shared();

        return is_empty;
    }

    bool insert(T item) {
        mtx.lock();
        bool stat = set.insert(item).second;
        mtx.unlock();

        return stat;
    }

    int count(T item) {
        mtx.lock_shared();
        int cnt = set.count(item);
        mtx.unlock_shared();

        return cnt;
    }

    void erase(T item) {
        mtx.lock();
        set.erase(item);
        mtx.unlock();

        return;
    }
};
