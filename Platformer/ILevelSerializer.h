
#pragma once
#include <string>
class Engine;
class ILevelSerializer {
public:
	virtual void Serialize(const Engine& engine, std::string filePath) = 0;
	virtual bool DeSerialize(Engine& engine, std::string filePath) = 0;
};