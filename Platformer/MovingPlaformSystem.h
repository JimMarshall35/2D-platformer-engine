#pragma once
#include "ISystem.h"
class MovingPlaformSystem : public ISystem
{
	void Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers);
};

