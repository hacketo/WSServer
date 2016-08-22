//
// Created by hacketo on 08/08/16.
//

#include <iostream>
#include <fstream>
#include <cstdarg>
#include "debug.h"

namespace debug {

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

    std::string getTabs(int depth){
        std::stringstream ss;
        while(depth > 0){
            ss << "\t";
			depth--;
        }
        return ss.str();
    }
}


