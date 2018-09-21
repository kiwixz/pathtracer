#pragma once

namespace pathtracer {
    class App {
    public:
        int main(int argc, char** argv) noexcept;

    private:
        void main_impl(int argc, char** argv);
    };
}  // namespace pathtracer
