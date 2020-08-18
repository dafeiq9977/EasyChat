#pragma once
#include "CommenHeaders.h"
class ScopedThread {
private:
	std::thread t;
public:
	explicit ScopedThread(std::thread somethread):t(std::move(somethread)) {
		if (!t.joinable())
			throw std::logic_error("empty thread");
	}
	~ScopedThread() {
		t.join();
	}
	ScopedThread(ScopedThread const&) = delete;
	ScopedThread & operator=(ScopedThread const&) = delete;
};