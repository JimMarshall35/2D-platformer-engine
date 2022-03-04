#pragma once
#include "ISystem.h"
class TileLayer;
class Physics;
class EnemyBehaviorSystem :
    public ISystem
{
public:
    // Inherited via ISystem
    virtual void Update(float delta_t, Camera2D& camera, Engine& engine) override;

private:
    bool SolidTileAtCoords(int x, int y, const std::vector<TileLayer>& tileLayers, const Physics& phys);

    // Inherited via ISystem
    virtual void Initialize(Engine* engine) override;
};

