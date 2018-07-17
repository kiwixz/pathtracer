FROM kiwixz/arch-vcpkg:2018-07-15

RUN echo -e "\e[1;97;44m> Installing system packages...\e[0m"  \
    && pacman --color=always --needed --noconfirm -Sy cmake gcc make

RUN echo -e "\e[1;97;44m> Installing dependencies...\e[0m"  \
    && pacman --color=always --needed --noconfirm -Sy glm
    #&& vcpkg install

COPY "." "/tmp/repository/"
WORKDIR "/tmp/repository"

CMD echo -e "\e[1;97;44m> Dumping installed packages...\e[0m"  \
    && pacman --color=always -Q  \
    && vcpkg list  \
    \
    && echo -e "\e[1;97;44m> Building...\e[0m"  \
    && mkdir "build"  \
    && cd "build"  \
    && CXXFLAGS="-Werror -fdiagnostics-color=always -isystem /opt/vcpkg/installed/x64-linux/include"  \
        cmake "-DCMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake" ..  \
    && CLICOLOR_FORCE=1  \
        make -j$(nproc) -Orecurse