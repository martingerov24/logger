#pragma once
#include <cinttypes>

namespace scae {
    class FileSink {
    private:
        bool deinit () noexcept;
        bool createRotatingFile() noexcept;
    public:
        FileSink() = default;
        bool init(
            const char* filename,
            const int maxMb
        ) noexcept;

        std::size_t write(
            const uint8_t* data,
            std::size_t size
        ) noexcept;

        ~FileSink();
    private:
        const char* filename = nullptr;
        int fd = -1;
        int maxMb = 50;
        int currentLog = 0;
    };
}
