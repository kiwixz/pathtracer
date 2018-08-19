#include "thread_pool.h"

namespace pathtrace {
    ThreadPool::ThreadPool(size_t nr_threads)
    {
        resize(nr_threads);
    }

    ThreadPool::~ThreadPool()
    {
        {
            std::unique_lock lock(tasks_mutex_);
            stopping_ = true;
        }

        tasks_condvar_.notify_all();
        for (std::thread& worker : workers_)
            worker.join();
    }

    size_t ThreadPool::size() const
    {
        return workers_.size();
    }
    void ThreadPool::resize(size_t nr_threads)
    {
        if (nr_threads < workers_.size())
            workers_.resize(nr_threads);
        workers_.reserve(nr_threads);

        for (size_t i = nr_threads - workers_.size(); i > 0; --i)
            workers_.emplace_back([this] {
                while (true) {
                    std::unique_lock lock{tasks_mutex_};
                    tasks_condvar_.wait(lock, [this] {
                        return stopping_ || !tasks_.empty();
                    });
                    if (stopping_ && tasks_.empty())
                        return;

                    std::function<void()> task = std::move(tasks_.front());
                    tasks_.pop();
                    lock.unlock();

                    task();
                }
            });
    }
}  // namespace pathtrace
