#include <stdio.h>
#include "common.hpp"
#include "ring_buffer.hpp"
#include "stdout_sink.h"
#include "file_sink.h"
#include "logger.hpp"
#include <thread>
#include <vector>

scae::Buffer<2048> rb;
scae::StdoutSink sink;
scae::FileSink fileSink;

scae::Logger<scae::LOGGING_LEVEL::INFO, scae::Buffer<2048>, scae::FileSink> logger(rb, fileSink);

// Testing function
// ------------------------------
// Write a test that spawns multiple threads that log messages concurrently.
// Each thread should log a series of messages at different levels.
// After all threads have finished, ensure that the logger has flushed all messages.
// ------------------------------
void testing();

int main() {
    fileSink.init("log.txt", 5);

    testing();
    return 0;
}

void testing() {
    constexpr int kNumWorkers = 8;
    constexpr int kMsgsPerWorker = 2000;

    std::atomic<bool> start{false};
    std::atomic<bool> stop_flusher{false};

    std::thread flusher([&] {
        while (start.load(std::memory_order_acquire) == false) {
            std::this_thread::yield();
        }

        while (stop_flusher.load(std::memory_order_acquire) == false) {
            logger.flush();
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        logger.flush();
    });

    std::vector<std::thread> workers;
    workers.reserve(kNumWorkers);

    for (int t = 0; t < kNumWorkers; ++t) {
        workers.emplace_back([&, t] {
            while (!start.load(std::memory_order_acquire)) {
                std::this_thread::yield();
            }

            char msg[128];

            for (int i = 0; i < kMsgsPerWorker; ++i) {
                const int n = std::snprintf(
                    msg, sizeof(msg),
                    "thread=%d i=%d hello from worker",
                    t, i
                );
                if (n <= 0) {
                    continue;
                }
                if ((i % 20) == 0) {
                    logger.error(msg);
                } else if ((i % 7) == 0) {
                    logger.warn(msg);
                } else {
                    logger.info(msg);
                }

                logger.debug("debug: should not be compiled for MaxLevel=INFO");
            }
        });
    }

    start.store(true, std::memory_order_release);

    for (auto& th : workers) {
        th.join();
    }

    stop_flusher.store(true, std::memory_order_release);
    flusher.join();
    logger.flush();
}