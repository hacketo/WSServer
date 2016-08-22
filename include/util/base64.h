//
// Created by hacketo on 29/05/15.
//


#ifndef SERVER_BASE64_H
#define SERVER_BASE64_H

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
#include <boost/archive/iterators/remove_whitespace.hpp>
#include <algorithm>
#include <string>

using namespace boost::archive::iterators;

typedef transform_width< binary_from_base64<remove_whitespace<std::string::const_iterator> >, 8, 6 > it_binary_t;
typedef insert_linebreaks<base64_from_binary<transform_width<std::string::const_iterator,6,8> >, 72 > it_base64_t;


std::string  base64_encode(std::string text);

std::string  base64_decode(std::string base64);

#endif //SERVER_BASE64_H
