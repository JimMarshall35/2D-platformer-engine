#include "ECS.h"
EntityID GetEntityId()
{
	static EntityID id = 1;
	return id++;
}

EntityID ECS::CreateEntity(std::vector<ComponentType> components)
{
	EntityID id = GetEntityId();
	for (const auto& ct : components) {
		switch (ct) {
		case CT_TRANSFORM:
			_Components.transforms[id] = Transform();
			break;
		case CT_ANIMATION:
			_Components.animations[id] = Animation();
			break;
		case CT_HEALTHS:
			_Components.healths[id] = Health();
			break;
		case CT_PHYSICS:
			_Components.physicses[id] = Physics();
			break;
		case CT_PLAYERBEHAVIOR:
			_Components.player_behaviors[id] = PlayerBehavior();
			break;
		case CT_SPRITE:
			_Components.sprites[id] = Sprite();
			break;
		}
	}
	_Entities[id] = components;
	return id;
}

bool ECS::DeleteEntity(EntityID id)
{
	const auto& components = _Entities[id];
	for (const auto& ct : components) {
		switch (ct) {
		case CT_TRANSFORM:
			_Components.transforms.erase(id);
			break;
		case CT_ANIMATION:
			_Components.animations.erase(id);
			break;
		case CT_HEALTHS:
			_Components.healths.erase(id);
			break;
		case CT_PHYSICS:
			_Components.physicses.erase(id);
			break;
		case CT_PLAYERBEHAVIOR:
			_Components.player_behaviors.erase(id);
			break;
		case CT_SPRITE:
			_Components.sprites.erase(id);
			break;
		}
	}
	_Entities.erase(id);
	return true;
}
