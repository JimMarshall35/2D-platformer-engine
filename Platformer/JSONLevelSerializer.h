#pragma once
#include "ILevelSerializer.h"
#include <string>
class JSONLevelSerializer : public ILevelSerializer
{
public:
	void Serialize(const Engine& engine, std::string filePath);
	bool DeSerialize(Engine& engine, std::string filePath);
};

