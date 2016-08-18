//
// Created by hacketo on 11/08/16.
//

#include "../../src/util/genericvalue.h"
#include "gtest/gtest.h"

class StringValueFixture : public ::testing::Test {
protected:
	virtual void TearDown(){

	}
	virtual void SetUp(){

	}
public:
	StringValueFixture() : Test(){
		value = GenericValue::u_ptr(new StringValue());
	}

	virtual ~StringValueFixture(){}

	GenericValue::u_ptr value;
};

TEST_F(StringValueFixture, string_notinitialized_check) {
	EXPECT_FALSE(value->is_initialized());
}

TEST_F(StringValueFixture, string_setvalue_check) {
	std::string v = "testValue";
	value->setString(v);
	EXPECT_TRUE(value->is_initialized());
	EXPECT_STREQ(v.c_str(), value->getString().c_str());
}


TEST_F(StringValueFixture, string_initialized_check) {
	StringValue::u_ptr stringValue = GenericValue::u_ptr(new StringValue("b"));
	EXPECT_TRUE(stringValue->is_initialized());
	EXPECT_STREQ(stringValue->getString().c_str(), "b");
}

TEST_F(StringValueFixture, string_getvaluenotinitialized_check) {
	ASSERT_THROW(value->getString(), ValueNotInitializedException);
}

TEST_F(StringValueFixture, string_isstring_check) {
	EXPECT_TRUE(value->isString());
}

TEST_F(StringValueFixture, string_isnotbool_check) {
	EXPECT_FALSE(value->isBool());
}

TEST_F(StringValueFixture, string_isnotnull_check) {
	EXPECT_FALSE(value->isNull());
}

TEST_F(StringValueFixture, string_isnotarray_check) {
	EXPECT_FALSE(value->isArray());
}

TEST_F(StringValueFixture, string_isnotfloat_check) {
	EXPECT_FALSE(value->isFloat());
}

TEST_F(StringValueFixture, string_isnotobject_check) {
	EXPECT_FALSE(value->isObject());
}

TEST_F(StringValueFixture, string_isnotint_check) {
	EXPECT_FALSE(value->isInt());

}


TEST_F(StringValueFixture, string_assertionbool_check) {
	ASSERT_THROW(value->setBool(true), NotSupportedException);
	ASSERT_THROW(value->getBool(), NotSupportedException);
}

TEST_F(StringValueFixture, string_assertionint_check) {
	ASSERT_THROW(value->setInt(0), NotSupportedException);
	ASSERT_THROW(value->getInt(), NotSupportedException);
}

TEST_F(StringValueFixture, string_assertionfloat_check) {
	ASSERT_THROW(value->setFloat(0.1), NotSupportedException);
	ASSERT_THROW(value->getFloat(), NotSupportedException);
}

TEST_F(StringValueFixture, string_assertionobject_check) {
	ASSERT_THROW(value->set("key", value.get()), NotSupportedException);
	ASSERT_THROW(value->get("key"), NotSupportedException);
}

TEST_F(StringValueFixture, string_assertionarray_check) {
	ASSERT_THROW(value->add(value.get()), NotSupportedException);
	ASSERT_THROW(value->at(0), NotSupportedException);
	ASSERT_THROW(value->size(), NotSupportedException);
}