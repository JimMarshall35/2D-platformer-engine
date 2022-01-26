#include "ECS.h"
#include <algorithm>
#include <set>
#include <locale>


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
		case CT_HEALTH:
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
		case CT_MOVINGPLATFORM:
			_Components.moving_platforms[id] = MovingPlatform();
			break;
		case CT_COLLECTABLE:
			_Components.collectables[id] = Collectable();
			break;
		case CT_ENEMYBEHAVIOR:
			_Components.enemy_behaviors[id] = EnemyBehavior();
			break;
		case CT_EXPLODINGSPRITE:
			_Components.exploding_sprites[id] = ExplodingSprite();
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
		case CT_HEALTH:
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
		case CT_MOVINGPLATFORM:
			_Components.moving_platforms.erase(id);
			break;
		case CT_COLLECTABLE:
			_Components.collectables.erase(id);
			break;
		case CT_ENEMYBEHAVIOR:
			_Components.enemy_behaviors.erase(id);
			break;
		case CT_EXPLODINGSPRITE:
			_Components.exploding_sprites.erase(id);
			break;
		}
	}
	_Entities.erase(id);
	_DeletedIds.push(id);
	return true;
}

template<typename TK, typename TV>
std::vector<TK> extract_keys(std::unordered_map<TK, TV> const& input_map) {
	std::vector<TK> retval;
	for (auto const& element : input_map) {
		retval.push_back(element.first);
	}
	return retval;
}

template<typename TK, typename TV>
std::vector<TV> extract_values(std::unordered_map<TK, TV> const& input_map) {
	std::vector<TV> retval;
	for (auto const& element : input_map) {
		retval.push_back(element.second);
	}
	return retval;
}
// Function to convert Vector to Set
std::set<EntityID> convertToSet(std::vector<EntityID> v)
{
	// Declaring the set
	// using range of vector
	std::set<EntityID> s(v.begin(), v.end());

	// Return the resultant Set
	return s;
}

std::set<EntityID> ECS::getKeys(ComponentType type, const Components& components) {
	switch (type) {
	case CT_ANIMATION:
		return convertToSet(extract_keys<EntityID, Animation>(components.animations));
		break;
	case CT_HEALTH:
		return convertToSet(extract_keys<EntityID, Health>(components.healths));
		break;
	case CT_PHYSICS:
		return convertToSet(extract_keys<EntityID, Physics>(components.physicses));
		break;
	case CT_PLAYERBEHAVIOR:
		return convertToSet(extract_keys<EntityID, PlayerBehavior>(components.player_behaviors));
		break;
	case CT_SPRITE:
		return convertToSet(extract_keys<EntityID, Sprite>(components.sprites));
		break;
	case CT_TRANSFORM:
		return convertToSet(extract_keys<EntityID, Transform>(components.transforms));
		break;
	case CT_MOVINGPLATFORM:
		return convertToSet(extract_keys<EntityID, MovingPlatform>(components.moving_platforms));
		break;
	case CT_COLLECTABLE:
		return convertToSet(extract_keys<EntityID, Collectable>(components.collectables));
		break;
	case CT_ENEMYBEHAVIOR:
		return convertToSet(extract_keys<EntityID, EnemyBehavior>(components.enemy_behaviors));
		break;
	case CT_EXPLODINGSPRITE:
		return convertToSet(extract_keys<EntityID, ExplodingSprite>(components.exploding_sprites));
		break;
	}
}

std::set<EntityID> ECS::getIntersection(const std::vector<ComponentType>& componentTypes, const Components& components)
{
	if (componentTypes.size() == 0) return std::set<EntityID>();
	
	auto ct = componentTypes[0];
	
	std::set<EntityID> lastkeys;
	lastkeys = getKeys(componentTypes[0], components);
	if (componentTypes.size() == 1) {
		return lastkeys;
	}
	std::unordered_set<EntityID> rset;
	for (int i = 1; i < componentTypes.size(); i++) {
		
		auto nextkeys = getKeys(componentTypes[i], components);
		std::set<EntityID> intersect;
		set_intersection(lastkeys.begin(), lastkeys.end(), nextkeys.begin(), nextkeys.end(),
			std::inserter(intersect, intersect.begin()));
		lastkeys = intersect;
	}
	return lastkeys;
	
}

EntityID ECS::GetEntityId()
{
	static EntityID id = 1;
	if (!_DeletedIds.empty()) {
		EntityID recycled = _DeletedIds.front();
		_DeletedIds.pop();
		return recycled;
	}
		
	return id++;
}
