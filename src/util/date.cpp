//
// Created by hacketo on 20/08/16.
//

#include <boost/date_time/time_clock.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "util/date.h"

using namespace boost::posix_time;

std::string date::utc_from_now(u_int32_t offset) {
	using boost::posix_time::second_clock;
	using boost::posix_time::to_simple_string;
	using boost::gregorian::day_clock;

	ptime next = ptime(day_clock::universal_day(), second_clock::universal_time().time_of_day() + seconds(offset));
	time_facet* lf = new time_facet("%a, %d %b %Y %H:%M:%S UTC");

	std::stringstream ss;
	ss.imbue(std::locale(ss.getloc(), lf));
	ss << next;

	//Wed, 13 Jan 2021 22:23:01 GMT
	return ss.str();
}

std::string date::gmt(ptime& time) {
	time_facet* lf = new time_facet("%a, %d %b %Y %H:%M:%S GMT");

	std::stringstream ss;
	ss.imbue(std::locale(ss.getloc(), lf));
	ss << time;

	//Wed, 13 Jan 2021 22:23:01 GMT
	return ss.str();
}
