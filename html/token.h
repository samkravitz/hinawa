#pragma once

#include <utility>
#include <string>
#include <variant>
#include <vector>

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

	inline bool is_doctype() { return m_type == Doctype; }
	inline bool is_start_tag() { return m_type == StartTag; }
	inline bool is_end_tag() { return m_type == EndTag; }
	inline bool is_character() { return m_type == Character; }
	inline bool is_comment() { return m_type == Comment; }
	inline bool is_eof() { return m_type == Eof; }
	inline bool is_tag() { return is_start_tag() || is_end_tag(); }
	inline TokenType type() { return m_type; }

	static Token make_doctype()
	{
		Token t(Doctype);
		t.data = DoctypeData {};
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

	static Token make_character(char c)
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

	inline TagData as_tag_data() { return std::get<TagData>(data); }

	void new_attribute();
	void append_tag_name(char c);
	void append_attribute_name(char c);
	void append_attribute_value(char c);
	void append_doctype_name(char c);
	void doctype_set_name(char c);
	void set_force_quirks();
	char get_char();
	std::string tag_name();

	std::string to_string();

private:
	TokenType m_type;
	std::variant<
		DoctypeData,   // Doctype
		TagData,       // StartTag, EndTag
		char,          // Character
		std::string,   // Comment
		std::monostate // EOF
	> data;
};
}
