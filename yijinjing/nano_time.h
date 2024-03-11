#pragma once

#include <string>

#include "tscns.h"

namespace kungfu {

class TimeUnit {
public:
    static const int64_t MILLISECONDS_PER_SECOND = 1000;
    static const int64_t MICROSECONDS_PER_MILLISECOND = 1000;
    static const int64_t NANOSECONDS_PER_MICROSECOND = 1000;

    static constexpr int64_t MICROSECONDS_PER_SECOND = MICROSECONDS_PER_MILLISECOND * MILLISECONDS_PER_SECOND;
    static constexpr int64_t NANOSECONDS_PER_MILLISECOND = NANOSECONDS_PER_MICROSECOND * MICROSECONDS_PER_MILLISECOND;
    static constexpr int64_t NANOSECONDS_PER_SECOND = NANOSECONDS_PER_MILLISECOND * MILLISECONDS_PER_SECOND;

    static const int64_t SECONDS_PER_MINUTE = 60;
    static const int64_t MINUTES_PER_HOUR = 60;
    static const int64_t HOURS_PER_DAY = 24;
    static constexpr int64_t SECONDS_PER_HOUR = SECONDS_PER_MINUTE * MINUTES_PER_HOUR;
    static constexpr int64_t SECONDS_PER_DAY = HOURS_PER_DAY * MINUTES_PER_HOUR * SECONDS_PER_MINUTE;

    static constexpr int64_t MILLISECONDS_PER_MINUTE = MILLISECONDS_PER_SECOND * SECONDS_PER_MINUTE;
    static constexpr int64_t NANOSECONDS_PER_MINUTE = NANOSECONDS_PER_SECOND * SECONDS_PER_MINUTE;
    static constexpr int64_t NANOSECONDS_PER_HOUR = NANOSECONDS_PER_SECOND * SECONDS_PER_HOUR;
    static constexpr int64_t NANOSECONDS_PER_DAY = NANOSECONDS_PER_HOUR * HOURS_PER_DAY;
};

class Time {
public:
    static void calibrate();

    static int64_t now_in_nano();

    // return %Y%m%d %H:%M:%S.123456789
    static std::string strftime(int64_t nanotime);

    // timestr should be format as one of
    // 1: %Y%m%d
    // 2: %Y%m%d %H:%M:%S
    // 3: %Y%m%d %H:%M:%S.123456789
    static int64_t strptime(const std::string &timestr);

    // 0=Sunday, 1=Monday, ..., 6=Saturday
    static int weekday(int64_t nanotime);

    // return %Y%m%d
    static int to_date(int64_t nanotime, int offset = 0);

    static int to_ms_since_midnight(int64_t nanotime);

private:
    Time();

    static Time &get_instance() {
        static Time t;
        return t;
    }

    TSCNS clock_;
};

}  // namespace kungfu
