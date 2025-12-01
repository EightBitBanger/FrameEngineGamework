#include <GameEngineFramework/UI/components/sprite.h>

Sprite::Sprite() : 
    name("image"),
    isActive(true),
    texture(nullptr),
    x(0),
    y(0),
    w(0),
    h(0),
    u0(0.0f),
    v0(1.0f),
    u1(1.0f),
    v1(0.0f),
    tint(1.0f, 1.0f, 1.0f, 1.0f)
{
}
