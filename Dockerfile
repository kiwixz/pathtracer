FROM kiwixz/arch-vcpkg:2018-09-30

RUN echo -e "\n\e[1;97;44m > Installing system packages... \e[0m"  \
    && pacman --color=always --needed --noconfirm -Sy  \
        cmake gcc make  \
        clang diffutils  \
    \
    && echo -e "\n\e[1;97;44m > Installing dependencies... \e[0m"  \
    && bash -l -c "vcpkg install  \
        cxxopts fmt glm lodepng nlohmann-json spdlog tinyobjloader"  \
    \
    && echo -e "\n\e[1;97;44m > Cleanup... \e[0m"  \
    && kiwixz_cleanup

WORKDIR "/mnt/repo"
CMD ["/bin/bash"]
COPY "." "/mnt/repo/"
