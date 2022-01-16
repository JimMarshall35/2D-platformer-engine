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
	static int l_LoadLevelFromLuaTable(lua_State* L);     // lua parameters: engine*, (lua table representing level) | returns bool success/failiure 
	static int l_CreateEntity(lua_State* L);              // lua parameters: engine*, (lua table list of ComponentTypes) | returns an entityID
	static int l_SetTransformComponent(lua_State* L);     // lua parameters: engine*, enityID, (lua table matching transform component) | returns void
	static int l_SetFloorColliderComponent(lua_State* L); // lua parameters: engine*, entityID, topPx, bottomPx, leftPx, RightPX | returns void
	static int l_SetSpriteComponent(lua_State* L);        // lua parameters: engine*, entityID, sprite | returns void 
	static int l_SetAnimationComponent(lua_State* L);     // lua parameters: engine*, entityID, {isanimating,name,numframes,timer,shouldloop,onframe} | returns void
	static int l_SetEntityPlayer1(lua_State* L);          // lua parameters: engine*, entityId | returns void
	static int l_SetMovingPlatformComponent(lua_State* L);// lua parameters: engine*, entityId, point1, point2, timeperiod, timer | returns void
	static int l_LoadTilesetFile(lua_State* L);           // lua parameters: engine*, tileset_w, tileset_h, path
	static int l_LoadTileLayer(lua_State* L);             // lua parameters: engine*, widthtiles, heighttiles, t_type, tiles
	static int l_LoadAnimationFrames(lua_State* L);       // lua parameters: engine*, name, frames
	static int l_GetTileset(lua_State* L);                // lua parameters: engine*
	static int l_GetEntities(lua_State* L);               // lua parameters: engine*
	static int l_GetAnimations(lua_State* L);             // lua parameters: engine*
	static int l_GetTilelayers(lua_State* L);             // lua parameters: engine*

#pragma endregion

private:
	bool CheckLua(lua_State* L, int returncode);
	void RegisterLuaAPI();
	lua_State* _L;
	inline void registerFunction(int(*func)(lua_State* L), std::string func_name);
};

