#include "ThreadPool.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <mutex>

void ThreadPool::start()
{
    // todo: pass maxNumThreads as parameter and only use hardware_concurrency as upper limit or as value if
    // none provided
    const uint32_t maxNumThreads = std::thread::hardware_concurrency();
    threads.resize(maxNumThreads);
    for(uint32_t i = 0; i < maxNumThreads; i++)
    {
        threads.at(i) = std::thread(&ThreadPool::threadLoop, this, i);
    }
    busyThreads = 0;
}

void ThreadPool::threadLoop(int i)
{
#if THREADPOOL_DEBUG_OUTPUT == 1
    {
        std::unique_lock<std::mutex> lock(printMutex);
        std::cerr << "Thread " << i << " started!" << std::endl;
    }
#endif
    while(true)
    {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            mutexCondition.wait(
                lock,
                [this]
                {
                    return !jobs.empty() || shouldTerminate;
                });
            if(shouldTerminate)
            {
#if THREADPOOL_DEBUG_OUTPUT == 1
                {
                    std::unique_lock<std::mutex> lock(printMutex);
                    std::cerr << "Thread " << i << " terminating!" << std::endl;
                }
#endif
                return;
            }
            job = jobs.front();
            jobs.pop();
        }
        busyThreads++;
        job();
        busyThreads--;
    }
    assert(false && "Should not be reached!");
}

void ThreadPool::queueJob(const std::function<void()>& job)
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        jobs.push(job);
    }
    mutexCondition.notify_one();
}

bool ThreadPool::busy()
{
    if(busyThreads > 0)
        return true;
    bool jobsLeftInQueue = false;
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        jobsLeftInQueue = !jobs.empty();
    }
    return (busyThreads > 0) || jobsLeftInQueue;
}

void ThreadPool::stop()
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        shouldTerminate = true;
    }
    mutexCondition.notify_all();
    for(std::thread& activeThread : threads)
    {
        activeThread.join();
    }
    threads.clear();
}