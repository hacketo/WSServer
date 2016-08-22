//
// Created by hacketo on 11/08/16.
//

#include <memory>
#include <debug.h>
#include "util/genericvalue.h"


GenericValue::GenericValue() : initialized(false){}

GenericValue::~GenericValue() {}

void GenericValue::set_initialized(){
	initialized = true;
}
bool GenericValue::is_initialized() const{
	return initialized;
}

void GenericValue::check_circular_reference(void* something){
	if (something == static_cast<void *>(this)) {
		throw CircularException();
	}
}
void GenericValue::check_circular_reference(initializer list){
	for (initializer::iterator it = list.begin() ; it != list.end() ;++it){
		if (*it == static_cast<void *>(this)) {
			throw CircularException();
		}
	}

}

inline std::string GenericValue::getReadableType() const{
	return "GenericValue";
}


GenericValue* GenericValue::parse(rapidjson::Value *value) {

	GenericValue* packetValue;

	if (value->IsString()) {
		packetValue = new StringValue(value->GetString());
	} else if (value->IsNull()) {
		packetValue = new NullValue();
	} else if (value->IsInt()) {
		packetValue = new IntValue(value->GetInt());
	} else if (value->IsFloat()) {
		packetValue = new FloatValue(value->GetFloat());
	} else if (value->IsBool()) {
		packetValue = new BoolValue(value->GetBool());
	} else if (value->IsObject()) {
		packetValue = new ObjectValue();
		for (rapidjson::Value::MemberIterator itr = value->MemberBegin();
			 itr != value->MemberEnd(); ++itr) {
			packetValue->set(itr->name.GetString(), parse(&itr->value));
		}
	} else if (value->IsArray()) {
		packetValue = new ArrayValue();
		for (rapidjson::Value::ValueIterator itr = value->Begin(); itr != value->End(); ++itr) {
			packetValue->add(parse(itr));
		}
	} else {
		packetValue = new GenericValue();
	}

	return packetValue;
}


//<editor-fold desc="Object">

GenericValue::obj_const_iterator GenericValue::begin_object() {THROW_NOT_SUPPORTED()}

GenericValue::obj_const_iterator GenericValue::end_object() {THROW_NOT_SUPPORTED()}

bool GenericValue::isObject() const { return false; }

void GenericValue::set(std::string key, GenericValue* value) {THROW_NOT_SUPPORTED()}

GenericValue* GenericValue::get(std::string key) {THROW_NOT_SUPPORTED()}

std::vector<std::string> GenericValue::getKeys() const {THROW_NOT_SUPPORTED()}

GenericValue* GenericValue::operator[](std::string key) {THROW_NOT_SUPPORTED()}

//</editor-fold>

//<editor-fold desc="Array">

GenericValue::const_iterator GenericValue::begin_array() {THROW_NOT_SUPPORTED()}

GenericValue::const_iterator GenericValue::end_array() {THROW_NOT_SUPPORTED()}

bool GenericValue::isArray() const { return false; }

void GenericValue::add(GenericValue* value) {THROW_NOT_SUPPORTED()}

GenericValue* GenericValue::at(unsigned int index) {THROW_NOT_SUPPORTED()}

unsigned long GenericValue::size() const {THROW_NOT_SUPPORTED()}

GenericValue* GenericValue::operator[](unsigned int key) {THROW_NOT_SUPPORTED()}

//</editor-fold>

//<editor-fold desc="Bool">

bool GenericValue::isBool() const { return false; }

void GenericValue::setBool(bool value) {THROW_NOT_SUPPORTED()}

bool GenericValue::getBool() const {THROW_NOT_SUPPORTED()}

//</editor-fold>

//<editor-fold desc="String">

bool GenericValue::isString() const { return false; }

void GenericValue::setString(std::string value) {THROW_NOT_SUPPORTED()}

std::string GenericValue::getString() const {THROW_NOT_SUPPORTED()}

//</editor-fold>

//<editor-fold desc="Int">

bool GenericValue::isInt() const { return false; }

void GenericValue::setInt(int value) {THROW_NOT_SUPPORTED()}

int GenericValue::getInt() const {THROW_NOT_SUPPORTED()}

//</editor-fold>

//<editor-fold desc="Float">

bool GenericValue::isFloat() const { return false; }

void GenericValue::setFloat(float value) {THROW_NOT_SUPPORTED()}

float GenericValue::getFloat() const {THROW_NOT_SUPPORTED()}

//</editor-fold>

//<editor-fold desc="Null">

bool GenericValue::isNull() const { return false; }

#if WS_DEBUG
void GenericValue::debugValue(GenericValue *value, int baseDepth) {

	std::string tabs = debug::getTabs(baseDepth);

	if (value->isString()){
		debug::print(tabs,"String : ", value->getString());
	}

	else if (value->isInt()){
		debug::print(tabs,"Int : ", value->getInt());
	}
	else if (value->isNull()){
		debug::print(tabs,"Null");
	}

	else if (value->isBool()){
		debug::print(tabs,"Bool : ", value->getBool() ? "true" : "false");
	}
	else if (value->isFloat()){
		debug::print(tabs,"Float : ", value->getFloat());
	}

	else if (value->isArray()){
		debug::print(tabs,"Array : ");
		for (ArrayValue::const_iterator itr = value->begin_array(); itr != value->end_array(); ++itr){
			debugValue(itr->get(), baseDepth+1);
		}
	}

	else if (value->isObject()){
		debug::print(tabs,"Object : ");
		baseDepth++;

		std::vector<std::string> keys = value->getKeys();
		for (std::vector<std::string>::iterator itr = keys.begin();
			 itr != keys.end(); ++itr){
			debug::print(debug::getTabs(baseDepth),*itr);
			debugValue(value->get(*itr), baseDepth);
		}
	}

}
#endif

