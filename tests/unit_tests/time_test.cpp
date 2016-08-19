//
// Created by hacketo on 19/08/16.
//

#include <gtest/gtest.h>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

TEST(time_test, testinit){

	boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();

	if (!start_time.is_not_a_date_time()){
		SUCCEED();
	}
	else{
		FAIL();
	}

}