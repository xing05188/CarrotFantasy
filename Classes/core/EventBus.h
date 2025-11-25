#pragma once

#include <algorithm>
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace carrot {
namespace core {

struct Event {
    virtual ~Event() {}
};

typedef std::string EventId;
typedef std::function<void(const Event&)> EventHandler;

class SimpleEventBus;

class Subscription {
public:
    virtual ~Subscription() {}
    virtual void Unsubscribe() = 0;
};

class SubscriptionImpl : public Subscription {
public:
    SubscriptionImpl(const std::weak_ptr<SimpleEventBus>& bus, const EventId& id, size_t token)
        : bus_(bus), id_(id), token_(token), active_(true) {}

    virtual ~SubscriptionImpl() { Unsubscribe(); }

    void Unsubscribe() override;

private:
    std::weak_ptr<SimpleEventBus> bus_;
    EventId id_;
    size_t token_;
    std::atomic_bool active_;
};

class IEventBus {
public:
    virtual ~IEventBus() {}
    virtual std::shared_ptr<Subscription> Subscribe(const EventId& id, EventHandler handler) = 0;
    virtual void Publish(const EventId& id, const Event& event) = 0;
    virtual void Clear() = 0;
};

class SimpleEventBus : public IEventBus, public std::enable_shared_from_this<SimpleEventBus> {
public:
    SimpleEventBus();
    std::shared_ptr<Subscription> Subscribe(const EventId& id, EventHandler handler) override;
    void Publish(const EventId& id, const Event& event) override;
    void Clear() override;

private:
    struct HandlerEntry {
        size_t token;
        EventHandler handler;
    };

    void UnsubscribeInternal(const EventId& id, size_t token);

    std::mutex mutex_;
    std::unordered_map<EventId, std::vector<HandlerEntry> > handlerMap_;
    std::atomic_size_t nextToken_;

    friend class SubscriptionImpl;
};

inline size_t NextToken(std::atomic_size_t& counter) {
    return counter.fetch_add(1, std::memory_order_relaxed) + 1;
}

inline std::shared_ptr<Subscription> SimpleEventBus::Subscribe(const EventId& id, EventHandler handler) {
    size_t token = NextToken(nextToken_);
    {
        std::lock_guard<std::mutex> guard(mutex_);
        handlerMap_[id].push_back(HandlerEntry{token, handler});
    }
    std::shared_ptr<SimpleEventBus> self = shared_from_this();
    std::weak_ptr<SimpleEventBus> weak = self;
    return std::shared_ptr<Subscription>(new SubscriptionImpl(weak, id, token));
}

inline void SimpleEventBus::Publish(const EventId& id, const Event& event) {
    std::vector<HandlerEntry> handlersCopy;
    {
        std::lock_guard<std::mutex> guard(mutex_);
        std::unordered_map<EventId, std::vector<HandlerEntry> >::iterator it = handlerMap_.find(id);
        if (it == handlerMap_.end()) {
            return;
        }
        handlersCopy = it->second;
    }
    for (size_t i = 0; i < handlersCopy.size(); ++i) {
        if (handlersCopy[i].handler) {
            handlersCopy[i].handler(event);
        }
    }
}

inline void SimpleEventBus::Clear() {
    std::lock_guard<std::mutex> guard(mutex_);
    handlerMap_.clear();
}

inline void SimpleEventBus::UnsubscribeInternal(const EventId& id, size_t token) {
    std::lock_guard<std::mutex> guard(mutex_);
    std::unordered_map<EventId, std::vector<HandlerEntry> >::iterator mapIt = handlerMap_.find(id);
    if (mapIt == handlerMap_.end()) {
        return;
    }
    std::vector<HandlerEntry>& handlers = mapIt->second;
    handlers.erase(std::remove_if(handlers.begin(), handlers.end(),
                                  [token](const HandlerEntry& entry) { return entry.token == token; }),
                   handlers.end());
    if (handlers.empty()) {
        handlerMap_.erase(mapIt);
    }
}

inline void SubscriptionImpl::Unsubscribe() {
    bool expected = true;
    if (!active_.compare_exchange_strong(expected, false)) {
        return;
    }
    std::shared_ptr<SimpleEventBus> bus = bus_.lock();
    if (bus) {
        bus->UnsubscribeInternal(id_, token_);
    }
}

inline SimpleEventBus::SimpleEventBus() : nextToken_(0) {}

}  // namespace core
}  // namespace carrot

