#pragma once
#include <thread>
#define THREAD_SLEEP_MS(ms) std::this_thread::sleep_for(std::chrono::duration<double,std::milli>(ms));
