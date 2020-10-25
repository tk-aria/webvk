#pragma once

#include <cstdint>
#include <vector>

class Zstd {
public:
    bool compress(std::vector<uint8_t>& dest, const std::vector<uint8_t>& src, int level) const;
    bool decompress(std::vector<uint8_t>& dest, const std::vector<uint8_t>& src) const;
};
