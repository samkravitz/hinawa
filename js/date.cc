#include "date.h"

#include <chrono>

#include "vm.h"

namespace js
{
Date::Date()
{
	auto time = std::chrono::system_clock::now();
	auto since_epoch = time.time_since_epoch();
	auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(since_epoch);
	m_time = millis.count();
}

Object *Date::prototype()
{
	return DatePrototype::the();
}

DatePrototype *DatePrototype::instance = nullptr;

DatePrototype::DatePrototype()
{
	set_native("getTime", [](auto &vm, const auto &argv) -> Value {
		auto *date = vm.current_this()->as_date();
		auto time = date->time();
		auto val = Value((double) time);
		return Value(val);
	});
}

DatePrototype *DatePrototype::the()
{
	if (!instance)
		instance = heap().allocate<DatePrototype>();

	return instance;
}

std::string Date::to_string() const
{
	return "{ Date }";
}
}
