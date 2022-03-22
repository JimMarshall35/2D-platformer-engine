#pragma once
#include <glm/glm.hpp>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <queue>
#include <iostream>
#include <mutex>

#include "IdGenerator.h"

using EntityID = int64_t;

struct Sprite {
	unsigned int texture;
	bool shoulddraw = true;
};

struct ExplodingSprite {
	unsigned int texture;
	bool shoulddraw = true;
	float explodeTimer = 0;
	float explodeTime = 1.0;
	bool finishedExploding = false;
};

struct Transform{
	Transform() {}
	Transform(const Transform&) = default;

	glm::vec2 pos;
	glm::vec2 scale;
	float rot;
public:
	void SetPos(const glm::vec2& newpos){
		std::lock_guard<std::mutex> lg(mut);
		pos.x = newpos.x;
		pos.y = newpos.y;
	}
	glm::vec2 GetPos() {
		std::lock_guard<std::mutex> lg(mut);
		return pos;
	}
	void SetRot(float newrot) {
		std::lock_guard<std::mutex> lg(mut);
		rot = newrot;
	}
	float GetRot() {
		return rot;
	}
private:
	std::mutex mut;
};

struct MovingPlatform {
	glm::vec2 point1;
	glm::vec2 point2;
	double time_period;
	double timer = 0;
	glm::vec2 lastpos;
};

struct Health
{
	int max = 5;
	int current = 5;
};

struct FloorCollider {
	float MinusPixelsTop;
	float MinusPixelsBottom;
	float MinusPixelsLeft;
	float MinusPixelsRight;
	bool Collidable = true;
};

struct Physics {
	glm::vec2 velocity = glm::vec2(0.0f);
	glm::vec2 lastPos;
	bool bottomTouching = false;
	bool topTouching = false;
	bool lastbottomTouching = false;
	bool leftTouching = false;
	bool rightTouching = false;
	bool lastLeftTouching = false;
	bool lastRightTouching = false;
	bool gravity = true;
	FloorCollider collider;
	EntityID movingPlatformId = 0;
};
class b2Body;
struct Box2dPhysics {
	b2Body* body;
};

struct Animation {
	bool isAnimating = false;
	std::string animationName;
	int numframes = 0;
	double timer = 0;
	double fps = 0;
	bool shouldLoop = false;
	int onframe = 0;
	
};
enum PlayerState: unsigned int {
	NoState = 0,
	Walk = 1,
	JumpUp = 2,
	JumpDown = 3,
	JumpLand = 4,
	Climb = 5,
	KnockBack = 6, 
	Dead = 7,
	Stabbing = 8
};

template< typename StateEnum>
struct BehaviorComponent {
	StateEnum state;
	StateEnum laststate; // must be initialized with an enum of the value 0
};
struct PlayerBehavior : BehaviorComponent<PlayerState>
{
	PlayerBehavior() {
		state = Walk;
		laststate = NoState;
	}
	bool leftPressed = false;
	bool rightPressed = false;
	bool upPressed = false;
	bool downPressed = false;
	bool spacePressed = false;
	bool attackPressed = false;
	bool jumping = false;
	int jumpcounter = 0;

	float MAX_X_SPEED = 100;
	float movespeed = 700;
	float jumpmovespeed = 700;
	float JumpAmount = 250;
	float climbspeed = 50;

	float friction = 1.0f;

	EntityID colliding_enemy = 0;

	double knockback_timer = 0.0;
	double knockback_blink_timer = 0.0;

	int coins_collected = 0;
};

enum class CollectableType {
	None = 0,
	Coin = 1
};

struct Collectable {
	CollectableType type;
	union {
		int val_i;
		float val_f;
		const char* val_str;
	};
	bool collected = false;
};

struct EnemyBehavior {

};

#define NUM_COMPONENTS 11
enum ComponentType : unsigned int {
	CT_INVALID = 0,
	CT_TRANSFORM = 1,
	CT_PHYSICS = 2,
	CT_HEALTH = 3,
	CT_SPRITE = 4,
	CT_ANIMATION = 5,
	CT_PLAYERBEHAVIOR = 6,
	CT_MOVINGPLATFORM = 7,

	CT_COLLECTABLE = 8,
	CT_ENEMYBEHAVIOR = 9,

	CT_EXPLODINGSPRITE = 10,
	CT_BOX2DPHYSICS = 11
};

template <typename Type>
using ComponentMap = std::unordered_map<EntityID, Type>;
using Transforms = ComponentMap<Transform>;
using Physicses = ComponentMap<Physics>;
using Healths = ComponentMap<Health>;
using Sprites = ComponentMap<Sprite>;
using Animations = ComponentMap<Animation>;
using PlayerBehaviors = ComponentMap<PlayerBehavior>;
using MovingPlatforms = ComponentMap<MovingPlatform>;
using Collectables = ComponentMap<Collectable>;
using EnemyBehaviors = ComponentMap<EnemyBehavior>;
using ExplodingSprites = ComponentMap<ExplodingSprite>;
using Box2dPhysicses = ComponentMap<Box2dPhysics>;

struct Components
{
	Transforms transforms;
	Physicses physicses;
	Healths healths;
	Sprites sprites;
	Animations animations;
	PlayerBehaviors player_behaviors;
	MovingPlatforms moving_platforms;
	Collectables collectables;
	EnemyBehaviors enemy_behaviors;
	ExplodingSprites exploding_sprites;
	Box2dPhysicses box2d_physicses;
};

enum class EntityType {
	Undefined,
	Player,

};

struct TaggedEntity {
	EntityID id;
	std::vector<ComponentType> components;
};

class ECS {
public:
	template<typename T>
	void AddComponentToEntity(ComponentMap<T>& componentmap, T val, EntityID id){
		// get the corresponding enum value
		ComponentType enum_value;
		if (std::is_same<T, Sprite>::value) {
			enum_value = CT_SPRITE;
		}
		else if (std::is_same<T, ExplodingSprite>::value) {
			enum_value = CT_EXPLODINGSPRITE;
		}
		// remove from list of entities
		std::vector<ComponentType>& v = _Entities[id];
		if (std::find(v.begin(), v.end(), enum_value) == v.end()) {
			v.push_back(enum_value);
		}
		else {
			std::cerr << "component " << enum_value << " already present on entity "<<id << std::endl;
			return;
		}
		componentmap[id] = val;
	}
	template<typename T>
	void RemoveComponentFromEntity(ComponentMap<T>& componentmap, EntityID id) {
		ComponentType enum_value;
		if (std::is_same<T, Sprite>::value) {
			enum_value = CT_SPRITE;
		}
		else if (std::is_same<T, ExplodingSprite>::value) {
			enum_value = CT_EXPLODINGSPRITE;
		}

		std::vector<ComponentType>& v = _Entities[id];
		auto it = std::find(v.begin(), v.end(), enum_value);
		if (it == v.end()) {
			std::cout << "component  " << enum_value << " not present on entity "<< id<< " so can't delete" << std::endl;
		}
		else {
			v.erase(it);
		}

		componentmap.erase(id);
	}
	EntityID CreateEntity(std::vector<ComponentType> components);
	virtual bool DeleteEntity(EntityID id);
	Components _Components;
	std::unordered_map<EntityID, std::vector<ComponentType>> _Entities;

	static std::set<EntityID> getKeys(ComponentType type, const Components& components);
	static std::set<EntityID> getIntersection(const std::vector<ComponentType>& componentTypes, const Components& components);
	EntityID _lastPhysicsId_TEST = 0;
private:
	IdGenerator<EntityID> _idGenerator;
};