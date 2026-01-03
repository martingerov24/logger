#include "file_sink.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <mutex>
#include <string>

using namespace scae;

inline size_t getFileSize(const int fd) noexcept {
    std::size_t sizeOfFile = 0;
    if (fd == -1) {
        return false;
    }

    struct stat st;
    if (fstat(fd, &st) != 0) {
        return (off_t)-1;
    }
    return st.st_size;
}

bool FileSink::createRotatingFile() noexcept {
    if (fd == -1) {
        fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
        return fd != -1;
    }
    std::size_t sizeOfFile = getFileSize(fd);
    if (sizeOfFile == (std::size_t)-1) {
        return false;
    }
    if (sizeOfFile < static_cast<std::size_t>(maxMb) * 1024 * 1024) {
        return true;
    }
    close(fd);

    fd = -1;
    currentLog++;
    if (currentLog >= maxFiles && maxFiles > 0) {
        currentLog = 0;
    }

    const std::string rotatedFilename = std::string(filename) + std::to_string(currentLog) + ".old";

    (void)unlink(rotatedFilename.c_str());
    (void)rename(filename, rotatedFilename.c_str());

    fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
    return fd != -1;
}

bool FileSink::init(
    const char* filename,
    const int maxMb,
    const int maxFiles
) noexcept {
    std::snprintf(this->filename, sizeof(filename), "%s", filename);
    this->maxMb = maxMb;
    this->maxFiles = maxFiles;
    return createRotatingFile();
}

std::size_t FileSink::write(
    const uint8_t* data,
    std::size_t size
) noexcept {
    static std::mutex fileMtx;
    std::lock_guard<std::mutex> lock(fileMtx);
    if (createRotatingFile() == false) {
        return 0;
    }
    if (fd != -1) {
        return ::write(fd, data, size);
    }
    return 0;
}

bool FileSink::deinit () noexcept {
    if (fd != -1) {
        close(fd);
        fd = -1;
        return true;
    }
    return false;
}

FileSink::~FileSink() {
    deinit();
}
