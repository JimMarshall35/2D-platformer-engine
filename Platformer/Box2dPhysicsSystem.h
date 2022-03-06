#pragma once
#include "ISystem.h"
#include <memory>
class Box2dPhysicsSystem : public ISystem
{
	// Inherited via ISystem
	virtual void Update(float delta_t, Camera2D& camera, Engine& engine) override;
	virtual void Initialize(Engine* engine) override;
private:
};

