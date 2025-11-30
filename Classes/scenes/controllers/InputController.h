#pragma once
#include "cocos2d.h"

class InputController {
public:
    using MouseDownCallback = std::function<void(const cocos2d::Vec2&)>;
    InputController(cocos2d::Node* target);
    ~InputController();
    void addMouseListener();
    void setMouseDownCallback(MouseDownCallback cb);
private:
    cocos2d::Node* target_;
    cocos2d::EventListenerMouse* mouseListener_ = nullptr;
    MouseDownCallback mouseDownCallback_ = nullptr;
    void handleMouseDown(cocos2d::EventMouse* event);
};
