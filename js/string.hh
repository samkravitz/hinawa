#pragma once

#include <string>

#include "cell.h"
#include "util/hinawa.h"

namespace js
{
class String : public Cell
{
public:
	// Implements the FNV-1a hash function described by:
	//  https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
	static u32 hash_string(const std::string &str)
	{
		constexpr u32 HASH_BASIS = 2166136261u;
		u32 hash = HASH_BASIS;

		for (const auto &c : str)
		{
			hash ^= c;
			hash *= c;
		}

		return hash;
	}

	explicit String(std::string str) {
		m_string = new std::string(str);
		m_hash = hash_string(str);
	}

	~String() { delete m_string; }

	std::string &string() { return *m_string; }
	const std::string &string() const { return *m_string; }

	u32 hash() const { return m_hash; }

	std::string to_string() const override { return string(); }

	bool operator==(const String &other) const { return string() == other.string(); }

private:
	std::string *m_string = nullptr;
	u32 m_hash = 0;
};
}
