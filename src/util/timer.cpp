//
// Created by hacketo on 28/05/15.
//

#include "timer.h"

#include <stdio.h>
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

void Timer::printTime(std::string msg){
    boost::format output("%.2f");
    output % (elapsed()/1000.0) ;
    std::cout << msg << " : " << output << "s " << std::endl;
}