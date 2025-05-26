#include <condition_variable>
#include <mutex>
#include <optional>
#include <vector>

template <typename T> class EntryBuffer {
  public:
    EntryBuffer(int n) : buf_size(n), buffer(new T[n]) {}
    ~EntryBuffer() = default;

    // Maximum number of items that the buffer can hold
    int max_size() { return buf_size; }

    bool add_item(T item) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return full() == false || done; });

        if (done)
            return false;

        buffer[write_i] = item;
        write_i = (write_i + 1) % buf_size;
        lock.unlock();
        cv.notify_one();

        return true;
    }

    std::optional<T> remove_item(std::vector<int> &res) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return empty() == false || done; });

        if (empty() && done) {
            lock.unlock();
            cv.notify_one();
            return std::nullopt;
        }

        T item = buffer[read_i];
        res[item]++;
        read_i = (read_i + 1) % buf_size;

        lock.unlock();
        cv.notify_one();

        return item;
    }

    // Adding/producing items is complete. After calling this,
    // further calls to EntryBuffer::add_item() will fail.
    void add_done() {
        std::unique_lock<std::mutex> lock(mtx);
        done = true;
        lock.unlock();
    }

  private:
    int buf_size;
    int write_i = 0;
    int read_i = 0;
    bool done = false;

    std::unique_ptr<T[]> buffer;
    std::mutex mtx;
    std::condition_variable cv;

    // Checks whether the buffer is full
    bool full() { return (write_i + 1) % buf_size == read_i; }

    // Checks whether the buffer is empty
    bool empty() { return write_i == read_i; }
};
