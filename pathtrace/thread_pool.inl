#pragma once

#include "thread_pool.h"

namespace pathtrace {
    template <typename F, typename... Args>
    std::future<std::invoke_result_t<F, Args...>> ThreadPool::add_work(F&& function, Args&&... args)
    {
        using ReturnType = std::invoke_result_t<F, Args...>;

        auto task = std::make_shared<std::packaged_task<ReturnType()>>([=]() mutable {
            return std::invoke(std::forward<F>(function), std::forward<Args>(args)...);
        });
        std::future<ReturnType> future = task->get_future();

        {
            std::unique_lock lock(tasks_mutex_);
            if (stopping_)
                throw std::runtime_error{"trying to add work on stopping thread pool"};
            tasks_.emplace([=]() {
                (*task)();
            });
        }

        tasks_condvar_.notify_one();
        return future;
    }
}  // namespace pathtrace
