#include "token.h"

#include <sstream>

namespace css
{
std::string Token::to_string()
{
	std::stringstream str;
	str << "{";
	str << " value: " << m_value;
	str << ", type: " << m_type;
	str << " }";
	return str.str();
}

}
