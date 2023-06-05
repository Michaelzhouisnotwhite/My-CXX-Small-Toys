#include "toy/pch.hpp"
#include "toy/include.h"

toy::ThreadPool::~ThreadPool() {
    std::unique_lock<std::mutex> lock(queue_mtx_);
    thread_stop_ = true;
    lock.unlock();
    condition_.notify_all();
    for (auto &worker: workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}