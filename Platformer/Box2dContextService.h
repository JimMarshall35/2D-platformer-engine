#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "Box2dWorldSettings.h"
#include <atomic>
#include <mutex>
#include <thread>
#include <memory>
#include "ECS.h"
#include "TSQueue.h"
#include <functional>

struct PhysicsCommand {
	std::function<void()> Execute;
};
class Engine;

class b2World;
class b2Body;
class Box2dContextService
{
public:
	Box2dContextService(Engine* e);
	~Box2dContextService();
	void initialize();
	b2Body* MakeStaticPolygon(const std::vector<glm::vec2>& points, EntityID id);
	b2Body* MakeDynamicBox(float halfwidth, float halfheight, const glm::vec2& center, float angle, EntityID id);
	b2Body* MakeStaticBox(float halfwidth, float halfheight, const glm::vec2& center, float angle, EntityID id);
	b2Body* MakeStaticCircle(float radius, const glm::vec2& center, EntityID id);
	b2Body* MakeDynamicCircle(float radius, const glm::vec2& center, EntityID id);
	void ApplyForce(EntityID entity, const glm::vec2& force, const glm::vec2& point);
	void DeleteBody(b2Body* body);
	const Box2dWorldSettings& GetSettings() {
		return s_settings;
	}
	static void Step();
private:
	b2Body* MakeCircleBase(float radius, const glm::vec2& center, EntityID id, bool dynamic);

	static glm::vec2 pixelsToMeterConversion(glm::vec2 pixelsPosition);
	static glm::vec2 metersToPixelsConversion(glm::vec2 metersPosition);
	static float pixelsToMeterConversion(float val);
	static float metersToPixelsConversion(float val);
	
	static void PhysicsWorker();
private:
	
	static const float s_pixelsPerMeter;
	static b2World* s_world;
	static std::atomic_bool s_physicsWorkerShouldContinue;
	static Box2dWorldSettings s_settings;
	static Engine* s_engine;
	std::unique_ptr<std::thread> _physicsThread;
	static std::mutex s_b2dMutex;
	static TSQueue<PhysicsCommand> _physicsCommandQueue;

};




