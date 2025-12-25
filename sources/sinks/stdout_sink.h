#pragma once
#include <cinttypes>

namespace scae {
    struct StdoutSink {
        std::size_t write(const uint8_t* data, std::size_t n) noexcept;
    };
}