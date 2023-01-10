#pragma once

#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "util/hinawa.h"

namespace html
{
enum TokenType
{
	Doctype,
	StartTag,
	EndTag,
	Character,
	Comment,
	Eof,
};

struct DoctypeData
{
	std::string name;
	std::string public_identifier;
	std::string system_identifier;
	bool force_quirks = false;
};

struct TagData
{
	std::string name;
	std::vector<std::pair<std::string, std::string>> attributes;
	bool self_closing = false;
};

class Token
{
public:
	Token(TokenType);
	Token() = default;

	operator bool() const { return m_type != Eof; }

	inline bool is_doctype() const { return m_type == Doctype; }
	inline bool is_start_tag() const { return m_type == StartTag; }
	inline bool is_end_tag() const { return m_type == EndTag; }
	inline bool is_character() const { return m_type == Character; }
	inline bool is_comment() const { return m_type == Comment; }
	inline bool is_eof() const { return m_type == Eof; }
	inline bool is_tag() const { return is_start_tag() || is_end_tag(); }
	inline TokenType type() const { return m_type; }

	static Token make_doctype()
	{
		Token t(Doctype);
		t.data = DoctypeData{};
		return t;
	}

	static Token make_start_tag()
	{
		Token t(StartTag);
		t.data = TagData{};
		return t;
	}

	static Token make_end_tag()
	{
		Token t(EndTag);
		t.data = TagData{};
		return t;
	}

	static Token make_character(u32 c)
	{
		Token t(Character);
		t.data = c;
		return t;
	}

	static Token make_comment()
	{
		Token t(Comment);
		t.data = "";
		return t;
	}

	static Token make_eof()
	{
		Token t(Eof);
		t.data = std::monostate{};
		return t;
	}

	inline TagData as_tag_data() const { return std::get<TagData>(data); }

	void new_attribute();
	std::string &tag_name();
	std::string &attribute_name();
	std::string &attribute_value();
	std::string &doctype_name();
	std::string &comment();
	void set_force_quirks();
	void set_self_closing();
	u32 get_char() const;

	std::string to_string() const;

private:
	TokenType m_type;
	std::variant<
		DoctypeData,      // Doctype
		TagData,          // StartTag, EndTag
		u32,              // Character
		std::string,      // Comment
		std::monostate    // EOF
	> data;
};
}
