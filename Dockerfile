FROM kiwixz/arch-vcpkg:2018-07-29

RUN echo -e "\e[1;97;44m > Installing system packages... \e[0m"  \
    && pacman --color=always --needed --noconfirm -Sy  \
        cmake gcc make  \
        clang

SHELL ["/bin/sh", "-l", "-c"]
RUN echo -e "\e[1;97;44m > Installing dependencies... \e[0m"  \
    && vcpkg install cxxopts fmt glm lodepng nlohmann-json spdlog

COPY "." "/tmp/repository/"
WORKDIR "/tmp/repository"

CMD ["/bin/bash", "-l"]
