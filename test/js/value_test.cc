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

// 0.0 and -0.0 tests
TEST(ZeroTests, ZeroTests)
{
	auto zero = Value::js_zero();
	auto negative_zero = Value::js_negative_zero();

	EXPECT_TRUE(zero.is_zero());
	EXPECT_FALSE(zero.is_negative_zero());
	EXPECT_FALSE(negative_zero.is_zero());
	EXPECT_TRUE(negative_zero.is_negative_zero());

	EXPECT_TRUE(zero.as_number() == negative_zero.as_number());
	EXPECT_TRUE(zero.strict_eq(negative_zero));
	EXPECT_TRUE(negative_zero.strict_eq(zero));

	EXPECT_FALSE(Value((double) 0).is_negative_zero());
	EXPECT_FALSE(Value(0.0).is_negative_zero());
	EXPECT_FALSE(Value(0.00).is_negative_zero());
	EXPECT_FALSE(Value(0.000).is_negative_zero());
	EXPECT_FALSE(Value(0.0000).is_negative_zero());
}

// to_string tests
TEST(ToStringTests, LargeIntegerToString)
{
	auto large_positive_integer = Value((double) 123456789012);
	auto large_negative_integer = Value((double) -987654321098);

	EXPECT_EQ(large_positive_integer.to_string(), "123456789012");
	EXPECT_EQ(large_negative_integer.to_string(), "-987654321098");
}

TEST(ToStringTests, SpecialNumbersToString)
{
	EXPECT_EQ(Value::js_infinity().to_string(), "Infinity");
	EXPECT_EQ(Value::js_negative_infinity().to_string(), "-Infinity");
	EXPECT_EQ(Value::js_zero().to_string(), "0");
	EXPECT_EQ(Value::js_negative_zero().to_string(), "-0");
	EXPECT_EQ(Value::js_nan().to_string(), "NaN");
}
}
