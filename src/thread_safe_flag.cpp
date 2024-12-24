#include <thread_safe_flag.h>

ThreadSafeFlag::ThreadSafeFlag() : flag(false) {}

void ThreadSafeFlag::set(bool value) {
    std::unique_lock lock(mtx);
    flag = value;
}

bool ThreadSafeFlag::get() const {
    std::shared_lock lock(mtx);
    return flag;
}