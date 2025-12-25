#include <stdio.h>
#include "common.hpp"
#include "ring_buffer.hpp"
#include "logger.hpp"

int main() {
    scae::Buffer<2048> rb;
    scae::StdoutSink sink;

    scae::Logger<scae::LOGGING_LEVEL::INFO, scae::Buffer<2048>, scae::StdoutSink> logger(rb, sink);

    logger.log<scae::LOGGING_LEVEL::ERROR>("Fatal error occurred");
    logger.log<scae::LOGGING_LEVEL::DEBUG>("Debugging details"); // This will not be compiled

    printf("Flushing log buffer:\n");
    logger.flush();
    return 0;
}