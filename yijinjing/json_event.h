#pragma once

#include "common.h"
#include "json.hpp"

namespace kungfu {

class JsonEvent : public Event {
public:
    JsonEvent(const std::string &msg) : binding_(nlohmann::json::parse(msg)), msg_(msg){};

    int64_t gen_time() const override { return get_meta<int64_t>("gen_time", 0); }
    int32_t msg_type() const override { return get_meta<int32_t>("msg_type", 0); }
    uint32_t source() const override { return get_meta<uint32_t>("source", 0); }
    uint32_t dest() const override { return get_meta<uint32_t>("dest", 0); }
    uint32_t data_length() const override { return binding_.size(); }
    const char *data_as_bytes() const override { return msg_.c_str(); }
    const std::string data_as_string() const override { return binding_["data"].dump(); }
    const std::string to_string() const override { return msg_; }

protected:
    const void *data_address() const override { return &binding_["data"]; }

private:
    const nlohmann::json binding_;
    const std::string msg_;

    template <typename T>
    T get_meta(const std::string &name, T default_value) const {
        if (binding_.find(name) == binding_.end()) return default_value;
        return binding_[name].get<T>();
    }
};
DECLARE_PTR(JsonEvent)

}  // namespace kungfu
