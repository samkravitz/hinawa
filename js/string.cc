#include "string.hh"

namespace js
{
String::String(const std::string &string) :
    mstring(std::move(string))
{ }

Object *String::prototype()
{
	return StringPrototype::the();
}

StringPrototype *StringPrototype::instance = nullptr;

StringPrototype::StringPrototype() { }

StringPrototype *StringPrototype::the()
{
	if (instance == nullptr)
		instance = new StringPrototype;

	return instance;
}
}
