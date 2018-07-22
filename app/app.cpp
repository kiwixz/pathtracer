#include "app/app.h"
#include "pathtrace/renderer.h"
#include <cxxopts.hpp>
#include <lodepng.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <fstream>

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
        catch (const std::exception& ex) {
            logger->error("exception: {}", ex.what());
            return 1;
        }
        return 0;
    }

    void App::main_impl(int argc, const char** argv)
    {
        using namespace std::string_literals;

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

        std::string input = args["input"].as<std::string>();
        std::string output = args["output"].as<std::string>();

        Scene scene;
        Image image = Renderer{}.render(scene);

        lodepng::State state;
        state.info_raw.bitdepth = 8;
        state.info_raw.colortype = LodePNGColorType::LCT_RGB;
        state.encoder.zlibsettings.nicematch = 258;     //  for optimal compression
        state.encoder.zlibsettings.windowsize = 32768;  //

        std::vector<uint8_t> png;
        unsigned err = lodepng::encode(png, image.convert_to_24_bits(), image.width(), image.height(), state);
        if (err)
            throw std::runtime_error{"lodepng: "s + lodepng_error_text(err)};

        std::ofstream ofs{output, std::ios_base::out | std::ios_base::binary};
        if (!ofs)
            throw std::runtime_error{"could not open output file"};
        ofs.write(reinterpret_cast<const char*>(png.data()), png.size());
    }
}  // namespace pathtracer
