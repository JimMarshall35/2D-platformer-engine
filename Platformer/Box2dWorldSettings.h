#pragma once
struct Box2dWorldSettings
{
	Box2dWorldSettings()
	{
		Reset();
	}

	void Reset()
	{
		m_testIndex = 0;
		m_windowWidth = 1600;
		m_windowHeight = 900;
		m_hertz = 60.0;
		m_velocityIterations = 10;//8;
		m_positionIterations = 5;//3;
		m_drawShapes = true;
		m_drawJoints = true;
		m_drawAABBs = false;
		m_drawContactPoints = false;
		m_drawContactNormals = false;
		m_drawContactImpulse = false;
		m_drawFrictionImpulse = false;
		m_drawCOMs = false;
		m_drawStats = false;
		m_drawProfile = false;
		m_enableWarmStarting = true;
		m_enableContinuous = true;
		m_enableSubStepping = true;
		m_enableSleep = false;
		m_pause = false;
		m_singleStep = false;
	}

	int m_testIndex;
	int m_windowWidth;
	int m_windowHeight;
	float m_hertz;
	int m_velocityIterations;
	int m_positionIterations;
	bool m_drawShapes;
	bool m_drawJoints;
	bool m_drawAABBs;
	bool m_drawContactPoints;
	bool m_drawContactNormals;
	bool m_drawContactImpulse;
	bool m_drawFrictionImpulse;
	bool m_drawCOMs;
	bool m_drawStats;
	bool m_drawProfile;
	bool m_enableWarmStarting;
	bool m_enableContinuous;
	bool m_enableSubStepping;
	bool m_enableSleep;
	bool m_pause;
	bool m_singleStep;
};