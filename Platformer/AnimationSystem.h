#pragma once
#include "ISystem.h"

class AnimationSystem :
    public ISystem
{
public:
    AnimationSystem(Engine* e) :ISystem(e) {}
    void Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers);
};

