#pragma once
#include "ISystem.h"
class ExplodingSpriteUpdateSystem :
    public ISystem
{
public:
    ExplodingSpriteUpdateSystem(Engine* e);
    // Inherited via ISystem
    virtual void Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers) override;
};

