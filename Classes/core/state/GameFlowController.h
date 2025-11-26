#pragma once

#include <memory>
#include <mutex>

namespace carrot {
namespace core {
namespace state {

class GameStateContext;

// Centralizes high-level game flow transitions so individual scenes
// don't have to manipulate GameStateContext directly.
class GameFlowController : public std::enable_shared_from_this<GameFlowController> {
public:
    explicit GameFlowController(std::shared_ptr<GameStateContext> context = nullptr);

    void SetContext(const std::shared_ptr<GameStateContext>& context);

    void TransitionToMenu();
    void TransitionToLevelSelect();

private:
    std::shared_ptr<GameStateContext> EnsureContextLocked();

    std::mutex mutex_;
    std::weak_ptr<GameStateContext> context_;
};

}  // namespace state
}  // namespace core
}  // namespace carrot


