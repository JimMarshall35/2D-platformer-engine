#pragma once
#include "ISystem.h"

class CollectableSystem :
    public ISystem
{
public:
    // Inherited via ISystem
    virtual void Update(float delta_t, Camera2D& camera, Engine& engine) override;
};

