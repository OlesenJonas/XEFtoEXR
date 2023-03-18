#pragma once

/*
    based on:
        https://stackoverflow.com/a/32593825
*/

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <vector>

// todo: dont do like this, add actual logger and log to debug output
#define THREADPOOL_DEBUG_OUTPUT 0 // 0 or 1

class ThreadPool
{
  public:
    void start();
    void queueJob(const std::function<void()>& job);
    void stop();
    bool busy();

  private:
    void threadLoop(int i);

    bool shouldTerminate = false;           // If set threads will stop looking for job
    std::mutex queueMutex;                  // Prevent race condition when accessing job queue
    std::condition_variable mutexCondition; // Allows threads to wait on new jobs or termination
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> jobs;
    std::atomic<uint8_t> busyThreads;
#if THREADPOOL_DEBUG_OUTPUT == 1
    std::mutex printMutex;
#endif
};