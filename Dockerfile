FROM kiwixz/arch-vcpkg:2018-08-05

RUN echo -e "\e[1;97;44m > Installing system packages... \e[0m"  \
    && pacman --color=always --needed --noconfirm -Sy  \
        cmake gcc make  \
        clang diffutils

RUN echo -e "\e[1;97;44m > Installing dependencies... \e[0m"  \
    && bash -l -c "vcpkg install  \
        cxxopts fmt glm lodepng nlohmann-json spdlog tinyobjloader"

WORKDIR "/mnt/repo"
CMD ["/bin/bash"]
COPY "." "/mnt/repo/"
