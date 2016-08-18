//
// Created by hacketo on 11/08/16.
//

#include "../../src/util/genericvalue.h"
#include "gtest/gtest.h"

class NullValueFixture : public ::testing::Test {
protected:
	virtual void TearDown(){

	}
	virtual void SetUp(){

	}
public:
	NullValueFixture() : Test(){
		value = GenericValue::u_ptr(new NullValue());
	}

	virtual ~NullValueFixture(){}

	GenericValue::u_ptr value;
};

TEST_F(NullValueFixture, null_initialized_check) {
	EXPECT_TRUE(value->is_initialized());
}

TEST_F(NullValueFixture, null_isnotnull_check) {
	EXPECT_TRUE(value->isNull());
}

TEST_F(NullValueFixture, null_isnotint_check) {
	EXPECT_FALSE(value->isInt());
}

TEST_F(NullValueFixture, null_isnotobject_check) {
	EXPECT_FALSE(value->isObject());
}

TEST_F(NullValueFixture, null_isarray_check) {
	EXPECT_FALSE(value->isArray());
}

TEST_F(NullValueFixture, null_isnotstring_check) {
	EXPECT_FALSE(value->isString());
}

TEST_F(NullValueFixture, null_isnotbool_check) {
	EXPECT_FALSE(value->isBool());
}


TEST_F(NullValueFixture, null_isnotfloat_check) {
	EXPECT_FALSE(value->isFloat());
}

TEST_F(NullValueFixture, null_assertionbool_check) {
	ASSERT_THROW(value->setBool(true), NotSupportedException);
	ASSERT_THROW(value->getBool(), NotSupportedException);
}

TEST_F(NullValueFixture, null_assertionint_check) {
	ASSERT_THROW(value->setInt(0), NotSupportedException);
	ASSERT_THROW(value->getInt(), NotSupportedException);
}

TEST_F(NullValueFixture, null_assertionfloat_check) {
	ASSERT_THROW(value->setFloat(0.1), NotSupportedException);
	ASSERT_THROW(value->getFloat(), NotSupportedException);
}

TEST_F(NullValueFixture, null_assertionstring_check) {
	ASSERT_THROW(value->setString(""), NotSupportedException);
	ASSERT_THROW(value->getString(), NotSupportedException);
}

TEST_F(NullValueFixture, null_assertionobject_check) {
	ASSERT_THROW(value->set("key", value.get()), NotSupportedException);
	ASSERT_THROW(value->get("key"), NotSupportedException);
}

TEST_F(NullValueFixture, null_assertionarray_check) {
	ASSERT_THROW(value->add(value.get()), NotSupportedException);
	ASSERT_THROW(value->at(0), NotSupportedException);
	ASSERT_THROW(value->size(), NotSupportedException);
}
