#pragma once
#include <cinttypes>
#include <cstring>

namespace scae {
    /// Stack allocator with predefined max size
    /// The total memory is 64 byte aligned, all but the first allocation are not guaranteed to be aligned
    /// Can only free all the allocations at once
    class StackAllocator {
    public:
        StackAllocator(uint8_t *ptr, int bytes)
            : totalBytes(bytes)
            , data(ptr) {
        }

        /// Allocate memory for @count T objects
        /// Does *NOT* call constructors
        /// @param count - the number of objects needed
        /// @return pointer to the allocated memory or nullptr
        template <typename T>
        T *alloc(int count) {
            const int size = count * sizeof(T);
            if (idx + size > totalBytes) {
                return nullptr;
            }
            uint8_t *start = data + idx;
            idx += size;
            return reinterpret_cast<T *>(start);
        }

        /// De-allocate all the memory previously allocated with @alloc
        void freeAll() {
            idx = 0;
        }

        /// Get the max number of bytes that can be allocated by the allocator
        int maxBytes() const {
            return totalBytes;
        }

        /// Get the free space that can still be allocated, same as maxBytes before any allocations
        int freeBytes() const {
            return totalBytes - idx;
        }

        void zeroAll() const {
            memset(data, 0, totalBytes);
        }

        StackAllocator(const StackAllocator &) = delete;
        StackAllocator &operator=(const StackAllocator &) = delete;
    private:
        const int totalBytes;
        int idx = 0;
        uint8_t *data = nullptr;
    };
}