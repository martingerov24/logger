#pragma once
#include <cinttypes>
#include "sink.h"
#include "common.hpp"

namespace scae {
    enum class LOGGING_LEVEL : uint8_t {
        ERROR = 0,
        WARN  = 1,
        INFO  = 2,
        DEBUG = 3,
        OFF   = 255
    };
    constexpr const char* levelToString(LOGGING_LEVEL lvl) noexcept {
        switch (lvl) {
            case LOGGING_LEVEL::DEBUG: return "DEBUG";
            case LOGGING_LEVEL::INFO: return "INFO";
            case LOGGING_LEVEL::WARN: return "WARN";
            case LOGGING_LEVEL::ERROR: return "ERROR";
            default: return "UNKNOWN";
        }
    }


    // Add functions for the different logging levels, so no template is passed, add the file and line.
    // Add string view implementation
    template <
        LOGGING_LEVEL MaxLevel,
        BufferRefOk BufferT,
        HasSinkWrite SinkT = StdoutSink
    >
    class Logger {
    public:
        explicit constexpr Logger(BufferT& rb, SinkT& sink) noexcept
            : logBuffer(rb), sink(sink) {}

        template <LOGGING_LEVEL LevelToLog>
        void log(const char* message) noexcept {
            if constexpr (LevelToLog > MaxLevel) {
                return;
            }
            if (message == nullptr || message[0] == '\0') {
                return;
            }

            char line[256];

            const uint64_t ts = scae::timestampMs();
            const char* lvlStr = levelToString(LevelToLog);

            const int rc = std::snprintf(
                line, sizeof(line),
                "%llu [%s]: %s\n",
                static_cast<unsigned long long>(ts),
                lvlStr,
                message
            );
            if (rc <= 0) {
                return;
            }

            const std::size_t toWrite =
                (static_cast<std::size_t>(rc) >= sizeof(line))
                    ? (sizeof(line) - 1)
                    : static_cast<std::size_t>(rc);

            (void)logBuffer.write(reinterpret_cast<const uint8_t*>(line), toWrite);
        }

        void flush() noexcept {
            uint8_t temp[256];
            while (true) {
                const std::size_t n = logBuffer.read(temp, sizeof(temp));
                if (n == 0) {
                    break;
                }
                (void)sink.write(temp, n);
            }
        }

    private:
        BufferT& logBuffer;
        SinkT& sink;
    };
} // namespace scae