#pragma once

#include <cstdint>
#include <filesystem>

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using uint = unsigned;

struct Point
{
	int x;
	int y;
};

namespace fs = std::filesystem;
extern fs::path DATA_DIR;
