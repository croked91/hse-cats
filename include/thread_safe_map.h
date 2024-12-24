#ifndef THREAD_SAFE_MAP_H
#define THREAD_SAFE_MAP_H

#include <unordered_map>
#include <vector>
#include <string>
#include <shared_mutex>
#include <mutex>
#include <optional>
#include <algorithm>

class ThreadSafeMap {
    mutable std::shared_mutex mtx;
    std::unordered_map<std::string, std::vector<std::string>> map;

public:
    ThreadSafeMap();
    bool insert_if_not_contains(const std::string& key, const std::string& value);
};

#endif