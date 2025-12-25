#pragma once
#include <cinttypes>
#include "common.hpp"
#include <mutex>
#include <atomic>

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
        HasSinkWrite SinkT
    >
    class Logger {
    public:
        explicit constexpr Logger(BufferT& rb, SinkT& sink) noexcept
            : logBuffer(rb), sink(sink) {}

        inline void debug(const char* message) noexcept {
            log<LOGGING_LEVEL::DEBUG>(message);
        }

        inline void info(const char* message) noexcept {
            log<LOGGING_LEVEL::INFO>(message);
        }
        inline void warn(const char* message) noexcept {
            log<LOGGING_LEVEL::WARN>(message);
        }

        inline void error(const char* message) noexcept {
            log<LOGGING_LEVEL::ERROR>(message);
        }

        void flush() noexcept {
            if (writtenBytes.load() == 0) {
                return;
            }
            std::lock_guard<std::mutex> flushLock(flushMtx);
            uint8_t temp[256];
            while (true) {
                std::size_t n = 0;
                { // Locking the buffer while reading
                    std::lock_guard<std::mutex> lock(mtx);
                    n = logBuffer.read(temp, sizeof(temp));
                }
                if (n == 0) {
                    break;
                }
                (void)sink.write(temp, n);
            }
            writtenBytes.store(0);
        }
    private:
        template <LOGGING_LEVEL LevelToLog>
        void log(const char* message) noexcept {
            if constexpr (LevelToLog > MaxLevel) {
                return;
            }
            if (message == nullptr || message[0] == '\0') {
                return;
            }

            char line[256];

            const long long unsigned int ts = scae::timestampMs();
            const char* lvlStr = levelToString(LevelToLog);

            const int rc = std::snprintf(
                line, sizeof(line),
                "%llu [%s]: %s\n",
                ts,
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

            { // Locking the buffer while writing
                std::lock_guard<std::mutex> lock(mtx);
                writtenBytes.fetch_add(static_cast<int>(toWrite), std::memory_order_relaxed);
                (void)logBuffer.write(reinterpret_cast<const uint8_t*>(line), toWrite);
            } // Unlocking the buffer
        }
    private:
        BufferT& logBuffer;
        SinkT& sink;
        std::mutex mtx;
        std::mutex flushMtx;
        std::atomic<int> writtenBytes = 0;
    };
} // namespace scae