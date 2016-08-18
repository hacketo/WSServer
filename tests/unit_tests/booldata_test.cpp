//
// Created by hacketo on 11/08/16.
//

#include "../../src/util/genericvalue.h"
#include "gtest/gtest.h"

class BoolValueFixture : public ::testing::Test {
protected:
	virtual void TearDown(){

	}
	virtual void SetUp(){

	}
public:
	BoolValueFixture() : Test(){
		value = GenericValue::u_ptr(new BoolValue());
	}

	virtual ~BoolValueFixture(){}

	GenericValue::u_ptr value;
};

TEST_F(BoolValueFixture, bool_notinitialized_check) {
	EXPECT_FALSE(value->is_initialized());
}

TEST_F(BoolValueFixture, bool_setvalue_check) {
	value->setBool(true);
	EXPECT_TRUE(value->is_initialized());
	EXPECT_TRUE(value->getBool());
}


TEST_F(BoolValueFixture, bool_initialized_check) {
	GenericValue::u_ptr boolValue = GenericValue::u_ptr(new BoolValue(true));
	EXPECT_TRUE(boolValue->is_initialized());
	EXPECT_TRUE(boolValue->getBool());
}

TEST_F(BoolValueFixture, bool_getvaluenotinitialized_check) {
	ASSERT_THROW(value->getBool(), ValueNotInitializedException);
}


TEST_F(BoolValueFixture, bool_isnotbool_check) {
	EXPECT_TRUE(value->isBool());
}

TEST_F(BoolValueFixture, bool_isnotfloat_check) {
	EXPECT_FALSE(value->isFloat());
}

TEST_F(BoolValueFixture, bool_isnotint_check) {
	EXPECT_FALSE(value->isInt());
}

TEST_F(BoolValueFixture, bool_isnotobject_check) {
	EXPECT_FALSE(value->isObject());
}

TEST_F(BoolValueFixture, bool_isarray_check) {
	EXPECT_FALSE(value->isArray());
}

TEST_F(BoolValueFixture, bool_isnotstring_check) {
	EXPECT_FALSE(value->isString());
}

TEST_F(BoolValueFixture, bool_isnotnull_check) {
	EXPECT_FALSE(value->isNull());
}

TEST_F(BoolValueFixture, bool_assertionint_check) {
	ASSERT_THROW(value->setInt(0), NotSupportedException);
	ASSERT_THROW(value->getInt(), NotSupportedException);
}

TEST_F(BoolValueFixture, bool_assertionfloat_check) {
	ASSERT_THROW(value->setFloat(0.1), NotSupportedException);
	ASSERT_THROW(value->getFloat(), NotSupportedException);
}

TEST_F(BoolValueFixture, bool_assertionstring_check) {
	ASSERT_THROW(value->setString(""), NotSupportedException);
	ASSERT_THROW(value->getString(), NotSupportedException);
}

TEST_F(BoolValueFixture, bool_assertionobject_check) {
	ASSERT_THROW(value->set("key", value.get()), NotSupportedException);
	ASSERT_THROW(value->get("key"), NotSupportedException);
}

TEST_F(BoolValueFixture, bool_assertionarray_check) {
	ASSERT_THROW(value->add(value.get()), NotSupportedException);
	ASSERT_THROW(value->at(0), NotSupportedException);
	ASSERT_THROW(value->size(), NotSupportedException);
}
