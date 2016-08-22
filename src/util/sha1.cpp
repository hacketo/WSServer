//
// Created by hacketo on 08/08/16.
//

#include <openssl/sha.h>
#include "util/sha1.h"

std::string get_sha1(std::string sequence){
    unsigned char digest[SHA_DIGEST_LENGTH]; // == 20
    digest[20] = 0;
    SHA1(reinterpret_cast<const unsigned char*>(sequence.c_str()), sequence.size(), digest);
    std::string ret((char *)digest, 20);
    return ret;
}