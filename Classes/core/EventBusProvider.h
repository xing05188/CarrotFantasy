#pragma once

#include "EventBus.h"

#include <memory>
#include <mutex>

namespace carrot {
namespace core {

class EventBusProvider {
public:
    static void Set(const std::shared_ptr<IEventBus>& bus) {
        std::lock_guard<std::mutex> guard(Mutex());
        InstanceRef() = bus;
    }

    static std::shared_ptr<IEventBus> Get() {
        std::lock_guard<std::mutex> guard(Mutex());
        std::shared_ptr<IEventBus>& instance = InstanceRef();
        if (!instance) {
            instance = CreateDefaultUnlocked();
        }
        return instance;
    }

private:
    static std::shared_ptr<IEventBus>& InstanceRef() {
        static std::shared_ptr<IEventBus> instance;
        return instance;
    }

    static std::mutex& Mutex() {
        static std::mutex mutex;
        return mutex;
    }

    static std::shared_ptr<IEventBus> CreateDefaultUnlocked() {
        return std::shared_ptr<IEventBus>(new SimpleEventBus());
    }
};

}  // namespace core
}  // namespace carrot

