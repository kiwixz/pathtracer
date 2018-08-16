FROM kiwixz/arch-vcpkg:2018-08-05

SHELL ["/bin/sh", "-l", "-c"]

RUN echo -e "\e[1;97;44m > Installing system packages... \e[0m"  \
    && pacman --color=always --needed --noconfirm -Sy  \
        cmake gcc make  \
        clang diffutils  \
        doxygen graphviz

RUN echo -e "\e[1;97;44m > Installing dependencies... \e[0m"  \
    && vcpkg install cxxopts fmt glm lodepng nlohmann-json spdlog tinyobjloader

COPY "." "/mnt/repo/"
WORKDIR "/mnt/repo"

CMD ["/bin/bash", "-l"]
