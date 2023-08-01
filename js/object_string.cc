#include "object_string.h"

#include "vm.h"

namespace js
{
ObjectString::ObjectString(const std::string &string) :
    ObjectString(new std::string(string))
{ }

ObjectString::ObjectString(std::string *string) :
    primitive_string(string)
{ }

ObjectString::~ObjectString()
{
	if (primitive_string)
		delete primitive_string;
}

Object *ObjectString::prototype()
{
	return StringPrototype::the();
}

StringPrototype *StringPrototype::instance = nullptr;

StringPrototype::StringPrototype()
{
	set_native("charAt", [](auto &vm, const auto &argv) -> Value {
		// TODO - arguments checking and validity.
		// This is wildly unsafe as is
		int index = (int) argv[0].as_number();
		auto *string_object = static_cast<ObjectString *>(vm.current_this());
		auto *underlying_string = string_object->primitive_string;
		auto *allocated_string = new std::string(1, underlying_string->at(index));
		return Value(heap().allocate<ObjectString>(allocated_string));
	});
}

StringPrototype *StringPrototype::the()
{
	if (!instance)
		instance = heap().allocate<StringPrototype>();

	return instance;
}
}
