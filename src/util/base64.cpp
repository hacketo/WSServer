//
// Created by hacketo on 08/08/16.
//

#include "base64.h"

std::string  base64_encode(const std::string text){
    unsigned long writePaddChars = (3-text.length()%3)%3;
    std::string base64(it_base64_t(text.begin()),it_base64_t(text.end()));
    base64.append(writePaddChars,'=');

    return base64;
}


std::string  base64_decode(std::string base64){
    long paddChars = std::count(base64.begin(), base64.end(), '=');
    std::replace(base64.begin(),base64.end(),'=','A'); // replace '=' by base64 encoding of '\0'
    std::string result(it_binary_t(base64.begin()), it_binary_t(base64.end())); // decode
    result.erase(result.end()-paddChars,result.end());  // erase padding '\0' characters

    return result;
}