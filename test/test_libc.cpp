#include <array>
#include <cstdint>
#include <cstddef>
#include <iostream>

constexpr size_t SIZE = 47;

constexpr auto TEST_ARR = []() {
    std::array<uint8_t, SIZE> ret{};
    for (size_t i = 0; i < SIZE; i++) {
        ret[i] = i;
    }
    return ret;
}();

static uint8_t dst[SIZE] = {0};

void* mymemcpy(void* dst, const void* src, size_t nr_bytes)
{
    constexpr int INT_SIZE = sizeof(unsigned);

    size_t i;
    uint8_t* dst_small;
    const uint8_t* src_small;

    for (i = 0; i < (nr_bytes / INT_SIZE); i++)
        reinterpret_cast<unsigned*>(dst)[i] = reinterpret_cast<const unsigned*>(src)[i];

    dst_small = &reinterpret_cast<uint8_t*>(dst)[INT_SIZE * i];
    src_small = &reinterpret_cast<const uint8_t*>(src)[INT_SIZE * i];
    
    for (i = 0; i < (nr_bytes & (INT_SIZE - 1)); i++)
        dst_small[i] = src_small[i];

    return dst;
}

int main()
{
    mymemcpy(dst, TEST_ARR.data(), TEST_ARR.size() * sizeof(TEST_ARR[0]));

    for (size_t i = 0; i < SIZE; i++) {
        if (dst[i] != TEST_ARR[i]) {
            std::cout << "FAIL: expected: " << TEST_ARR[i] << ", got: " << dst[i] << std::endl;
        }
    }

    return 0;
}
