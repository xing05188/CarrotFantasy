#include "GameFlowProvider.h"

#include "GameFlowController.h"

namespace carrot {
namespace core {
namespace state {

void GameFlowProvider::Set(const std::shared_ptr<GameFlowController>& controller) {
    std::lock_guard<std::mutex> guard(Mutex());
    InstanceRef() = controller;
}

std::shared_ptr<GameFlowController> GameFlowProvider::Get() {
    std::lock_guard<std::mutex> guard(Mutex());
    return InstanceRef();
}

std::shared_ptr<GameFlowController>& GameFlowProvider::InstanceRef() {
    static std::shared_ptr<GameFlowController> instance;
    return instance;
}

std::mutex& GameFlowProvider::Mutex() {
    static std::mutex mutex;
    return mutex;
}

}  // namespace state
}  // namespace core
}  // namespace carrot


