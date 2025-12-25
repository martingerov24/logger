#include "stdout_sink.h"
#include <cstdio>

using namespace scae;
std::size_t StdoutSink::write(const uint8_t* data, std::size_t n) noexcept {
    return std::fwrite(data, 1, n, stdout);
}