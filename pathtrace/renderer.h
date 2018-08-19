#pragma once

#include <pathtrace/image.h>
#include <pathtrace/scene.h>
#include <pathtrace/thread_pool.h>

namespace pathtrace {
    class Renderer {
    public:
        Renderer() = default;
        Renderer(size_t nr_threads);

        /// nr_threads=0 means auto
        void resize_thread_pool(size_t nr_threads);
        Image render(const Scene& scene);

    private:
        ThreadPool thread_pool_;
    };
}  // namespace pathtrace
