#pragma once

#include "utils/span.h"
#include <cstddef>

namespace @prefix@::embed
{
namespace detail
{

extern "C"
{
    extern const std::byte* @prefix@_embed_@name@_begin;
    extern const std::byte* @prefix@_embed_@name@_end;
    extern const size_t @prefix@_embed_@name@_size;
}

}  // namespace @prefix@::embed::detail


inline utils::Span<const std::byte> @name@() {
    static utils::Span<const std::byte> span{detail::@prefix@_embed_@name@_begin,
                                             detail::@prefix@_embed_@name@_size};
    return span;
}

}  // namespace @prefix@::embed
