#pragma once
#include "ISystem.h"

class AnimationSystem :
    public ISystem
{
public:
    void Update(float delta_t, Camera2D& camera, Engine& engine);
};

