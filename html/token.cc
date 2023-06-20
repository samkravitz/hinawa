#include "token.h"

#include <cassert>

namespace html
{
Token::Token(TokenType type) :
    m_type(type)
{ }

void Token::new_attribute()
{
	assert(is_tag());

	auto tag_data = std::get<TagData>(data);
	tag_data.attributes.push_back({});
	data = tag_data;
}

u32 Token::get_char() const
{
	assert(is_character());
	return std::get<u32>(data);
}

std::string &Token::tag_name()
{
	assert(is_tag());
	return std::get<TagData>(data).name;
}

std::string &Token::attribute_name()
{
	assert(is_tag());
	auto &tag_data = std::get<TagData>(data);
	assert(!tag_data.attributes.empty());
	return tag_data.attributes.back().first;
}

std::string &Token::attribute_value()
{
	assert(is_tag());
	auto &tag_data = std::get<TagData>(data);
	assert(!tag_data.attributes.empty());
	return tag_data.attributes.back().second;
}

std::string &Token::doctype_name()
{
	assert(is_doctype());
	return std::get<DoctypeData>(data).name;
}

std::string &Token::public_identifier()
{
	assert(is_doctype());
	return std::get<DoctypeData>(data).public_identifier;
}

std::string &Token::system_identifier()
{
	assert(is_doctype());
	return std::get<DoctypeData>(data).system_identifier;
}

std::string &Token::comment()
{
	assert(is_comment());
	return std::get<std::string>(data);
}

void Token::set_self_closing()
{
	assert(is_tag());
	std::get<TagData>(data).self_closing = true;
}

void Token::set_force_quirks()
{
	assert(is_doctype());
	std::get<DoctypeData>(data).force_quirks = true;
}

std::string Token::to_string() const
{
	return std::visit(
	    [this](auto &&arg) -> std::string {
		    using T = std::decay_t<decltype(arg)>;
		    if constexpr (std::is_same_v<T, DoctypeData>)
		    {
			    std::string res = "Doctype: ";
			    res += "name: " + arg.name + " ";
			    res += "public id: " + arg.public_identifier + " ";
			    res += "force quirks: " + std::to_string(arg.force_quirks);
			    return res;
		    }

		    else if constexpr (std::is_same_v<T, TagData>)
		    {
			    std::string res = "<";
			    if (is_end_tag())
				    res += '/';

			    res += arg.name;

			    for (auto attribute : arg.attributes)
			    {
				    res += " ";
				    res += attribute.first;
				    res += "=";
				    res += "\"";
				    res += attribute.second;
				    res += "\"";
			    }

			    if (arg.self_closing)
				    res += '/';

			    res += '>';
			    return res;
		    }

		    else if constexpr (std::is_same_v<T, u32>)
		    {
			    return std::string(1, arg);
		    }

		    else if constexpr (std::is_same_v<T, std::string>)
		    {
			    return "Comment: " + arg;
		    }

		    else if constexpr (std::is_same_v<T, std::monostate>)
		    {
			    return "EOF";
		    }

		    return "Unknown variant!";
	    },
	    data);
}
}
