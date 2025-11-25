#include "PausedState.h"

#include "cocos2d.h"

namespace carrot {
namespace core {
namespace state {

void PausedState::Enter(GameStateContext& context) {
    cocos2d::Director::getInstance()->pause();
}

void PausedState::Exit(GameStateContext& context) {
    cocos2d::Director::getInstance()->resume();
}

}  // namespace state
}  // namespace core
}  // namespace carrot

