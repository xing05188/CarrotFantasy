#pragma once

#include <memory>
#include <mutex>

namespace carrot {
namespace core {
namespace state {

class GameFlowController;

class GameFlowProvider {
public:
    static void Set(const std::shared_ptr<GameFlowController>& controller);
    static std::shared_ptr<GameFlowController> Get();

private:
    static std::shared_ptr<GameFlowController>& InstanceRef();
    static std::mutex& Mutex();
};

}  // namespace state
}  // namespace core
}  // namespace carrot


