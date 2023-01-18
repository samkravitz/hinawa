#include "object_string.h"

#include "vm.h"

namespace js
{
ObjectString::ObjectString(std::string *string) :
    primitive_string(string)
{ }

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
		auto *string_object = static_cast<ObjectString*>(vm.current_this());
		auto *underlying_string = string_object->primitive_string;
		return Value(new std::string(1, underlying_string->at(index)));
	});
}

StringPrototype *StringPrototype::the()
{
	if (!instance)
		instance = new StringPrototype;

	return instance;
}
}
