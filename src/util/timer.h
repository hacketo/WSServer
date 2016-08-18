//
// Created by hacketo on 28/05/15.
//

#ifndef WS_SERVER_TIMER_H
#define WS_SERVER_TIMER_H

#include <boost/date_time/posix_time/posix_time.hpp>


class Timer{
public:
    Timer();
    void reset();
    long elapsed();
    void printTime(std::string msg);

private:
    boost::posix_time::ptime time1;
};

#endif //WS_SERVER_TIMER_H