//</editor-fold>

/** Implémentations */

//<editor-fold desc="ObjectValue">

ObjectValue::ObjectValue() : GenericValue(){
	set_initialized();
}

ObjectValue::~ObjectValue() {
//	if (keys.size() > 0) {
//		for (obj_const_iterator itr = begin_object(); itr != end_object(); ++itr) {
//			delete itr->second;
//		}
//		keys.clear();
//		objects.clear();
//	}
	keys.clear();
	objects.clear();
}

bool ObjectValue::isObject() const { return true; }

void ObjectValue::set(std::string key, GenericValue* value) {
	check_circular_reference(value);
	if (objects.count(key) == 0) {
		keys.push_back(key);
	}
	objects[key] = u_ptr(value);
}

GenericValue* ObjectValue::get(std::string key) {
	if (objects.count(key) > 0) {
		return objects[key].get();
	}
	throw std::out_of_range(key);

}

std::vector<std::string> ObjectValue::getKeys() const {
	return std::vector<std::string>(keys);
}

GenericValue* ObjectValue::operator[](const std::string key) {
	if (objects.count(key) > 0) {
		return objects[key].get();
	}
	throw std::out_of_range(key);
}

GenericValue::obj_const_iterator ObjectValue::begin_object() {
	return objects.begin();
}

GenericValue::obj_const_iterator ObjectValue::end_object() {
	return objects.end();
}

inline std::string ObjectValue::getReadableType() const{
	return "ObjectValue";
}

//</editor-fold>

//<editor-fold desc="ArrayValue">

ArrayValue::ArrayValue() : GenericValue(){
	set_initialized();
}

ArrayValue::ArrayValue(initializer list){
	this->set_initialized();
	check_circular_reference(list);

	for(initializer::iterator it = list.begin(); it != list.end(); ++it){
		add(*it);
	}
}

ArrayValue::~ArrayValue() {
//	if (size() > 0) {
//		for (std::vector<s_ptr>::packet_iterator itr = array.begin(); itr != array.end(); ++itr) {
//			delete (GenericValue *) *itr;
//		}
//		array.clear();
//	}
	array.clear();
}


bool ArrayValue::isArray() const { return true; }

void ArrayValue::add(GenericValue* value) {
	check_circular_reference(value);
	array.push_back(u_ptr(value));
}

GenericValue* ArrayValue::at(unsigned int index) {
	if (array.size() > index && index >= 0) {
		return array[index].get();
	}
	throw std::out_of_range(std::to_string(index));
}

unsigned long ArrayValue::size() const { return array.size(); }

GenericValue* ArrayValue::operator[](unsigned int index) {
	if (array.size() > index && index >= 0) {
		return array[index].get();
	}
	throw std::out_of_range(std::to_string(index));
}

GenericValue::const_iterator ArrayValue::begin_array() { return array.begin(); }

GenericValue::const_iterator ArrayValue::end_array() { return array.end(); }

inline std::string ArrayValue::getReadableType() const{
	return "ArrayValue";
}

//</editor-fold>

//<editor-fold desc="BoolValue">

BoolValue::BoolValue() : GenericValue(){}

BoolValue::BoolValue(bool data) : GenericValue() , boolValue(data) {
	set_initialized();
}

bool BoolValue::isBool() const {
	return true;
}

bool BoolValue::getBool() const { CHECK_INITIALIZED(); return boolValue; }

void BoolValue::setBool(bool value) { boolValue = value; set_initialized();}

inline std::string BoolValue::getReadableType() const {
	return "BoolValue";
}

//</editor-fold>

//<editor-fold desc="StringValue">

StringValue::StringValue() : GenericValue(){}
StringValue::StringValue(std::string data) : GenericValue() ,  stringValue(data) {
	set_initialized();
}

bool StringValue::isString() const { return true; }

std::string StringValue::getString() const {CHECK_INITIALIZED(); return stringValue; }

void StringValue::setString(std::string value) { stringValue = value; set_initialized();}

inline std::string StringValue::getReadableType() const{
	return "StringValue";
}

//</editor-fold>

//<editor-fold desc="IntValue">

IntValue::IntValue() : GenericValue(){}
IntValue::IntValue(int data) : GenericValue() ,  intNumber(data) {
	set_initialized();
}

bool IntValue::isInt() const { return true; }

int IntValue::getInt() const {CHECK_INITIALIZED(); return intNumber; }

void IntValue::setInt(int value) { intNumber = value; set_initialized();}

inline std::string IntValue::getReadableType() const{
	return "IntValue";
}

//</editor-fold>

//<editor-fold desc="FloatValue">

FloatValue::FloatValue() : GenericValue(){}
FloatValue::FloatValue(float data) : GenericValue() ,  floatNumber(data) {
	set_initialized();
}

bool FloatValue::isFloat() const { return true; }

float FloatValue::getFloat() const {CHECK_INITIALIZED(); return floatNumber; }

void FloatValue::setFloat(float value) { floatNumber = value; set_initialized();}

inline std::string FloatValue::getReadableType() const{
	return "FloatValue";
}

//</editor-fold>

//<editor-fold desc="NullValue">

NullValue::NullValue() : GenericValue(){
	set_initialized();
}

bool NullValue::isNull() const { return true; }

inline std::string NullValue::getReadableType() const{
	return "NullValue";
}

//</editor-fold>