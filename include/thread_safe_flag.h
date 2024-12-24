#ifndef THREAD_SAFE_FLAG_H
#define THREAD_SAFE_FLAG_H

#include <shared_mutex>
#include <mutex>

class ThreadSafeFlag {
    mutable std::shared_mutex mtx;
    bool flag;

public:
    ThreadSafeFlag();
    void set(bool value);
    bool get() const;
};

#endif