#pragma once

#include "GameState.h"

#include <memory>
#include <mutex>

namespace carrot {
namespace core {
namespace state {

class GameStateProvider {
public:
    static void Set(const std::shared_ptr<GameStateContext>& context) {
        std::lock_guard<std::mutex> guard(Mutex());
        InstanceRef() = context;
    }

    static std::shared_ptr<GameStateContext> Get() {
        std::lock_guard<std::mutex> guard(Mutex());
        std::shared_ptr<GameStateContext>& instance = InstanceRef();
        if (!instance) {
            instance = std::make_shared<GameStateContext>();
        }
        return instance;
    }

private:
    static std::shared_ptr<GameStateContext>& InstanceRef() {
        static std::shared_ptr<GameStateContext> instance;
        return instance;
    }

    static std::mutex& Mutex() {
        static std::mutex mutex;
        return mutex;
    }
};

}  // namespace state
}  // namespace core
}  // namespace carrot


