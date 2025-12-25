#include <stdio.h>
#include "common.hpp"
#include "buffer.hpp"
#include "logger.hpp"

int main() {
    Buffer<2048> rb;
    StdoutSink sink;

    scae::Logger<scae::LOGGING_LEVEL::INFO, scae::Buffer<2048>, StdoutSink> logger(rb, sink);

    logger.log<scae::LOGGING_LEVEL::ERROR>("Fatal error occurred");
    logger.log<scae::LOGGING_LEVEL::DEBUG>("Debugging details"); // This will not be compiled

    printf("Flushing log buffer:\n");
    logger.flush();
    return 0;
}