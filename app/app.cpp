#include "app/app.h"
#include "pathtrace/renderer.h"
#include <cxxopts.hpp>
#include <lodepng.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <filesystem>
#include <fstream>

namespace pathtracer {
    namespace {
        void full_render(const std::string& input, const std::string& output, float dithering)
        {
            using namespace std::string_literals;

            std::shared_ptr<spdlog::logger> logger = spdlog::get("stderr");

            Scene scene;
            scene.load_from_file(input);
            Image image = Renderer{}.render(scene);

            lodepng::State state;
            state.info_raw.bitdepth = 8;
            state.info_raw.colortype = LodePNGColorType::LCT_RGB;
            state.encoder.zlibsettings.nicematch = 258;     //  for optimal compression
            state.encoder.zlibsettings.windowsize = 32768;  //

            std::vector<uint8_t> png;
            unsigned err = lodepng::encode(png, image.convert<uint8_t>(dithering), image.width(), image.height(), state);
            if (err)
                throw std::runtime_error{"lodepng: "s + lodepng_error_text(err)};

            std::ofstream ofs{output, std::ios_base::out | std::ios_base::binary};
            if (!ofs)
                throw std::runtime_error{"could not open output file"};
            ofs.write(reinterpret_cast<const char*>(png.data()), png.size());

            logger->info("image written");
        }
    }  // namespace

    int App::main(int argc, const char** argv) noexcept
    {
        spdlog::stderr_color_mt("stderr");
#ifdef _DEBUG
        spdlog::set_level(spdlog::level::debug);
#endif
        try {
            spdlog::set_pattern("%^[%H:%M:%S.%f][%t][%l] %v%$");
            main_impl(argc, argv);
            spdlog::drop_all();
        }
        catch (const std::exception& ex) {
            spdlog::get("stderr")->error("exception: {}", ex.what());
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
            ("w,watch", "Watch input file for modification instead of exiting", cxxopts::value<bool>()->default_value("false"))
        ;
        options.add_options("processing")
            ("d,dithering", "Ratio of dithering", cxxopts::value<float>()->default_value("1.0"))
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

        bool watch = args["watch"].as<bool>();
        std::string input = args["input"].as<std::string>();
        std::string output = args["output"].as<std::string>();
        float dithering = args["dithering"].as<float>();

        if (!watch) {
            full_render(input, output, dithering);
            return;
        }

        std::shared_ptr<spdlog::logger> logger = spdlog::get("stderr");
        std::filesystem::file_time_type last;
        while (true) {
            std::filesystem::file_time_type new_last = std::filesystem::last_write_time(input);
            if (new_last > last) {
                try {
                    full_render(input, output, dithering);
                }
                catch (const std::exception& ex) {
                    logger->error("exception: {}", ex.what());
                }
                last = new_last;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }
}  // namespace pathtracer
