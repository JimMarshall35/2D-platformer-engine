#pragma once
#include <vector>
#include "Tilelayer.h"
#define OperateOnComponentGroup(...) for (auto& entityID : ECS::getIntersection(std::vector<ComponentType>({ __VA_ARGS__}), components ))
class Components;
class Camera2D;
class TileSet;
class Engine;
class ISystem {
public:
	ISystem(Engine* e) { _Engine = e; };
	virtual void Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers ) = 0;
protected:
	Engine* _Engine;
};