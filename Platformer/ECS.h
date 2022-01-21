#pragma once
#include <glm/glm.hpp>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <queue>

using EntityID = int64_t;

struct Sprite {
	unsigned int texture;
};

struct Transform{
	glm::vec2 pos;
	glm::vec2 scale;
	float rot;
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
	int max;
	int current;
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
	NoState,
	Walk,
	JumpUp,
	JumpDown,
	JumpLand,
	Climb
};
struct PlayerBehavior {
	bool leftPressed;
	bool rightPressed;
	bool upPressed;
	bool downPressed;
	bool spacePressed;
	bool jumping;
	int jumpcounter = 0;
	PlayerState state = Walk;
	PlayerState laststate = NoState;

	float MAX_X_SPEED = 100;
	float movespeed = 700;
	float jumpmovespeed = 700;
	float JumpAmount = 250;
	float climbspeed = 50;

	float friction = 1.0f;
};

struct Collectable {

};

struct EnemyBehavior {

};
#define NUM_COMPONENTS 10
enum ComponentType : unsigned int {
	CT_INVALID = 0,
	CT_TRANSFORM = 1,
	CT_PHYSICS = 2,
	CT_HEALTHS = 3,
	CT_SPRITE = 4,
	CT_ANIMATION = 5,
	CT_PLAYERBEHAVIOR = 6,
	CT_MOVINGPLATFORM = 7,

	CT_COLLECTABLE = 8,
	CT_ENEMYBEHAVIOR = 9
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
	EntityID CreateEntity(std::vector<ComponentType> components);
	bool DeleteEntity(EntityID id);
	Components _Components;
	std::unordered_map<EntityID, std::vector<ComponentType>> _Entities;

	static std::set<EntityID> getKeys(ComponentType type, const Components& components);
	static std::set<EntityID> getIntersection(const std::vector<ComponentType>& componentTypes, const Components& components);
private:
	EntityID GetEntityId();
	std::queue<EntityID> _DeletedIds;
};