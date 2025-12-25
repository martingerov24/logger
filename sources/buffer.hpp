#pragma once
#include "common.hpp"
#include <array>
#include <cinttypes>

namespace scae {
    //
    template<std::size_t N>
    class Buffer {
    private:
        static_assert(isPow2(N), "Buffer size must be a power of two");
    public:
        constexpr std::size_t size() const noexcept;
        constexpr std::size_t capacity() const noexcept;
        constexpr bool isEmpty() const noexcept;
        constexpr bool isFull() const noexcept;
        constexpr Buffer();
        const std::size_t write(
            const uint8_t* src,
            std::size_t len
        ) noexcept;
        const std::size_t read(
            uint8_t* dst,
            std::size_t len
        ) noexcept;
    private:
        std::array<uint8_t, N> data;
        std::size_t head = 0;
        std::size_t tail = 0;
    };
}

#include "buffer.cpp"