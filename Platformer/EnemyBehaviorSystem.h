#pragma once
#include "ISystem.h"
class TileLayer;
class Physics;
class EnemyBehaviorSystem :
    public ISystem
{
public:
    EnemyBehaviorSystem(Engine* e):ISystem(e){}
    // Inherited via ISystem
    virtual void Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers) override;

private:
    bool SolidTileAtCoords(int x, int y, const std::vector<TileLayer>& tileLayers, const Physics& phys);
};

