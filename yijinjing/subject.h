#pragma once

#include <eventpp/eventdispatcher.h>

#include <limits>
#include <unordered_map>

#include "reader.h"
#include "writer.h"

namespace yijinjing {

class Publisher {
public:
    Publisher(LocationPtr location) : location_(location) {
        writers_[0] = std::make_shared<Writer>(location, 0);
    }

    virtual ~Publisher() {}

    LocationPtr location() const { return location_; }

    WriterPtr get_writer(uint32_t dest_id) {
        auto it = writers_.find(dest_id);
        if (it == writers_.end()) throw yijinjing_error(fmt::format("No writer for: {:08x}", dest_id));
        return it->second;
    }

    // true=new created, false=existed
    bool require_write_to(uint32_t dest_id) {
        auto it = writers_.find(dest_id);
        if (it != writers_.end()) return false;
        writers_[dest_id] = std::make_shared<Writer>(location_, dest_id);
        return true;
    }

private:
    const LocationPtr location_;
    std::unordered_map<uint32_t, WriterPtr> writers_;
};

class Observer {
public:
    Observer() : reader_(new Reader()) {}

    virtual ~Observer() {}

    ReaderPtr get_reader() const { return reader_; }

    auto& get_dispatcher() const { return dispatcher_; }

    void require_read_from(LocationPtr location, uint32_t dest_id, int64_t from_time) {
        reader_->join(location, dest_id, from_time);
    }

    void subscribe(int msg_type, std::function<void(const EventPtr& e)> callback) {
        dispatcher_.appendListener(msg_type, callback);
    }

    virtual void run(bool live = false) {
        on_start();

        bool active = true;
        while (active) {
            while (reader_->data_available()) {
                dispatcher_.dispatch(reader_->current_frame());
                reader_->next();
            }
            if (!live) active = false;
        }

        on_stop();
    }

protected:
    virtual void on_start() {}
    virtual void on_stop() {}

private:
    ReaderPtr reader_;

    struct EventPolicies {
        static int getEvent(const EventPtr& e) { return e->msg_type(); }
    };
    eventpp::EventDispatcher<int, void(const EventPtr&), EventPolicies> dispatcher_;
};

class Subject : public Publisher, public Observer {
public:
    Subject(LocationPtr location) : Publisher(location) {}
};

}  // namespace yijinjing