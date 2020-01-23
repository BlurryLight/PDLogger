#pragma once
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

template <typename T> class safe_queue {
public:
    safe_queue() { queue_ptr_ = std::unique_ptr<std::queue<T>>(new std::queue<T>); }
    ~safe_queue() {}
    safe_queue(const safe_queue &other) = delete;
    safe_queue(safe_queue &&other) = delete;
    safe_queue &operator=(const safe_queue &other) = delete;

    T pop()
    {
        std::unique_lock<std::mutex> guard(mutex_);
        while (queue_ptr_->empty()) {
            cond_.wait(guard);
        }
        T first = queue_ptr_->front(); // reference?
        queue_ptr_->pop();
        return first;
    }

  void push(const T &item) {
    {
      std::lock_guard<std::mutex> guard(mutex_);
      queue_ptr_->push(item);
    }
    cond_.notify_one();
  }

  void push(T &&item) {
    {
      std::lock_guard<std::mutex> guard(mutex_);
      queue_ptr_->push(std::move(item));
    }
    cond_.notify_one();
  }

  bool empty() {
    std::lock_guard<std::mutex> guard(mutex_);
    return queue_ptr_->empty();
  }

  //  bool empty() const { return queue_ptr_->empty(); }

  private:
  std::unique_ptr<std::queue<T, std::deque<T>>> queue_ptr_; // deque
  std::mutex mutex_;
  std::condition_variable cond_;
};
