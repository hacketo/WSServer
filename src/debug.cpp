//
// Created by hacketo on 08/08/16.
//

#include <iostream>
#include <fstream>
#include <cstdarg>
#include "debug.h"

namespace debug {
    bool DEBUG = true;

    std::string filePath = "/home/hacketo/Workspace/C++/WS_Server/logs/";




    void log_data(std::string name, const unsigned char *data, size_t len) {

        std::ofstream myfile;
        myfile.open(filePath + name + ".txt", std::ios::out | std::ios::app);
        if (myfile.is_open()) {
            char buffer[len * 2 + 1];
            //sprintf(buffer,"%02X ",data);
            myfile.write(buffer, len * 2 + 1);
        }
        myfile.close();
    }

    void print(errors::error &e){
        std::stringstream ss;
        if (e.master.length() > 0){
            ss << "[" << e.master<< "] : ";
        }
        ss << "(" << e.code << ") : " << e.msg;
        std::cout << ss.str() << std::endl ;
    }

    void debugField(GenericValue* value , int baseDepth){

		std::string tabs = getTabs(baseDepth);

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
                debugField(itr->get(), baseDepth+1);
            }
        }

        else if (value->isObject()){
            debug::print(tabs,"Object : ");
			baseDepth++;

			std::vector<std::string> keys = value->getKeys();
            for (std::vector<std::string>::iterator itr = keys.begin();
                 itr != keys.end(); ++itr){
                debug::print(getTabs(baseDepth),*itr);
                debugField(value->get(*itr), baseDepth);
            }
        }


    }

    std::string getTypeOfField(rapidjson::Value::ConstMemberIterator &it){
        return kTypeNames[it->value.GetType()];
    }

    std::string getTabs(int depth){
        std::stringstream ss;
        while(depth > 0){
            ss << "\t";
			depth--;
        }
        return ss.str();
    }
}


