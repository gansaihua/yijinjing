#include "nano_time.h"

#include <fmt/format.h>

namespace kungfu {

// use Zeller formula
// 0=Sunday, 1=Monday, ..., 6=Saturday
inline int weekday_from_ymd(int ymd) {
    int c = ymd / 1000000;
    int y = ymd / 10000;
    int m = ymd % 10000 / 100;
    int d = ymd % 100;

    if (m == 1 || m == 2) {
        m += 12;
        --y;
    }

    y %= 100;

    int w = (c / 4 - 2 * c + y + y / 4 + 13 * (m + 1) / 5 + d - 1) % 7;
    return w >= 0 ? w : w + 7;
}

// return 0 for 20000101
inline int days_from_ymd(int ymd) {
    int y = ymd / 10000;
    int m = ymd % 10000 / 100;
    int d = ymd % 100;

    m = (m + 9) % 12;
    y -= m / 10;
    return 365 * y + y / 4 - y / 100 + y / 400 + (m * 306 + 5) / 10 + (d - 730426);
}

// return 20000101 for 0
inline int ymd_from_days(int g) {
    g += 730425;  // convert offset from 2000.01.01 to 0003.01.01

    int y = (10000 * static_cast<int64_t>(g) + 14780) / 3652425;
    int ddd = g - (365 * y + y / 4 - y / 100 + y / 400);
    if (ddd < 0) {
        --y;
        ddd = g - (365 * y + y / 4 - y / 100 + y / 400);
    }
    int mi = (100 * ddd + 52) / 3060;
    int mm = (mi + 2) % 12 + 1;
    y += (mi + 2) / 12;
    int dd = ddd - (mi * 306 + 5) / 10 + 1;
    return y * 10000 + mm * 100 + dd;
}

Time::Time() : clock_() { clock_.init(); }

void Time::calibrate() { get_instance().clock_.calibrate(); }

int64_t Time::now_in_nano() { return get_instance().clock_.rdns(); }

// return %Y%m%d %H:%M:%S.123456789
std::string Time::strftime(int64_t nanotime) {
    int ns = nanotime % TimeUnit::NANOSECONDS_PER_SECOND;
    int64_t remaining = (nanotime - ns) / TimeUnit::NANOSECONDS_PER_SECOND;
    int64_t t = remaining % TimeUnit::SECONDS_PER_DAY;
    remaining = (remaining - t) / TimeUnit::SECONDS_PER_DAY;

    int h = t / 3600 + 8;  // +8H convert to local
    if (h >= 24) {
        h -= 24;
        ++remaining;
    }
    int ymd = ymd_from_days(remaining - 10957);
    int m = (t % 3600) / 60;
    int s = t % 60;

    return fmt::format("{} {:02d}:{:02d}:{:02d}.{:09d}", ymd, h, m, s, ns);
}

// timestr should be format as one of
// 1: %Y%m%d
// 2: %Y%m%d %H:%M:%S
// 3: %Y%m%d %H:%M:%S.123456789
int64_t Time::strptime(const std::string &timestr) {
    int ymd = std::stoi(timestr);
    int m = 0, s = 0, ns = 0, h = -8;  // -8H convert to UTC
    if (timestr.size() > 17) ns = std::atoi(timestr.c_str() + 18);
    if (timestr.size() > 8) {
        h += std::atoi(timestr.c_str() + 9);
        m = std::atoi(timestr.c_str() + 12);
        s = std::atoi(timestr.c_str() + 15);
    }
    return static_cast<int64_t>(days_from_ymd(ymd) + 10957) * TimeUnit::NANOSECONDS_PER_DAY + h * TimeUnit::NANOSECONDS_PER_HOUR + m * TimeUnit::NANOSECONDS_PER_MINUTE + s * TimeUnit::NANOSECONDS_PER_SECOND + ns;
}

int Time::weekday(int64_t nanotime) {
    return weekday_from_ymd(Time::to_date(nanotime));
}

int Time::to_date(int64_t nanotime, int offset) {
    return ymd_from_days((nanotime + 8 * TimeUnit::NANOSECONDS_PER_HOUR) / TimeUnit::NANOSECONDS_PER_DAY - 10957 + offset);  // +8H to local date
}

int Time::to_ms_since_midnight(int64_t nanotime) {
    return (nanotime + 8 * TimeUnit::NANOSECONDS_PER_HOUR) % TimeUnit::NANOSECONDS_PER_DAY / TimeUnit::NANOSECONDS_PER_MILLISECOND;  // +8H to local hour
}

}  // namespace kungfu
