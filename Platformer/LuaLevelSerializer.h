#pragma once
#include "ILevelSerializer.h"
class lua_State;
class LuaVMService;

class LuaLevelSerializer : public ILevelSerializer
{
public:

#pragma region ILevelSerializer implementation
	void Serialize(const Engine& engine, std::string filePath);
	bool DeSerialize(Engine& engine, std::string filePath);
#pragma endregion
	LuaLevelSerializer(LuaVMService*);
	~LuaLevelSerializer();


private:
	bool CheckLua(lua_State* L, int returncode);
	void RegisterLuaAPI();
	LuaVMService* _VM;
	inline void registerFunction(int(*func)(lua_State* L), std::string func_name);
};

