#ifndef UPLOAD_H
#define UPLOAD_H

#include <thread_safe_flag.h>
#include <thread_safe_map.h>
#include <future>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <future>
#include <chrono>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <atomic>
#include <stdexcept>
#include <coroutine>

struct task {
    struct promise_type {
        task get_return_object() {
            return task{this};
        }

        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void unhandled_exception() { std::exit(1); }
        void return_void() {}
    };

    using handle_type = std::coroutine_handle<promise_type>;

    task(promise_type* p) : handle(handle_type::from_promise(*p)) {}

    ~task() {
        if (handle) handle.destroy();
    }

    handle_type handle;

    bool await_ready() const noexcept { return false; }
    void await_suspend(std::coroutine_handle<>) const noexcept {
        handle.promise().final_suspend();
    }
    void await_resume() const noexcept {}
};

struct download_progress {
    std::atomic<bool> is_download_complete{false};
};

struct future_awaiter {
    std::future<void>& fut;

    future_awaiter(std::future<void>& f);
    bool await_ready();
    void await_suspend(std::coroutine_handle<> h);
    void await_resume();
};

void show_loader(download_progress& progress);

std::future<void> send_file_async(download_progress& progress);

task send_file_with_await();

#endif