#include <thread_safe_map.h>

ThreadSafeMap::ThreadSafeMap() {}

bool ThreadSafeMap::insert_if_not_contains(const std::string& key, const std::string& value) {
    std::unique_lock lock(mtx);
    if (map.find(key) == map.end()) {
        map[key].push_back(value);
        return true;
    }
    return false;
}