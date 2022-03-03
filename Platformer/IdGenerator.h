#pragma once
#include <queue>

template<typename T>
class IdGenerator {
    static_assert(std::is_arithmetic<T>::value == true, "must be arithmetic");
public:
    T GetId() {
        if (!_deletedIds.empty()) {
            auto val = _deletedIds.front();
            _deletedIds.pop();
            return val;
        }
        else {
            return ++_onId;
        }
    }
    void DeleteId(T id) {
        _deletedIds.push(id);
    }
private:
    std::queue<T> _deletedIds;
    T _onId = 0;
};