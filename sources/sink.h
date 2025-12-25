#pragma once
#include <cstdio>
#include <cinttypes>

namespace scae {
    struct StdoutSink {
        void write(const uint8_t* data, std::size_t n) noexcept {
            std::fwrite(data, 1, n, stdout);
        }
    };
}