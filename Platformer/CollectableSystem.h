#pragma once
#include "ISystem.h"

class CollectableSystem :
    public ISystem
{
public:
    CollectableSystem(Engine* e);
    // Inherited via ISystem
    virtual void Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers) override;
};

