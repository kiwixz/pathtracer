#pragma once

namespace pathtracer::endian {
    enum class Endianness {
#ifdef _WIN32
        little = 0,
        big = 1,
        native = little
#else
        little = __ORDER_LITTLE_ENDIAN__,
        big = __ORDER_BIG_ENDIAN__,
        native = __BYTE_ORDER__
#endif
    };

    template <typename T>
    constexpr T swap(T value);

    template <typename T>
    constexpr T host_to_big(T value);

    template <typename T>
    constexpr T host_to_little(T value);

    template <typename T>
    constexpr T big_to_host(T value);

    template <typename T>
    constexpr T little_to_host(T value);
}  // namespace pathtracer::endian

#include "app/endian.inl"
