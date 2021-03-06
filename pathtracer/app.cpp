#include "app.h"
#include <pathtrace/renderer.h>
#include <pathtracer/endian.h>
#include <cxxopts.hpp>
#include <lodepng.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <chrono>
#include <filesystem>
#include <fstream>

namespace pathtracer {
    namespace {
        using ProfClock = std::chrono::high_resolution_clock;

        struct AppArgs {
            bool help;
            std::string input;
            std::string output;
            bool dump;
            bool watch;

            double gamma;
            bool tag_software;
            bool tag_source;

            double dithering;
            size_t nr_threads;
        };


        constexpr std::string_view version = "(build " __DATE__ " " __TIME__ ")";


        pathtrace::Scene load_scene(const std::string& path)
        {
            pathtrace::Scene scene;

            try {
                scene.load_from_file(path);
                return scene;
            }
            catch (const std::exception& ex) {
                spdlog::get("stderr")->debug("could not load scene as JSON file: {}", ex.what());
            }


            unsigned err;
            std::vector<uint8_t> file;
            err = lodepng::load_file(file, path);
            if (err)
                throw std::runtime_error{"could not open input file"};

            std::vector<uint8_t> image;
            unsigned width;
            unsigned height;
            lodepng::State state;
            err = lodepng::decode(image, width, height, state, file);
            if (err)
                throw std::runtime_error{"could not load scene"};

            std::string source;
            for (size_t i = 0; i < state.info_png.text_num; ++i)
                if (state.info_png.text_keys[i] == std::string{"Source"})
                    source = state.info_png.text_strings[i];
            scene.load_from_json(source);

            return scene;
        }

        std::string format_time(double seconds)
        {
            int minutes = static_cast<int>(seconds) / 60;
            seconds -= minutes * 60;
            int hours = minutes / 60;
            minutes -= hours * 60;

            return fmt::format("{:02d}:{:02d}:{:05.2f}", hours, minutes, seconds);
        }

        void full_render(const AppArgs& args)
        {
            using namespace std::string_literals;

            std::shared_ptr<spdlog::logger> logger = spdlog::get("stderr");

            pathtrace::Scene scene = load_scene(args.input);

            logger->info("generating image...");
            ProfClock::time_point start = ProfClock::now();
            pathtrace::Image image = pathtrace::Renderer{args.nr_threads}.render(scene);
            std::string duration = format_time(std::chrono::duration<double>{ProfClock::now() - start}.count());
            logger->info("generated image in {}", duration);

            lodepng::State state;
            state.info_raw.bitdepth = 8;
            state.info_raw.colortype = LodePNGColorType::LCT_RGB;
            state.encoder.zlibsettings.nicematch = 258;     //  for optimal compression
            state.encoder.zlibsettings.windowsize = 32768;  //

            LodePNGInfo& info = state.info_png;
            if (args.gamma) {
                auto gamma = static_cast<uint32_t>(1 / args.gamma * 100'000);
                gamma = endian::host_to_big(gamma);
                lodepng_chunk_create(&info.unknown_chunks_data[0], &info.unknown_chunks_size[0],
                                     sizeof(gamma), "gAMA", reinterpret_cast<const uint8_t*>(&gamma));
            }
            if (args.tag_software)
                lodepng_add_text(&info, "Software", fmt::format("pathtrace {}", version).c_str());
            if (args.tag_source)
                lodepng_add_text(&info, "Source", scene.save_to_json().c_str());

            std::vector<uint8_t> png;
            unsigned err = lodepng::encode(png, image.convert<uint8_t>(args.dithering), image.width(), image.height(), state);
            if (err)
                throw std::runtime_error{"lodepng: "s + lodepng_error_text(err)};

            std::ofstream ofs{args.output, std::ios_base::out | std::ios_base::binary};
            if (!ofs)
                throw std::runtime_error{"could not open output file"};
            ofs.write(reinterpret_cast<const char*>(png.data()), png.size());

            logger->info("image written");
        }

        void do_args(const AppArgs& args)
        {
            if (args.dump) {
                load_scene(args.input).save_to_file(args.output);
                spdlog::get("stderr")->info("scene dumped");
            }
            else
                full_render(args);
        }
    }  // namespace

    int App::main(int argc, char** argv) noexcept
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

    void App::main_impl(int argc, char** argv)
    {
        AppArgs args;

        cxxopts::Options options{"pathtracer"};
        // clang-format off
        options.add_options()
            ("h,help", "Show help", cxxopts::value(args.help)->default_value("false"))
            ("i,input", "Input filename (JSON scene, or PNG with source tag)", cxxopts::value(args.input))
            ("o,output", "Output filename", cxxopts::value(args.output))
            ("d,dump", "Dump scene instead of rendering it", cxxopts::value(args.dump)->default_value("false"))
            ("w,watch", "Watch input file for modification instead of exiting", cxxopts::value(args.watch)->default_value("false"))
        ;
        options.add_options("metadata")
            ("gamma", "Fake gamma", cxxopts::value(args.gamma)->default_value("0.0"))
            ("tag-software", "Allow software tag", cxxopts::value(args.tag_software)->default_value("true"))
            ("tag-source", "Embed scene as source tag", cxxopts::value(args.tag_source)->default_value("true"))
        ;
        options.add_options("rendering")
            ("dithering", "Ratio of dithering", cxxopts::value(args.dithering)->default_value("1.0"))
            ("t,threads", "Number of threads", cxxopts::value(args.nr_threads)->default_value("0"))
        ;
        // clang-format on

        options.parse_positional({"input", "output"});
        options.positional_help("[INPUT] [OUTPUT]").show_positional_help();

        if (argc == 1) {
            fmt::print("{}", options.help({}));
            return;
        }

        cxxopts::ParseResult parse = options.parse(argc, argv);
        if (args.help) {
            fmt::print("{}", options.help({}));
            return;
        }

        // validate arguments
        if (argc != 1)
            throw std::runtime_error{"too much arguments"};
        if (!parse.count("input"))
            throw std::runtime_error{"no input file"};
        if (!parse.count("output"))
            throw std::runtime_error{"no output file"};

        if (!args.watch) {
            do_args(args);
            return;
        }

        std::shared_ptr<spdlog::logger> logger = spdlog::get("stderr");
        std::filesystem::path input_path = args.input;
        std::filesystem::file_time_type last;
        while (true) {
            std::filesystem::file_time_type new_last = std::filesystem::last_write_time(input_path);
            if (new_last > last) {
                try {
                    do_args(args);
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
