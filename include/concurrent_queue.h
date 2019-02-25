#ifndef TC_UTILS_CONCURRENT_QUEUE_H
#define TC_UTILS_CONCURRENT_QUEUE_H

#include <queue>
#include <condition_variable>
#include <mutex>
#include <chrono>

namespace tc_utils {
    template<typename T>
    class ConcurrentQueue {
        public:
            using size_type = typename std::queue<T>::size_type;
            using value_type = typename std::queue<T>::value_type;
            constexpr static size_type default_size = 2048;

        public:
            ConcurrentQueue() {_max_size = default_size;}
            ConcurrentQueue(size_type queue_size) {_max_size = queue_size;}
            ~ConcurrentQueue() = default;
            ConcurrentQueue(const ConcurrentQueue &queue) = delete;
            ConcurrentQueue &operator=(const ConcurrentQueue &queue) = delete;

        public:
            bool push(const value_type &value, double time_out);
            bool pop(value_type &value, double time_out);
            size_type size();

        private:
            std::queue<T> _queue;
            std::mutex _mutex;
            std::condition_variable _cond;
            size_type _max_size;
    };

    template<typename T>
    bool ConcurrentQueue<T>::push(const typename ConcurrentQueue<T>::value_type &value, double time_out) {
        std::unique_lock<std::mutex> localLock(_mutex);
        std::chrono::duration<double, std::milli> duration(time_out);
        bool waitRet = _cond.wait_for(localLock, duration, [this]{return this->_queue.size() < _max_size;});

        if (!waitRet) {
            return waitRet;
        }

        _queue.push(value);
        _cond.notify_one();

        return true;
    }

    template<typename T>
    bool ConcurrentQueue<T>::pop(typename ConcurrentQueue<T>::value_type &value, double time_out) {
        std::unique_lock<std::mutex> localLock(_mutex);
        std::chrono::duration<double, std::milli> duration(time_out);
        bool waitRet = _cond.wait_for(localLock, duration, [this]{return !this->_queue.empty();});

        if (!waitRet) {
            return waitRet;
        }

        value = _queue.front();
        _queue.pop();

        _cond.notify_one();

        return true;
    }

    template<typename T>
    typename ConcurrentQueue<T>::size_type ConcurrentQueue<T>::size() {
        std::unique_lock<std::mutex> localLock(_mutex);
        return _queue.size();
    }
}

#endif
