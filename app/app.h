#pragma once

namespace pathtracer {
    class App {
    public:
        int main(int argc, const char** argv) noexcept;

    private:
        void main_impl(int argc, const char** argv);
    };
}  // namespace pathtracer
