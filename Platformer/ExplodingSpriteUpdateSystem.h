#pragma once
#include "ISystem.h"
class ExplodingSpriteUpdateSystem :
    public ISystem
{
public:
    // Inherited via ISystem
    virtual void Update(float delta_t, Camera2D& camera, Engine& engine) override;

    // Inherited via ISystem
    virtual void Initialize(Engine* engine) override;
};

