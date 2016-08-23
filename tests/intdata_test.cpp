//
// Created by hacketo on 11/08/16.
//

#include "util/genericvalue.h"
#include "gtest/gtest.h"

class IntValueFixture : public ::testing::Test {
protected:
	virtual void TearDown(){

	}
	virtual void SetUp(){

	}
public:
	IntValueFixture() : Test(){
		value = GenericValue::u_ptr(new IntValue());
	}

	virtual ~IntValueFixture(){}

	GenericValue::u_ptr value;
};

TEST_F(IntValueFixture, int_notinitialized_check) {
	EXPECT_FALSE(value->is_initialized());
}

TEST_F(IntValueFixture, int_setvalue_check) {
	value->setInt(1);
	EXPECT_TRUE(value->is_initialized());
	EXPECT_EQ(1, value->getInt());
}

TEST_F(IntValueFixture, int_initialized_check) {
	IntValue::u_ptr intValue = GenericValue::u_ptr(new IntValue(2));
	EXPECT_TRUE(intValue->is_initialized());
	EXPECT_EQ(intValue->getInt(), 2);
}

TEST_F(IntValueFixture, int_getvaluenotinitialized_check) {
	ASSERT_THROW(value->getInt(), ValueNotInitializedException);
}

TEST_F(IntValueFixture, int_isnotint_check) {
	EXPECT_TRUE(value->isInt());
}

TEST_F(IntValueFixture, int_isnotobject_check) {
	EXPECT_FALSE(value->isObject());
}

TEST_F(IntValueFixture, int_isarray_check) {
	EXPECT_FALSE(value->isArray());
}

TEST_F(IntValueFixture, int_isnotstring_check) {
	EXPECT_FALSE(value->isString());
}

TEST_F(IntValueFixture, int_isnotbool_check) {
	EXPECT_FALSE(value->isBool());
}

TEST_F(IntValueFixture, int_isnotnull_check) {
	EXPECT_FALSE(value->isNull());
}

TEST_F(IntValueFixture, int_isnotfloat_check) {
	EXPECT_FALSE(value->isFloat());
}

TEST_F(IntValueFixture, int_assertionbool_check) {
	ASSERT_THROW(value->setBool(true), NotSupportedException);
	ASSERT_THROW(value->getBool(), NotSupportedException);
}

TEST_F(IntValueFixture, int_assertionfloat_check) {
	ASSERT_THROW(value->setFloat(0.1), NotSupportedException);
	ASSERT_THROW(value->getFloat(), NotSupportedException);
}

TEST_F(IntValueFixture, int_assertionstring_check) {
	ASSERT_THROW(value->setString(""), NotSupportedException);
	ASSERT_THROW(value->getString(), NotSupportedException);
}

TEST_F(IntValueFixture, int_assertionobject_check) {
	ASSERT_THROW(value->set("key", value.get()), NotSupportedException);
	ASSERT_THROW(value->get("key"), NotSupportedException);
}

TEST_F(IntValueFixture, int_assertionarray_check) {
	ASSERT_THROW(value->add(value.get()), NotSupportedException);
	ASSERT_THROW(value->at(0), NotSupportedException);
	ASSERT_THROW(value->size(), NotSupportedException);
}
