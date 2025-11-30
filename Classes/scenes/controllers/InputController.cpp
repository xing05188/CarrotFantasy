#include "InputController.h"
#include "cocos2d.h"

USING_NS_CC;

InputController::InputController(Node* target)
    : target_(target) {}

InputController::~InputController() {
    if (mouseListener_) {
        Director::getInstance()->getEventDispatcher()->removeEventListener(mouseListener_);
    }
}

void InputController::addMouseListener() {
    mouseListener_ = EventListenerMouse::create();
    mouseListener_->onMouseDown = CC_CALLBACK_1(InputController::handleMouseDown, this);
    target_->getEventDispatcher()->addEventListenerWithSceneGraphPriority(mouseListener_, target_);
}

void InputController::handleMouseDown(EventMouse* event) {
    Vec2 mousePos = event->getLocation();
    CCLOG("Mouse clicked at screen position: (%f, %f)", mousePos.x, mousePos.y);
    if (mouseDownCallback_) {
        mouseDownCallback_(mousePos);
    }
}

void InputController::setMouseDownCallback(MouseDownCallback cb) {
    mouseDownCallback_ = cb;
}
