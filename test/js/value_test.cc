#include <gtest/gtest.h>

#include <js/value.h>

namespace js
{
// StrictEqualityTests

TEST(StrictEqualityTests, UndefinedEqualsUndefined)
{
	Value a = {};
	Value b = {};
	EXPECT_TRUE(a.strict_eq(b));
	EXPECT_TRUE(a == b);
}

TEST(StrictEqualityTests, NullEqualsNull)
{
	auto a = Value::js_null();
	auto b = Value::js_null();
	EXPECT_TRUE(a.strict_eq(b));
	EXPECT_TRUE(a == b);
}

TEST(StrictEqualityTests, UndefinedNotEqualsNull)
{
	auto a = Value::js_null();
	auto b = Value::js_undefined();
	EXPECT_FALSE(a.strict_eq(b));
	EXPECT_FALSE(a == b);
}

// NaNTests
TEST(NaNTests, NaNIsNaN)
{
	auto nan = Value::js_nan();
	EXPECT_TRUE(nan.is_nan());

	auto not_nan = Value(5.0);
	EXPECT_FALSE(not_nan.is_nan());
}
}
