#pragma once
#include "ILevelSerializer.h"
class lua_State;


class LuaLevelSerializer : public ILevelSerializer
{
public:

#pragma region ILevelSerializer implementation
	void Serialize(const Engine& engine, std::string filePath);
	bool DeSerialize(Engine& engine, std::string filePath);
#pragma endregion
	LuaLevelSerializer();
	~LuaLevelSerializer();
#pragma region lua wrapped functions
	static int l_LoadTilesFromImageFile(lua_State* L); // lua parameters: engine*, tile_W_px, tile_H_px, image_path | returns lua: bool success/failiure 
	static int l_LoadLevelFromLuaTable(lua_State* L);  // lua parameters engine*, (lua table representing level) | returns lua: bool success/failiure 
#pragma endregion

private:
	bool CheckLua(lua_State* L, int returncode);
	void RegisterLuaAPI();
	lua_State* _L;
	inline void registerFunction(int(*func)(lua_State* L), std::string func_name);
};

