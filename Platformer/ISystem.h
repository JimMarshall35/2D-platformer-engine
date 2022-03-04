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
	virtual void Update(float delta_t, Camera2D& camera, Engine& engine ) = 0;
	virtual void Initialize(Engine* engine) = 0;
};