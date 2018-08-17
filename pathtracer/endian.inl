#pragma once

#include "endian.h"
#include <cstdint>
#include <type_traits>

namespace pathtracer::endian {
    namespace detail_endian {
        // clang-format off
        constexpr uint8_t swap_unsigned(uint8_t value)
        {
            return value;
        }
        constexpr uint16_t swap_unsigned(uint16_t value)
        {
            return ((value & 0x00ff) << 8)
                 | ((value & 0xff00) >> 8);
        }
        constexpr uint32_t swap_unsigned(uint32_t value)
        {
            return ((value & 0x000000ff) << 24)
                 | ((value & 0x0000ff00) << 8)
                 | ((value & 0x00ff0000) >> 8)
                 | ((value & 0xff000000) >> 24);
        }
        constexpr uint64_t swap_unsigned(uint64_t value)
        {
            return ((value & 0x00000000000000ffull) << 56)
                 | ((value & 0x000000000000ff00ull) << 40)
                 | ((value & 0x0000000000ff0000ull) << 24)
                 | ((value & 0x00000000ff000000ull) << 8)
                 | ((value & 0x000000ff00000000ull) >> 8)
                 | ((value & 0x0000ff0000000000ull) >> 24)
                 | ((value & 0x00ff000000000000ull) >> 40)
                 | ((value & 0xff00000000000000ull) >> 56);
        }
        // clang-format on
    }  // namespace detail_endian

    template <typename T>
    constexpr T swap(T value)
    {
        static_assert(std::is_integral_v<T> && !std::is_same_v<T, bool>, "trying to swap invalid type");
        return static_cast<T>(detail_endian::swap_unsigned(static_cast<std::make_unsigned_t<T>>(value)));
    }

    template <typename T>
    constexpr T host_to_big(T value)
    {
        return (Endianness::native == Endianness::big ? value : swap(value));
    }
    template <typename T>
    constexpr T host_to_little(T value)
    {
        return (Endianness::native == Endianness::little ? value : swap(value));
    }
    template <typename T>
    constexpr T big_to_host(T value)
    {
        return host_to_big(value);
    }
    template <typename T>
    constexpr T little_to_host(T value)
    {
        return host_to_little(value);
    }
}  // namespace pathtracer::endian
