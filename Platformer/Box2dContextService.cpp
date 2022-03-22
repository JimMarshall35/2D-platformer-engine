#include "Box2dContextService.h"
#include <box2d/b2_world.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_body.h>
#include <box2d/b2_settings.h>
#include <box2d/b2_edge_shape.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_circle_shape.h>
#include <algorithm>
#include <chrono>
#include "Engine.h"

Box2dWorldSettings Box2dContextService::s_settings;
std::atomic_bool Box2dContextService::s_physicsWorkerShouldContinue = true;
b2World* Box2dContextService::s_world = nullptr;
Engine* Box2dContextService::s_engine = nullptr;
const float Box2dContextService::s_pixelsPerMeter = 16;
std::mutex Box2dContextService::s_b2dMutex;

Box2dContextService::Box2dContextService(Engine* e)
{
	s_engine = e;
}

Box2dContextService::~Box2dContextService()
{
	if (_physicsThread != nullptr) {
		s_physicsWorkerShouldContinue = false;
		_physicsThread->join();
	}
	
	if(s_world != nullptr)
		delete s_world;
}
void Box2dContextService::initialize()
{
	b2Vec2 gravity(0.0f, 10.0f);
	s_world = new b2World(gravity);
	s_world->SetAllowSleeping(s_settings.m_enableSleep);
	s_world->SetWarmStarting(s_settings.m_enableWarmStarting);
	s_world->SetContinuousPhysics(s_settings.m_enableContinuous);
	s_world->SetSubStepping(s_settings.m_enableSubStepping);
#ifdef SEPARATE_PHYSICS_THREAD
	_physicsThread = std::unique_ptr<std::thread>(new std::thread(PhysicsWorker));
#endif
}

b2Body* Box2dContextService::MakeStaticPolygon(const std::vector<glm::vec2>& points, EntityID id)
{
	
	b2PolygonShape shape;
	std::vector<b2Vec2> transformed(points.size());
	size_t points_size = points.size();
	for (int i = 0; i < points_size; i++) {
		glm::vec2 converted = pixelsToMeterConversion(points[i]);
		transformed[i] = b2Vec2(converted.x, converted.y);
	}
	
	
	b2BodyDef bodydef;
	bodydef.enabled = true;
	bodydef.type = b2_staticBody;
	b2BodyUserData ud;
	
	bodydef.userData.pointer = id; // store the entity ID instead of a pointer (both same size in bytes)
	b2Body* body = nullptr;
	{
		std::lock_guard<std::mutex>lg(s_b2dMutex);
		body = s_world->CreateBody(&bodydef);
	}
	
	for (int i = 0; i < points_size / 2; i++) {
		auto v1 = transformed[i * 2];
		auto v2 = transformed[(i * 2) + 1];
		b2EdgeShape shape;
		shape.SetTwoSided(v1, v2);
		{
			std::lock_guard<std::mutex>lg(s_b2dMutex);
			body->CreateFixture(&shape, 1.0f);
		}
		
	}

	return body;
}

b2Body* Box2dContextService::MakeDynamicBox(float halfwidth, float halfheight, const glm::vec2& center, float angle, EntityID id)
{
	
	auto PhysicsCoordsCenter = pixelsToMeterConversion(center);
	auto PhysicsCoordsHalfHeight = pixelsToMeterConversion(halfheight);
	auto PhysicsCoordsHalfWidth = pixelsToMeterConversion(halfwidth);
	assert(s_engine->_Components.box2d_physicses.find(id) != s_engine->_Components.box2d_physicses.end());

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	//bodyDef.enabled = true;
	//bodyDef.allowSleep = false;
	//bodyDef.fixedRotation = true;
	bodyDef.position.Set(PhysicsCoordsCenter.x, PhysicsCoordsCenter.y);
	//bodyDef.awake = true;
	
	bodyDef.userData.pointer = id;

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(PhysicsCoordsHalfWidth, PhysicsCoordsHalfHeight, b2Vec2(0, 0), angle);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;


	b2Body* body = nullptr;
	{
		std::lock_guard<std::mutex>lg(s_b2dMutex);

		body = s_world->CreateBody(&bodyDef);
		body->CreateFixture(&fixtureDef);

		//body->SetAwake(true);
		
	}

	return body;
}

b2Body* Box2dContextService::MakeStaticBox(float halfwidth, float halfheight, const glm::vec2& center, float angle, EntityID id)
{
	
	auto PhysicsCoordsCenter = pixelsToMeterConversion(center);
	auto PhysicsCoordsHalfHeight = pixelsToMeterConversion(halfheight);
	auto PhysicsCoordsHalfWidth = pixelsToMeterConversion(halfwidth);

	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.enabled = true;
	bodyDef.allowSleep = false;
	bodyDef.position.Set(PhysicsCoordsCenter.x, PhysicsCoordsCenter.y);
	bodyDef.userData.pointer = id;
	bodyDef.awake = true;


	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(PhysicsCoordsHalfWidth, PhysicsCoordsHalfHeight, b2Vec2(0, 0), angle);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 0.0f;
	fixtureDef.friction = 0.3f;

	b2Body* body = nullptr;
	{
		std::lock_guard<std::mutex>lg(s_b2dMutex);

		

		body = s_world->CreateBody(&bodyDef);
		body->CreateFixture(&dynamicBox, 0.0);

		body->SetAwake(true);
	}
	
	return body;
}

