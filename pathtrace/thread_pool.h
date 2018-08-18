#pragma once

#include <future>
#include <queue>
#include <type_traits>
#include <vector>

namespace pathtrace {
    class ThreadPool {
    public:
        ThreadPool() = default;
        ThreadPool(size_t nr_threads);
        ~ThreadPool();

        size_t size() const;
        void resize(size_t nr_threads);

        /// stores a copy of arguments
        template <typename F, typename... Args>
        std::future<std::invoke_result_t<F, Args...>> add_work(F&& function, Args&&... args);

    private:
        std::vector<std::thread> workers_;

        std::mutex tasks_mutex_;
        std::condition_variable tasks_condvar_;
        std::queue<std::function<void()>> tasks_;
        bool stopping_ = false;
    };
}  // namespace pathtrace

#include "thread_pool.inl"
