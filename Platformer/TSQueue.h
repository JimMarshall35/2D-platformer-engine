#pragma once
#include <queue>
#include <mutex>
template <typename T>
class TSQueue
{
public:
	bool IsEmpty() {
		std::scoped_lock<std::mutex>lg(_mtx);
		return _queue.empty();
	}
	T PopFront() {
		std::scoped_lock<std::mutex>lg(_mtx);
		T val = _queue.front();
		_queue.pop();
		return val;
	}
	void Push(T val) {
		std::scoped_lock<std::mutex>lg(_mtx);
		_queue.push(val);
	}
	size_t Size() {
		std::scoped_lock<std::mutex>lg(_mtx);
		return _queue.size();
	}
private:
	std::mutex _mtx;
	std::queue<T> _queue;
};

