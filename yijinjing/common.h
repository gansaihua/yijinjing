#pragma once

#include <fmt/format.h>

#include <cassert>
#include <cstring>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "tscns.h"

#define DECLARE_PTR(X) typedef std::shared_ptr<X> X##Ptr; /** define smart ptr */
#define FORWARD_DECLARE_PTR(X) \
    class X;                   \
    DECLARE_PTR(X) /** forward defile smart ptr */

namespace yijinjing {

inline uint32_t murmurhash(const char *key, uint32_t len, uint32_t seed) {
    uint32_t c1 = 0xcc9e2d51;
    uint32_t c2 = 0x1b873593;
    uint32_t r1 = 15;
    uint32_t r2 = 13;
    uint32_t m = 5;
    uint32_t n = 0xe6546b64;
    uint32_t h = 0;
    uint32_t k = 0;
    uint8_t *d = (uint8_t *)key;  // 32 bit extract from `key'
    const uint32_t *chunks = NULL;
    const uint8_t *tail = NULL;  // tail - last 8 bytes
    int i = 0;
    int l = len / 4;  // chunk length

    h = seed;

    chunks = (const uint32_t *)(d + l * 4);  // body
    tail = (const uint8_t *)(d + l * 4);     // last 8 byte chunk of `key'

    // for each 4 byte chunk of `key'
    for (i = -l; i != 0; ++i) {
        // next 4 byte chunk of `key'
        k = chunks[i];

        // encode next 4 byte chunk of `key'
        k *= c1;
        k = (k << r1) | (k >> (32 - r1));
        k *= c2;

        // append to hash
        h ^= k;
        h = (h << r2) | (h >> (32 - r2));
        h = h * m + n;
    }

    k = 0;

    // remainder
    switch (len & 3) {  // `len % 4'
        case 3:
            k ^= (tail[2] << 16);
        case 2:
            k ^= (tail[1] << 8);

        case 1:
            k ^= tail[0];
            k *= c1;
            k = (k << r1) | (k >> (32 - r1));
            k *= c2;
            h ^= k;
    }

    h ^= len;

    h ^= (h >> 16);
    h *= 0x85ebca6b;
    h ^= (h >> 13);
    h *= 0xc2b2ae35;
    h ^= (h >> 16);

    return h;
}

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

inline int days_from_ymd(int ymd) {
    int y = ymd / 10000;
    int m = ymd % 10000 / 100;
    int d = ymd % 100;

    m = (m + 9) % 12;
    y -= m / 10;
    return 365 * y + y / 4 - y / 100 + y / 400 + (m * 306 + 5) / 10 + (d - 730426);
}

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

class Time {
public:
    // Should run calibration with some interval in the background:
    //     while(running) {
    //       Time::calibrate();
    //       std::this_thread::sleep_for(std::chrono::seconds(1));
    //     }
    static void calibrate() { get_instance().clock_.calibrate(); }

    static int64_t now_in_nano() { return get_instance().clock_.rdns(); }

    // return %Y%m%d %H:%M:%S.123456789
    static std::string strftime(int64_t nanotime) {
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
    static int64_t strptime(const std::string &timestr) {
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

private:
    Time() : clock_() { clock_.init(); }

    static Time &get_instance() {
        static Time t;
        return t;
    }

    TSCNS clock_;
};

class yijinjing_error : public std::runtime_error {
public:
    explicit yijinjing_error(const std::string &message) : std::runtime_error(message) {}
};

FORWARD_DECLARE_PTR(Location)

class Location {
public:
    Location(const std::string &path, uint32_t journal_page_size = std::numeric_limits<uint32_t>::max()) : path(path), journal_page_size(journal_page_size), uid(murmurhash(path.c_str(), path.size(), 6666)) {}

    const std::string path;
    const uint32_t uid;

    const uint32_t journal_page_size;  // 生成journal单个page的文件大小, in bytes
};

class Event {
public:
    virtual ~Event() = default;
    virtual int64_t gen_time() const = 0;
    virtual int32_t msg_type() const = 0;
    virtual uint32_t source() const = 0;
    virtual uint32_t dest() const = 0;
    virtual uint32_t data_length() const = 0;

    template <typename T>
    const T &data() const { return *reinterpret_cast<const T *>(data_address()); }

protected:
    virtual const void *data_address() const = 0;
};
DECLARE_PTR(Event)

}  // namespace yijinjing
