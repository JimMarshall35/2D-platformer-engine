#pragma once
#include <vector>
#include "Tilelayer.h"
class Components;
class Camera2D;
class TileSet;
class ISystem {
public:
	virtual void Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers ) = 0;
};