#include <thread_pool.h>

ThreadPool::ThreadPool(size_t threads) : stop(false), active_tasks(0) {
    for (size_t i = 0; i < threads; ++i) {
        workers.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(queue_mutex);
                    cv.wait(lock, [this] { return stop || !tasks.empty(); });

                    if (stop && tasks.empty()) {
                        return;
                    }

                    task = std::move(tasks.front());
                    tasks.pop();
                }

                ++active_tasks;
                try {
                    task();
                } catch (...) {
                    std::unique_lock<std::mutex> lock(queue_mutex);
                    exceptions.push_back(std::current_exception());
                }
                --active_tasks;

                cv.notify_all();
            }
        });
    }
}

void ThreadPool::join() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    cv.notify_all();

    std::unique_lock<std::mutex> lock(queue_mutex);
    cv.wait(lock, [this] { return active_tasks == 0 && tasks.empty(); });

    for (std::thread& worker : workers) {
        worker.join();
    }

    if (!exceptions.empty()) {
        throw std::runtime_error("Exceptions occurred in threads. Check the logs or debug further.");
    }
}

ThreadPool::~ThreadPool() {
    if (!stop) {
        join();
    }
}