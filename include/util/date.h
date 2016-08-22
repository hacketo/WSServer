//
// Created by hacketo on 20/08/16.
//

#ifndef SERVER_DATE_H
#define SERVER_DATE_H

#include <string>
#include <boost/date_time/posix_time/ptime.hpp>

using namespace boost::posix_time;

namespace date {

	std::string utc_from_now(u_int32_t offset = 0);
	std::string gmt( boost::posix_time::ptime& time );
};


#endif //SERVER_DATE_H
