#pragma once
#include "common.hpp"
#include <array>
#include <cinttypes>

namespace scae {
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

    template<std::size_t N>
    constexpr std::size_t Buffer<N>::size() const noexcept {
        return (head - tail) & (N - 1);;
    }

    template<std::size_t N>
    constexpr std::size_t Buffer<N>::capacity() const noexcept {
        return N - 1;
    }

    template<std::size_t N>
    constexpr bool Buffer<N>::isEmpty() const noexcept {
        return head == tail;
    }

    template<std::size_t N>
    constexpr bool Buffer<N>::isFull() const noexcept {
        return ((head + 1) & (N - 1)) == tail;
    }

    template <std::size_t N>
    constexpr Buffer<N>::Buffer()
    : data{}
    , head(0)
    , tail(0) {}

    template <std::size_t N>
    const std::size_t Buffer<N>::write(
        const uint8_t* src,
        std::size_t len
    ) noexcept {
        std::size_t bytesWritten = 0;
        while (bytesWritten < len && isFull() == false) {
            data[head] = src[bytesWritten];
            head = (head + 1) & (N - 1);
            bytesWritten++;
        }
        return bytesWritten;
    }

    template <std::size_t N>
    const std::size_t Buffer<N>::read(
        uint8_t* dst,
        std::size_t len
    ) noexcept {
        std::size_t bytesRead = 0;
        while (bytesRead < len && isEmpty() == false) {
            dst[bytesRead] = data[tail];
            tail = (tail + 1) & (N - 1);
            bytesRead++;
        }
        return bytesRead;
    }
}