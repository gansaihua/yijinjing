#pragma once

#include <memory>
#include <stdexcept>
#include <string>

#define DECLARE_PTR(X) typedef std::shared_ptr<X> X##Ptr; /** define smart ptr */
#define FORWARD_DECLARE_PTR(X) \
    class X;                   \
    DECLARE_PTR(X) /** forward defile smart ptr */

namespace kungfu {

class yijinjing_error : public std::runtime_error {
public:
    explicit yijinjing_error(const std::string &message) : std::runtime_error(message) {}
};

class Event {
public:
    virtual ~Event() = default;
    virtual int64_t gen_time() const = 0;
    virtual int32_t msg_type() const = 0;
    virtual uint32_t source() const = 0;
    virtual uint32_t dest() const = 0;
    virtual uint32_t data_length() const = 0;
    virtual const char *data_as_bytes() const = 0;
    virtual const std::string data_as_string() const = 0;
    virtual const std::string to_string() const = 0;

    template <typename T>
    const T &data() const { return *reinterpret_cast<const T *>(data_address()); }

protected:
    virtual const void *data_address() const = 0;
};
DECLARE_PTR(Event)

}  // namespace kungfu
