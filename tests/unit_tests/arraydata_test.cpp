//
// Created by hacketo on 11/08/16.
//

#include "../../src/util/genericvalue.h"
#include "gtest/gtest.h"

class ArrayValueFixture : public ::testing::Test {
protected:
	virtual void TearDown(){

	}
	virtual void SetUp(){

	}
public:
	ArrayValueFixture() : Test(){
		value = GenericValue::u_ptr(new ArrayValue());
	}

	virtual ~ArrayValueFixture(){}

	GenericValue::u_ptr value;
};

TEST_F(ArrayValueFixture, array_initialized_check) {
	EXPECT_TRUE(value->is_initialized());
}

TEST_F(ArrayValueFixture, array_addvalue_check) {

	StringValue* v = new StringValue("bb");
	value->add(v);
	
	GenericValue* data = value->at(0);
	EXPECT_EQ(v, data);
	EXPECT_EQ(1, value->size());

	try {
		std::string v1 = v->getString();
		std::string v2 = data->getString();
		EXPECT_STREQ(v1.c_str(), v2.c_str());
	}
	catch(NotSupportedException const & err) {
		FAIL();
	}

	GenericValue* data2 = (*value)[0];

	try {
		std::string v1 = v->getString();
		std::string v2 = data2->getString();
		EXPECT_STREQ(v1.c_str(), v2.c_str());
	}
	catch(NotSupportedException const & err) {
		FAIL();
	}
}

TEST_F(ArrayValueFixture, array_initializedarray_check) {

	GenericValue::u_ptr array = GenericValue::u_ptr(new ArrayValue({new StringValue("a"), new StringValue("b")}));

	EXPECT_TRUE(array->is_initialized());


	GenericValue* data = array->at(0);
	EXPECT_STREQ("a", data->getString().c_str());

	GenericValue* data2 = array->at(1);
	EXPECT_STREQ("b", data2->getString().c_str());

	GenericValue* data3 = (*array)[0];

	try {
		EXPECT_STREQ("a", data3->getString().c_str());
	}
	catch(NotSupportedException const & err) {
		FAIL();
	}
}

TEST_F(ArrayValueFixture, array_circularreference_check) {
	ASSERT_THROW(value->add(value.get()), CircularException)
								<< "GenericValue does not support circular reference";
}


TEST_F(ArrayValueFixture, array_isarray_check) {
	EXPECT_TRUE(value->isArray());
}

TEST_F(ArrayValueFixture, array_isnotstring_check) {
	EXPECT_FALSE(value->isString());
}

TEST_F(ArrayValueFixture, array_isnotbool_check) {
	EXPECT_FALSE(value->isBool());
}

TEST_F(ArrayValueFixture, array_isnotnull_check) {
	EXPECT_FALSE(value->isNull());
}

TEST_F(ArrayValueFixture, array_isnotfloat_check) {
	EXPECT_FALSE(value->isFloat());
}

TEST_F(ArrayValueFixture, array_isnotobject_check) {
	EXPECT_FALSE(value->isObject());
}

TEST_F(ArrayValueFixture, array_isnotint_check) {
	EXPECT_FALSE(value->isInt());

}

TEST_F(ArrayValueFixture, array_assertionbool_check) {
	ASSERT_THROW(value->setBool(true), NotSupportedException);
	ASSERT_THROW(value->getBool(), NotSupportedException);
}

TEST_F(ArrayValueFixture, array_assertionint_check) {
	ASSERT_THROW(value->setInt(0), NotSupportedException);
	ASSERT_THROW(value->getInt(), NotSupportedException);
}

TEST_F(ArrayValueFixture, array_assertionfloat_check) {
	ASSERT_THROW(value->setFloat(0.1), NotSupportedException);
	ASSERT_THROW(value->getFloat(), NotSupportedException);
}

TEST_F(ArrayValueFixture, array_assertionobject_check) {
	ASSERT_THROW(value->set("key", value.get()), NotSupportedException);
	ASSERT_THROW(value->get("key"), NotSupportedException);
}

TEST_F(ArrayValueFixture, array_assertionstring_check) {
	ASSERT_THROW(value->setString(""), NotSupportedException);
	ASSERT_THROW(value->getString(), NotSupportedException);
}
