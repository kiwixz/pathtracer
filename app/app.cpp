#include "app/app.h"
#include <cxxopts.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace pathtracer {
    int App::main(int argc, const char** argv) noexcept
    {
        std::shared_ptr<spdlog::logger> logger = spdlog::stderr_color_mt("stderr");
#ifdef _DEBUG
        spdlog::set_level(spdlog::level::debug);
#endif
        try {
            spdlog::set_pattern("%^[%H:%M:%S.%f][%t][%l] %v%$");
            main_impl(argc, argv);
            spdlog::drop_all();
        }
        catch (const std::exception& e) {
            logger->error("exception: {}", e.what());
            return 1;
        }
        return 0;
    }

    void App::main_impl(int argc, const char** argv)
    {
        cxxopts::Options options{"pathtracer"};
        // clang-format off
        options.add_options()
            ("i,input", "Input scene filename", cxxopts::value<std::string>())
            ("o,output", "Output image filename", cxxopts::value<std::string>())
        ;
        // clang-format on
        options.parse_positional({"input", "output"});
        cxxopts::ParseResult args = options.parse(argc, argv);
        if (argc != 1)
            throw std::runtime_error{"too much arguments"};
        if (!args.count("input"))
            throw std::runtime_error{"no input file"};
        if (!args.count("output"))
            throw std::runtime_error{"no output file"};
    }
}  // namespace pathtracer