b2Body* Box2dContextService::MakeStaticCircle(float radius, const glm::vec2& center, EntityID id)
{
	return MakeCircleBase(radius, center, id, false);
}

b2Body* Box2dContextService::MakeDynamicCircle(float radius, const glm::vec2& center, EntityID id)
{
	return MakeCircleBase(radius, center, id, true);
}

b2Vec2 ConvertGlmToBox2D(const glm::vec2 vec2convert) {
	return b2Vec2(vec2convert.x, vec2convert.y);
}
void Box2dContextService::ApplyForce(EntityID entity, const glm::vec2& force, const glm::vec2& point)
{
	assert(s_engine->_Components.box2d_physicses.find(entity) != s_engine->_Components.box2d_physicses.end());
	
	b2Body* body = s_engine->_Components.box2d_physicses[entity].body;
	auto meterUnitsPoint = pixelsToMeterConversion(point);
	std::lock_guard<std::mutex>lg(s_b2dMutex);
	body->ApplyForce(ConvertGlmToBox2D(force), b2Vec2(meterUnitsPoint.x, meterUnitsPoint.y),true);
}

void Box2dContextService::DeleteBody(b2Body* body)
{
	std::lock_guard<std::mutex>lg(s_b2dMutex);
	s_world->DestroyBody(body);
}

glm::vec2 Box2dContextService::pixelsToMeterConversion(glm::vec2 pixelsPosition)
{
	return (glm::vec2(pixelsPosition.x/s_pixelsPerMeter, pixelsPosition.y / s_pixelsPerMeter));
	//return pixelsPosition;
}

glm::vec2 Box2dContextService::metersToPixelsConversion(glm::vec2 metersPosition)
{
	return (glm::vec2(metersPosition.x * s_pixelsPerMeter, metersPosition.y * s_pixelsPerMeter));
	//return metersPosition;
}

float Box2dContextService::pixelsToMeterConversion(float val)
{
	return (val / s_pixelsPerMeter);
	//return val;
}

float Box2dContextService::metersToPixelsConversion(float val)
{
	return (val * s_pixelsPerMeter);
	//return val;
}

void Box2dContextService::Step()
{
	int32 numbodies;
	b2Body* bodiesList;
	float timeStep = s_settings.m_hertz > 0.0f ? 1.0f / s_settings.m_hertz : float(0.0f);
	{
		std::lock_guard<std::mutex>lg(s_b2dMutex);
		
		s_world->Step(timeStep, s_settings.m_velocityIterations, s_settings.m_positionIterations);
		numbodies = s_world->GetBodyCount();
		bodiesList = s_world->GetBodyList();
		//s_world->ClearForces();
	}
	b2Body* current = bodiesList;
	while(current != nullptr) {
		b2Body& body = *current;
		b2BodyType type = body.GetType();
		if (type == b2BodyType::b2_dynamicBody) {
			// set the transforms used for rendering
			EntityID id = (EntityID)body.GetUserData().pointer;

			auto b2pos = body.GetPosition();
			auto glmPosition = glm::vec2(b2pos.x, b2pos.y);
			auto rot = glm::degrees(body.GetAngle());
			auto& tr = s_engine->_Components.transforms[id];
			tr.SetPos(metersToPixelsConversion(glmPosition));
			tr.SetRot(rot);
		}
		current = current->GetNext();;
	}

}

b2Body* Box2dContextService::MakeCircleBase(float radius, const glm::vec2& center, EntityID id, bool dynamic)
{
	auto PhysicsCoordsCenter = pixelsToMeterConversion(center);
	auto PhysicsCoordsRadius = pixelsToMeterConversion(radius);

	b2BodyDef bodyDef;
	bodyDef.type = dynamic ? b2_dynamicBody : b2_staticBody;
	bodyDef.enabled = true;
	bodyDef.allowSleep = false;
	//bodyDef.fixedRotation = true;
	bodyDef.position.Set(PhysicsCoordsCenter.x, PhysicsCoordsCenter.y);
	bodyDef.awake = true;
	bodyDef.userData.pointer = id;


	b2CircleShape dynamicCircle;
	dynamicCircle.m_radius = PhysicsCoordsRadius;
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicCircle;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;

	b2Body* body = nullptr;
	{
		std::lock_guard<std::mutex>lg(s_b2dMutex);

		body = s_world->CreateBody(&bodyDef);
		body->CreateFixture(&fixtureDef);

		//body->SetAwake(true);
	}

	return body;
}

void Box2dContextService::PhysicsWorker()
{
	std::chrono::duration<double> frameTime(0.0);
	std::chrono::duration<double> sleepAdjust(0.0);
	while (s_physicsWorkerShouldContinue) {
		std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
		// do work
		
		Step();
		
		
		

		// Throttle to cap at 60Hz. This adaptive using a sleep adjustment. This could be improved by
		// using mm_pause or equivalent for the last millisecond.
		std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
		std::chrono::duration<double> target(1.0 / s_settings.m_hertz);
		std::chrono::duration<double> timeUsed = t2 - t1;
		std::chrono::duration<double> sleepTime = target - timeUsed + sleepAdjust;
		if (sleepTime > std::chrono::duration<double>(0))
		{
			std::this_thread::sleep_for(sleepTime);
		}

		std::chrono::steady_clock::time_point t3 = std::chrono::steady_clock::now();
		frameTime = t3 - t1;

		// Compute the sleep adjustment using a low pass filter
		sleepAdjust = 0.9 * sleepAdjust + 0.1 * (target - frameTime);
	}
}


