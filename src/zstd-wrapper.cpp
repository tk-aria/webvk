#include <cassert>
#include "zstd-wrapper.h"

bool Zstd::compress(std::vector<uint8_t> &dest, const std::vector<uint8_t> &src, int level) {
    // 圧縮後サイズの最悪値を取得する
    const auto buffer_size = ZSTD_compressBound(src.size());
    if (ZSTD_isError(buffer_size)) {
        return false;
    }

    // 圧縮結果を出力できるようにバッファサイズを変更する
    dest.resize(buffer_size);

    // 圧縮する
    const auto compressed_size = ZSTD_compress(dest.data(), dest.size(), src.data(), src.size(), level);
    if (ZSTD_isError(compressed_size)) {
        return false;
    }

    // 圧縮結果のサイズに変更する
    dest.resize(compressed_size);
    dest.shrink_to_fit();

    // 圧縮成功
    return true;
}

bool Zstd::decompress(std::vector<uint8_t> &dest, const std::vector<uint8_t> &src) {
    // 伸長後のサイズを取得する
    const auto dest_size = ZSTD_getDecompressedSize(src.data(), src.size());
    if (ZSTD_isError(dest_size)) {
        return false;
    }

    // 伸長結果を出力できるようにバッファサイズを変更する
    dest.resize(dest_size);

    // 伸長する
    const auto rc = ZSTD_decompress(dest.data(), dest.size(), src.data(), src.size());
    if (ZSTD_isError(rc)) {
        return false;
    }

    // 伸長に成功した場合はサイズが一致するはず
    assert(rc == dest_size);

    // 伸長成功
    return true;
}