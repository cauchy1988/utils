#ifndef TC_UTILS_PARALLEL_H
#define TC_UTILS_PARALLEL_H

#include <mutex>
#include <condition_variable>
#include <atomic>
#include <memory>

namespace tc_utils {
    class ParallelTracker {
        public:
            ParallelTracker(int parallelNum, uint64_t timeout) : _parallelNum(parallelNum), _finishedNum(0), _timeout(timeout), _bTimeout(false) {}
            void finishOne() {
                std::unique_lock<std::mutex> lock(_mutex);
                ++finishedCount;

                if (finishedCount >= _parallelNum) {
                    _cond.notify_one();
                }
            }

            void wait() {
                std::unique_lock<std::mutex> lock(_mutex);
                std::chrono::duration<double, std::milli> duration(_timeout);
                bool waitRet = _cond.wait_for(lock, duration, [this]{this->_finishedNum >= _parallelNum;});

                if (!waitRet) {
                    _bTimeout = true;
                    _cond.wait(lock, [this] {this->_finishedNum >= _parallelNum;});
                }
            }

            bool timeout() {
                return _bTimeout;
            }

        public:
            int _parallelNum;
            int _finishedNum;
            volatile uint64_t _timeout;

            std::mutex _mutex;
            std::condition_variable _cond;

            bool _bTimeout;

            const void *_request;
            void *_response;
    };


    class ParallelTask {
        public:
            static std::atomic<uint64_t> taskSeq;

            ParallelTask(std::shared_ptr<ParallelTracker> tracker) : _tracker(tracker) {}

            std::shared_ptr<ParallelTracker> _tracker;

            uint64_t _id;

            void generateId() {
                _id = taskSeq.fetch_and_add(1, std::memory_order_relaxed);
            }
    };
}

#endif
