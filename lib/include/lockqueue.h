#pragma once

#include <queue>
#include <thread>
#include <mutex>              // pthread_mutex_t
#include <condition_variable> // pthread_condition_t

namespace JayRPC
{
    // 异步日志队列
    template <typename T>
    class LockQueue
    {
    public:
        void Push(const T &data);
        T Pop();

    private:
        std::queue<T> __queue;
        std::mutex __mutex;
        std::condition_variable __condVariable;
    };

    template <typename T>
    void LockQueue<T>::Push(const T &data)
    {
        std::lock_guard<std::mutex> lock(__mutex);
        __queue.push(data);
        __condVariable.notify_one();
    }

    template <typename T>
    T LockQueue<T>::Pop()
    {
        std::unique_lock<std::mutex> locker(__mutex);
        while (__queue.empty())
        {
            __condVariable.wait(locker);
        }

        T data = __queue.front();
        __queue.pop();
        locker.unlock();
        return data;
    }

} // namespace JayRPC