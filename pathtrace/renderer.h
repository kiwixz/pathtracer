#pragma once

#include "pathtrace/fast_rand.h"
#include "pathtrace/image.h"
#include "pathtrace/scene.h"

namespace pathtracer {
    class Renderer {
    public:
        Image render(const Scene& scene);

    protected:
        Color radiance(const Ray& ray, const Scene& scene, int depth = 0);

    private:
        FastRand gen_;
    };
}  // namespace pathtracer
