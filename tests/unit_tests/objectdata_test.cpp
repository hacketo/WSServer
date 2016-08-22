//
// Created by hacketo on 11/08/16.
//

#include "util/genericvalue.h"
#include "gtest/gtest.h"

class ObjectValueFixture : public ::testing::Test {
protected:
	virtual void TearDown(){

	}
	virtual void SetUp(){

	}
public:
	ObjectValueFixture() : Test(){
		value = GenericValue::u_ptr(new ObjectValue());
	}

	virtual ~ObjectValueFixture(){}

	GenericValue::u_ptr value;
};

TEST_F(ObjectValueFixture, object_initialized_check) {
	EXPECT_TRUE(value->is_initialized());
}

TEST_F(ObjectValueFixture, object_setvalue_check) {

	StringValue* p = new StringValue("bb");

	value->set("key", p);
	
	GenericValue* data = value->get("key");
	EXPECT_EQ(p, data);

	try {
		std::string v1 = p->getString();
		std::string v2 = data->getString();
		EXPECT_STREQ(v1.c_str(), v2.c_str());
	}
	catch(NotSupportedException const & err) {
		FAIL();
	}


	GenericValue* data2 = (*value)["key"];
	EXPECT_EQ(data, data2);

	try {
		std::string v1 = p->getString();
		std::string v2 = data2->getString();
		EXPECT_STREQ(v1.c_str(), v2.c_str());
	}
	catch(NotSupportedException const & err) {
		FAIL();
	}
}

TEST_F(ObjectValueFixture, object_circularreference_check) {
	ASSERT_THROW(value->set("k",value.get()), CircularException)
								<< "GenericValue does not support circular reference";
}

TEST_F(ObjectValueFixture, object_isnotobject_check) {
	EXPECT_TRUE(value->isObject());
}

TEST_F(ObjectValueFixture, object_isarray_check) {
	EXPECT_FALSE(value->isArray());
}

TEST_F(ObjectValueFixture, object_isnotstring_check) {
	EXPECT_FALSE(value->isString());
}

TEST_F(ObjectValueFixture, object_isnotbool_check) {
	EXPECT_FALSE(value->isBool());
}

TEST_F(ObjectValueFixture, object_isnotnull_check) {
	EXPECT_FALSE(value->isNull());
}

TEST_F(ObjectValueFixture, object_isnotfloat_check) {
	EXPECT_FALSE(value->isFloat());
}

TEST_F(ObjectValueFixture, object_isnotint_check) {
	EXPECT_FALSE(value->isInt());

}

TEST_F(ObjectValueFixture, object_assertionbool_check) {
	ASSERT_THROW(value->setBool(true), NotSupportedException);
	ASSERT_THROW(value->getBool(), NotSupportedException);
}

TEST_F(ObjectValueFixture, object_assertionint_check) {
	ASSERT_THROW(value->setInt(0), NotSupportedException);
	ASSERT_THROW(value->getInt(), NotSupportedException);
}

TEST_F(ObjectValueFixture, object_assertionfloat_check) {
	ASSERT_THROW(value->setFloat(0.1), NotSupportedException);
	ASSERT_THROW(value->getFloat(), NotSupportedException);
}

TEST_F(ObjectValueFixture, object_assertionstring_check) {
	ASSERT_THROW(value->setString(""), NotSupportedException);
	ASSERT_THROW(value->getString(), NotSupportedException);
}

TEST_F(ObjectValueFixture, object_assertionarray_check) {
	ASSERT_THROW(value->add(value.get()), NotSupportedException);
	ASSERT_THROW(value->at(0), NotSupportedException);
	ASSERT_THROW(value->size(), NotSupportedException);
}
