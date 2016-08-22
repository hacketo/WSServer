//
// Created by hacketo on 28/05/15.
//

#include "util/timer.h"

#include <boost/format.hpp>



Timer::Timer() {
    reset();
}

void Timer::reset() {
    time1 = boost::posix_time::microsec_clock::local_time();
}
long Timer::elapsed() {
    boost::posix_time::time_duration diff = boost::posix_time::microsec_clock::local_time() - time1;
    return diff.total_milliseconds();
}

void Timer::printTime(const std::string& msg){
    boost::format output("%.10f");
    long e = elapsed();
    output % (e/1000.0) ;
    std::cout << msg << " : " << output << "s " << std::endl;
}

Timer Timer::timer = Timer();