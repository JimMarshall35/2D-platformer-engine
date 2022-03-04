#pragma once
#include "ISystem.h"
class MovingPlaformSystem : public ISystem
{
public:
	void Update(float delta_t, Camera2D& camera, Engine& engine) override;

	// Inherited via ISystem
	virtual void Initialize(Engine* engine) override;
};

