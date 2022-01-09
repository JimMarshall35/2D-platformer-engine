#pragma once
#include <glm/glm.hpp>
#include <unordered_map>

using EntityID = int64_t;

struct Sprite {
	unsigned int texture;
};

struct Transform
{
	glm::vec2 pos;
	glm::vec2 scale;
	float rot;
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
	FloorCollider collider;
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

struct PlayerBehavior {
	bool leftPressed;
	bool rightPressed;
	bool upPressed;
	bool downPressed;
	bool spacePressed;
	bool jumping;
	int jumpcounter = 0;
};
#define NUM_COMPONENTS 6
enum ComponentType : unsigned int {
	CT_TRANSFORM = 1,
	CT_PHYSICS = 2,
	CT_HEALTHS = 3,
	CT_SPRITE = 4,
	CT_ANIMATION = 5,
	CT_PLAYERBEHAVIOR = 6
};
template <typename Type>
using ComponentMap = std::unordered_map<EntityID, Type>;
using Transforms = ComponentMap<Transform>;
using Physicses = ComponentMap<Physics>;
using Healths = ComponentMap<Health>;
using Sprites = ComponentMap<Sprite>;
using Animations = ComponentMap<Animation>;
using PlayerBehaviors = ComponentMap<PlayerBehavior>;

struct Components
{
	Transforms transforms;
	Physicses physicses;
	Healths healths;
	Sprites sprites;
	Animations animations;
	PlayerBehaviors player_behaviors;
};
enum class EntityType {
	Undefined,
	Player,

};
struct TaggedEntity {
	EntityID id;
	std::vector<ComponentType> components;
};

EntityID GetEntityId();

class ECS {
public:
	EntityID CreateEntity(std::vector<ComponentType> components);
	bool DeleteEntity(EntityID id);
	Components _Components;
	std::unordered_map<EntityID, std::vector<ComponentType>> _Entities;
	

};