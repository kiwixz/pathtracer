FROM kiwixz/arch-vcpkg:2018-07-29

RUN echo -e "\e[1;97;44m> Installing system packages...\e[0m"  \
    && pacman --color=always --needed --noconfirm -Sy cmake gcc make

SHELL ["/bin/sh", "-l", "-c"]
RUN echo -e "\e[1;97;44m> Installing dependencies...\e[0m"  \
    && vcpkg install cxxopts fmt glm lodepng nlohmann-json spdlog

COPY "." "/tmp/repository/"
WORKDIR "/tmp/repository"

CMD echo -e "\e[1;97;44m> Dumping dependencies...\e[0m"  \
    && vcpkg list  \
    \
    && echo -e "\n\e[1;97;44m> Building...\e[0m"  \
    && export CXXFLAGS="-fdiagnostics-color=always -isystem /opt/vcpkg/installed/x64-linux/include -L /opt/vcpkg/installed/x64-linux/lib"  \
    && export CLICOLOR_FORCE="1"  \
    && mkdir -p "build/debug"  \
    && cd "build/debug"  \
    && cmake "-DCMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake" "-DCMAKE_BUILD_TYPE=Debug" "../.."  \
    && make -j$(nproc) -Orecurse  \
    && mkdir "../release"  \
    && cd "../release"  \
    && cmake "-DCMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake" "-DCMAKE_BUILD_TYPE=RelWithDebInfo" "../.."  \
    && make -j$(nproc) -Orecurse
