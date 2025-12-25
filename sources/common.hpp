#pragma once
#include <cstddef>

#if defined(__x86_64__) || defined(_M_X64)
    #include <chrono>
#else
    #include "hal.h" //TODO: replace with proper time header
#endif

namespace scae {
    static consteval bool isPow2(std::size_t x) {
        return x && ((x & (x - 1)) == 0);
    }
    template <typename T>
    concept HasSinkWrite = requires(T& b, const uint8_t* p, std::size_t n) {
        { b.write(p, n) } noexcept -> std::same_as<void>;
    };

    template <typename T>
    concept HasBufferWrite = requires(T& b, const uint8_t* p, std::size_t n) {
        { b.write(p, n) } noexcept -> std::same_as<std::size_t>;
    };

    template <typename T>
    concept HasBufferRead = requires(T& b, uint8_t* p, std::size_t n) {
        { b.read(p, n) } noexcept -> std::same_as<std::size_t>;
    };

    template <typename T>
    concept BufferRefOk = HasBufferWrite<T> && HasBufferRead<T> && (!std::is_const_v<T>);

    inline uint64_t timestampMs() noexcept {
        #if defined(__x86_64__) || defined(_M_X64)
            using namespace std::chrono;
            return static_cast<uint64_t>(
                duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count()
            );
        #else
            return static_cast<uint64_t>(HAL_GetTick()); // Convert to MS
        #endif
    }
}