//
// Created by hacketo on 11/08/16.
//

#include "util/genericvalue.h"
#include "gtest/gtest.h"

class FloatValueFixture : public ::testing::Test {
protected:
	virtual void TearDown(){

	}
	virtual void SetUp(){

	}
public:
	FloatValueFixture() : Test(){
		value = GenericValue::u_ptr(new FloatValue());
	}

	virtual ~FloatValueFixture(){};

	GenericValue::u_ptr value;
};

TEST_F(FloatValueFixture, float_notinitialized_check) {
	EXPECT_FALSE(value->is_initialized());
}

TEST_F(FloatValueFixture, float_setvalue_check) {
	value->setFloat(1.5);
	EXPECT_TRUE(value->is_initialized());
	EXPECT_FLOAT_EQ(1.5, value->getFloat());
}

TEST_F(FloatValueFixture, float_initialized_check) {
	FloatValue::u_ptr floatValue = GenericValue::u_ptr(new FloatValue(1.5));
	EXPECT_TRUE(floatValue->is_initialized());
	EXPECT_FLOAT_EQ(floatValue->getFloat(), 1.5);
}

TEST_F(FloatValueFixture, float_getvaluenotinitialized_check) {
	ASSERT_THROW(value->getFloat(), ValueNotInitializedException);
}

TEST_F(FloatValueFixture, float_isnotfloat_check) {
	EXPECT_TRUE(value->isFloat());
}

TEST_F(FloatValueFixture, float_isnotint_check) {
	EXPECT_FALSE(value->isInt());
}

TEST_F(FloatValueFixture, float_isnotobject_check) {
	EXPECT_FALSE(value->isObject());
}

TEST_F(FloatValueFixture, float_isarray_check) {
	EXPECT_FALSE(value->isArray());
}

TEST_F(FloatValueFixture, float_isnotstring_check) {
	EXPECT_FALSE(value->isString());
}

TEST_F(FloatValueFixture, float_isnotbool_check) {
	EXPECT_FALSE(value->isBool());
}

TEST_F(FloatValueFixture, float_isnotnull_check) {
	EXPECT_FALSE(value->isNull());
}

TEST_F(FloatValueFixture, float_assertionbool_check) {
	ASSERT_THROW(value->setBool(true), NotSupportedException);
	ASSERT_THROW(value->getBool(), NotSupportedException);
}

TEST_F(FloatValueFixture, float_assertionint_check) {
	ASSERT_THROW(value->setInt(0), NotSupportedException);
	ASSERT_THROW(value->getInt(), NotSupportedException);
}

TEST_F(FloatValueFixture, float_assertionstring_check) {
	ASSERT_THROW(value->setString(""), NotSupportedException);
	ASSERT_THROW(value->getString(), NotSupportedException);
}

TEST_F(FloatValueFixture, float_assertionobject_check) {
	ASSERT_THROW(value->set("key", value.get()), NotSupportedException);
	ASSERT_THROW(value->get("key"), NotSupportedException);
}

TEST_F(FloatValueFixture, float_assertionarray_check) {
	ASSERT_THROW(value->add(value.get()), NotSupportedException);
	ASSERT_THROW(value->at(0), NotSupportedException);
	ASSERT_THROW(value->size(), NotSupportedException);
}
