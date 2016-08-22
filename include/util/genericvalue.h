//
// Created by hacketo on 11/08/16.
//

#ifndef SERVER_GenericValue_H
#define SERVER_GenericValue_H

#include <string>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "exception.h"

#define THROW_NOT_SUPPORTED() { throw NotSupportedException(); }
#define CHECK_INITIALIZED() { if (!this->is_initialized()) { throw ValueNotInitializedException(this->getReadableType()); } }


//<editor-fold desc="GenericValue">

class GenericValue {

public:

	typedef std::unique_ptr<GenericValue> u_ptr;
	typedef boost::shared_ptr<GenericValue> shared_ptr;
	typedef std::initializer_list<GenericValue*> initializer;


	virtual ~GenericValue();

	bool is_initialized() const;

	virtual inline std::string getReadableType() const;

	/** Objects */

	typedef std::map<std::string, GenericValue::u_ptr>::const_iterator obj_const_iterator;

	virtual obj_const_iterator begin_object();

	virtual obj_const_iterator end_object();

	virtual bool isObject() const;

	virtual void set(const std::string key, GenericValue* value);

	virtual GenericValue* get(const std::string key);

	virtual std::vector<std::string> getKeys() const;

	virtual GenericValue* operator[](const std::string key);


	/** Array */

	typedef std::vector<GenericValue::u_ptr>::const_iterator const_iterator;

	virtual const_iterator begin_array();

	virtual const_iterator end_array();

	virtual bool isArray() const;

	virtual void add(GenericValue* value);

	virtual GenericValue* at(unsigned int index);

	virtual unsigned long size() const;

	virtual GenericValue* operator[](unsigned int key);

	/** Bool */

	virtual bool isBool() const;

	virtual void setBool(bool value);

	virtual bool getBool() const;

	/** String */

	virtual void setString(const std::string value);

	virtual bool isString() const;

	virtual std::string getString() const;

	/** Int */

	virtual void setInt(int value);

	virtual bool isInt() const;

	virtual int getInt() const;

	/** Float */

	virtual bool isFloat() const;

	virtual void setFloat(float value);

	virtual float getFloat() const;

	/** Null */

	virtual bool isNull() const;

	/** Parsing */

	static GenericValue* parse(rapidjson::Value *value);

	typedef rapidjson::Writer<rapidjson::StringBuffer> Writer;

	static std::string toJSON(GenericValue* value) {
		rapidjson::StringBuffer sb;
		rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
		GenericValue::write(&writer, value);
		std::string ret = sb.GetString();
		return ret;

	}

#if WS_DEBUG
	static void debugValue(GenericValue* value , int baseDepth);
#endif

	template<typename Writer>
	static void write(Writer *writer, GenericValue* value) {
		if (value->isString()) {
			writer->String(value->getString().c_str());
		} else if (value->isNull()) {
			writer->Null();
		} else if (value->isInt()) {
			writer->Int(value->getInt());
		} else if (value->isFloat()) {
			writer->Double(value->getFloat());
		} else if (value->isBool()) {
			writer->Bool(value->getBool());
		} else if (value->isObject()) {
			writer->StartObject();
			for (obj_const_iterator itr = value->begin_object();
				 itr != value->end_object(); ++itr) {
				writer->Key(itr->first.c_str());
				GenericValue::write(writer, itr->second.get());
			}
			writer->EndObject();
		} else if (value->isArray()) {
			writer->StartArray();
			for (const_iterator itr = value->begin_array(); itr != value->end_array(); ++itr) {
				GenericValue::write(writer, itr->get());
			}
			writer->EndArray();
		}
	}

protected:
	GenericValue();
	void set_initialized();
	void check_circular_reference(void* something);
	void check_circular_reference(initializer list);

private:
	/**
	 * True si la valeur à été initialisée
	 */
	bool initialized;


};

//</editor-fold>

//<editor-fold desc="BoolValue">

class BoolValue : public GenericValue {

public:
	BoolValue();
	BoolValue(bool data);
	bool getBool() const;

	void setBool(bool value);

	bool isBool() const;

	inline std::string getReadableType() const;
private:
	bool boolValue;
};

//</editor-fold>

//<editor-fold desc="StringValue">

class StringValue : public GenericValue {
public:
	StringValue();
	StringValue(const char * data);
	StringValue(const std::string data);

	std::string getString() const;

	void setString(const std::string value);

	bool isString() const;

	inline std::string getReadableType() const;

private:
	std::string stringValue;
};

//</editor-fold>

//<editor-fold desc="IntValue">

class IntValue : public GenericValue {
public:
	IntValue();
	IntValue(int data);

	int getInt() const;

	void setInt(int value);

	bool isInt() const;

	inline std::string getReadableType() const;

private:
	int intNumber;
};

//</editor-fold>

//<editor-fold desc="FloatValue">

class FloatValue : public GenericValue {
public:
	FloatValue();
	FloatValue(float data);

	float getFloat() const;

	void setFloat(float value);

	bool isFloat() const;

	inline std::string getReadableType() const;

private:

	float floatNumber;
};

//</editor-fold>

//<editor-fold desc="NullValue">

class NullValue : public GenericValue {
public:
	NullValue();
	bool isNull() const;
	inline std::string getReadableType() const;

private:

};

//</editor-fold>

//<editor-fold desc="ArrayValue">

class ArrayValue : public GenericValue {
public:
	ArrayValue();
	ArrayValue(initializer list);
	~ArrayValue();

	void add(GenericValue* value);

	GenericValue* at(unsigned int index);

	unsigned long size() const;

	GenericValue* operator[](unsigned int key);

	const_iterator begin_array();

	const_iterator end_array();

	bool isArray() const;

	inline std::string getReadableType() const;

private:
	std::vector<GenericValue::u_ptr> array;
};

//</editor-fold>

//<editor-fold desc="ObjectValue">

class ObjectValue : public GenericValue {
public:
	ObjectValue();
	~ObjectValue();

	void set(const std::string key, GenericValue* value);

	GenericValue* get(const std::string key);

	std::vector<std::string> getKeys() const;

	GenericValue* operator[](const std::string key);

	obj_const_iterator begin_object();

	obj_const_iterator end_object();

	bool isObject() const;

	inline std::string getReadableType() const;

private:
	std::map<std::string, GenericValue::u_ptr> objects;
	std::vector<std::string> keys;
};

//</editor-fold>

#endif //SERVER_GenericValue_H