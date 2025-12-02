#include <GameEngineFramework/UI/components/textField.h>

TextField::TextField() : 
    isActive(true),
    doStayInFocus(false),
    x(0),
    y(0),
    w(100),
    h(50),
    color(1, 1, 1, 1),
    text(""),
    callback(nullptr) {}
