#include "LuaLevelSerializer.h"
#include <iostream>
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
#include "Engine.h"

void LuaLevelSerializer::Serialize(const Engine& engine, std::string filePath)
{
}

bool LuaLevelSerializer::DeSerialize(Engine& engine, std::string filePath)
{
    engine._TileLayers.clear();
    engine._Tileset.ClearTiles();
    engine._Tileset.AnimationsMap.clear();
    lua_pushlightuserdata(_L, (void*)&engine);
    lua_setglobal(_L, "C_Engine");
    int r = luaL_dofile(_L, filePath.c_str());
    return CheckLua(_L, r);
}

LuaLevelSerializer::LuaLevelSerializer()
{
    _L = luaL_newstate();
    luaL_openlibs(_L);
    RegisterLuaAPI();
}

LuaLevelSerializer::~LuaLevelSerializer()
{
}

int LuaLevelSerializer::l_LoadTilesFromImageFile(lua_State* L)
{
    int n = lua_gettop(L);
    if (n != 4) {
        std::cout << "4 arguments required l_LoadTilesFromImageFile" << std::endl;
        lua_pushboolean(L, false);
        return 1;

    }
    Engine* e = (Engine*)lua_touserdata(L, 1);
    e->_Tileset.TileWidthAndHeightPx.x = luaL_checkinteger(L, 2);
    e->_Tileset.TileWidthAndHeightPx.y = luaL_checkinteger(L, 3);
    bool returnValue = e->_Tileset.LoadTilesFromImgFile(luaL_checkstring(L, 4));
    lua_pushboolean(L, returnValue);
    return 1;
}

int LuaLevelSerializer::l_LoadLevelFromLuaTable(lua_State* L)
{
    int n = lua_gettop(L);
    
    //int num;
    if (n != 2) {
        std::cout << "2 arguments required l_LoadLevelFromLuaTable" << std::endl;
        lua_pushboolean(L, false);
        return 1;
    }
    Engine* e = (Engine*)lua_touserdata(L, 1);
    int tilesetFilesSize, tilelayersSize, animationsSize;
    std::string filepath;
    if (lua_getfield(L, 2, "tiledims") != LUA_TTABLE) goto error;
    if (lua_getfield(L, 3, "x") != LUA_TNUMBER) goto error;
    e->_Tileset.TileWidthAndHeightPx.x = luaL_checkinteger(L, 4);
    std::cout << e->_Tileset.TileWidthAndHeightPx.x << std::endl;
    lua_pop(L, 1); // tiledims on the top
    if (lua_getfield(L, 3, "y") != LUA_TNUMBER) goto error;
    e->_Tileset.TileWidthAndHeightPx.y = luaL_checkinteger(L, 4);
    lua_pop(L, 2); // parent table on the top
    if (lua_getfield(L, 2, "tileset") != LUA_TTABLE) goto error;
    tilesetFilesSize = lua_rawlen(L, -1);
    for (int i = 0; i < tilesetFilesSize; i++) {
        lua_rawgeti(L, -1, i+1); // a tileset file table is top of the stack
        if (lua_getfield(L, -1, "path") != LUA_TSTRING) goto error;
        filepath = luaL_checkstring(L, -1);
        std::cout << filepath << std::endl;
        lua_pop(L, 1);
        if (lua_getfield(L, -1, "tile_width_px") != LUA_TNUMBER) goto error;
        if (lua_getfield(L, -2, "tile_height_px") != LUA_TNUMBER) goto error;
        e->_Tileset.TileWidthAndHeightPx.x = luaL_checkinteger(L, -2);
        e->_Tileset.TileWidthAndHeightPx.y = luaL_checkinteger(L, -1);
        e->_Tileset.LoadTilesFromImgFile(filepath);
        lua_pop(L, 3);
    }
    lua_pop(L, 1); // parent table on the top
    if (lua_getfield(L, 2, "tilelayers") != LUA_TTABLE) goto error;
    tilelayersSize = lua_rawlen(L, -1);
    for (int i = 0; i < tilelayersSize; i++) {
        lua_rawgeti(L, -1, i + 1); // a tile layer is on top of the stack
        lua_getfield(L, -1, "widthtiles");
        lua_getfield(L, -2, "heighttiles");
        lua_getfield(L, -3, "t_type"); 
        lua_getfield(L, -4, "tiles");

        TileLayer tl;
        tl.SetWidthAndHeight(luaL_checkinteger(L, -4), luaL_checkinteger(L, -3));
        tl.Type = (TileLayerType)luaL_checkinteger(L, -2);
        int tiles_size = lua_rawlen(L, -1);
        for (int j = 0; j < tiles_size; j++) {
            lua_rawgeti(L, -1, j + 1);
            tl.Tiles[j] = luaL_checkinteger(L, -1);
            lua_pop(L, 1);
        }
        e->_TileLayers.push_back(tl);
        lua_pop(L, 5);
    }
    lua_pop(L, 1); // parent table on the top
    if (lua_getfield(L, 2, "animations") != LUA_TTABLE) goto error;
    animationsSize = lua_rawlen(L, -1);
    for (int i = 0; i < animationsSize; i++) {
        lua_rawgeti(L, -1, i + 1);
        lua_getfield(L, -1, "name");
        lua_getfield(L, -2, "frames");
        std::vector<unsigned int> anim_frames;
        int frames_size = lua_rawlen(L, -1);
        for (int j = 0; j < frames_size; j++) {
            lua_rawgeti(L, -1, j + 1);
            anim_frames.push_back(luaL_checkinteger(L, -1));
            lua_pop(L, 1);
        }
        std::string anim_name = luaL_checkstring(L, -2);
        e->_Tileset.AnimationsMap[anim_name] = anim_frames;
        lua_pop(L, 3);
    }
    lua_pop(L, 1);
    lua_pushboolean(L, true);
    return 1;
error:
    std::cout << "an error occured" << std::endl;
    lua_pushboolean(L, false);
    return 1;
}

int LuaLevelSerializer::l_CreateEntity(lua_State* L)
{
    int n = lua_gettop(L);

    //int num;
    if (n != 2) {
        std::cout << "2 arguments required l_LoadLevelFromLuaTable" << std::endl;
        lua_pushinteger(L, 0);
        return 1;
    }
    Engine* e = (Engine*)lua_touserdata(L, 1);
    if (!lua_istable(L, 2)) {
        std::cout << "2nd parameter should be a table" << std::endl;
    }
    int components_size = lua_rawlen(L, 2);
    std::vector<ComponentType> components_vec;
    for (int i = 0; i < components_size; i++) {
        lua_rawgeti(L, -1, i+1);
        int val = luaL_checkinteger(L, -1);
        if (val > 0 && val <= NUM_COMPONENTS) {
            components_vec.push_back((ComponentType)val);
        }
        else {
            std::cout << "invalid component type " << val << " passed to l_CreateEntity" << std::endl;
        }
        lua_pop(L, 1);
    }
    lua_pushinteger(L, e->CreateEntity(components_vec));
    return 1;
}

int LuaLevelSerializer::l_SetTransformComponent(lua_State* L)
{
    int n = lua_gettop(L);

    //int num;
    if (n != 3) {
        std::cout << "l_SetTransformComponent takes 3 parameters" << std::endl;
        return 0;
    }
    if (!lua_isuserdata(L, 1)) {
        std::cout << "first parameter should be engine pointer" << std::endl;
        return 0;
    }
    Engine* e = (Engine*)lua_touserdata(L, 1);
    if (!lua_isinteger(L, 2)) {
        std::cout << "second parameter should be the entityID to change" << std::endl;
        return 0;
    }
    EntityID id = lua_tointeger(L, 2);
    if (!lua_istable(L, 3)) {
        std::cout << "second parameter should be the entityID to change" << std::endl;
        return 0;
    }
    if (e->_Components.transforms.find(id) != e->_Components.transforms.end()) {
        Transform& transform = e->_Components.transforms[id];
        lua_getfield(L, -1, "pos");
        lua_getfield(L, -1, "x");
        transform.pos.x = lua_tonumber(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, -1, "y");
        transform.pos.y = lua_tonumber(L, -1);
        lua_pop(L, 2);
        lua_getfield(L, -1, "scale");
        lua_getfield(L, -1, "x");
        transform.scale.x = lua_tonumber(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, -1, "y");
        transform.scale.y = lua_tonumber(L, -1);
        lua_pop(L, 2);
        lua_getfield(L, -1, "rot");
        transform.rot = lua_tonumber(L, -1);
    }
    else {
        std::cout << "you're trying to set a transform that hasn't been registered through createEntity" << std::endl;
    }
    
    

    return 0;
}

int LuaLevelSerializer::l_SetFloorColliderComponent(lua_State* L)
{
    int n = lua_gettop(L);

    //int num;
    if (n != 6) {
        std::cout << "l_SetFloorColliderComponent takes 6 parameters" << std::endl;
        return 0;
    }
    if (!lua_isuserdata(L, 1)) {
        std::cout << "first parameter should be engine pointer" << std::endl;
        return 0;
    }
    Engine* e = (Engine*)lua_touserdata(L, 1);
    if (!lua_isinteger(L, 2)) {
        std::cout << "second parameter should be the entityID to change" << std::endl;
        return 0;
    }
    EntityID id = lua_tointeger(L, 2);
    if (e->_Components.physicses.find(id) != e->_Components.physicses.end()) {
        Physics& phys = e->_Components.physicses[id];
        phys.collider.MinusPixelsTop = luaL_checkinteger(L, 3);
        phys.collider.MinusPixelsBottom = luaL_checkinteger(L, 4);
        phys.collider.MinusPixelsLeft = luaL_checkinteger(L, 5);
        phys.collider.MinusPixelsRight = luaL_checkinteger(L, 6);
    }
    else {
        std::cout << "you're trying to set a physics that hasn't been registered through createEntity" << std::endl;
    }
    return 0;
}

int LuaLevelSerializer::l_SetSpriteComponent(lua_State* L)
{
    int n = lua_gettop(L);

    //int num;
    if (n != 3) {
        std::cout << "l_SetSpriteComponent takes 3 parameters" << std::endl;
        return 0;
    }
    if (!lua_isuserdata(L, 1)) {
        std::cout << "first parameter should be engine pointer" << std::endl;
        return 0;
    }
    Engine* e = (Engine*)lua_touserdata(L, 1);
    if (!lua_isinteger(L, 2)) {
        std::cout << "second parameter should be the entityID to change" << std::endl;
        return 0;
    }
    EntityID id = lua_tointeger(L, 2);
    if (e->_Components.sprites.find(id) != e->_Components.sprites.end()) {
        int spritenum = luaL_checkinteger(L, 3);
        e->_Components.sprites[id].texture = spritenum;
    }
    else {
        std::cout << "you're trying to set a sprite that hasn't been registered through createEntity" << std::endl;
    }
    return 0;
}

int LuaLevelSerializer::l_SetAnimationComponent(lua_State* L)
{
    int n = lua_gettop(L);

    //int num;
    if (n != 3) {
        std::cout << "l_SetSpriteComponent takes 3 parameters" << std::endl;
        return 0;
    }
    if (!lua_isuserdata(L, 1)) {
        std::cout << "first parameter should be engine pointer" << std::endl;
        return 0;
    }
    Engine* e = (Engine*)lua_touserdata(L, 1);
    if (!lua_isinteger(L, 2)) {
        std::cout << "second parameter should be the entityID to change" << std::endl;
        return 0;
    }
    EntityID id = lua_tointeger(L, 2);
    if (e->_Components.animations.find(id) != e->_Components.animations.end()) {
        
        if (!lua_istable(L, 3)) {
            std::cout << "first arg should be a tabke l_SetAnimationComponent" << std::endl;
        }
        lua_getfield(L, -1, "isanimating");
        lua_getfield(L, -2, "name");
        lua_getfield(L, -3, "numframes");
        lua_getfield(L, -4, "timer");
        lua_getfield(L, -5, "shouldloop");
        lua_getfield(L, -6, "onframe");
        lua_getfield(L, -7, "fps");
        Animation& anim = e->_Components.animations[id];
        anim.fps = luaL_checknumber(L, -1);
        anim.onframe = luaL_checkinteger(L, -2);
        anim.shouldLoop = (bool)luaL_checkinteger(L, -3);
        anim.timer = luaL_checknumber(L, -4);
        anim.numframes = luaL_checkinteger(L, -5);
        anim.animationName = luaL_checkstring(L, -6);
        anim.isAnimating = luaL_checkinteger(L, -7);
    }
    else {
        std::cout << "you're trying to set an animation that hasn't been registered through createEntity" << std::endl;
    }
    return 0;
}

int LuaLevelSerializer::l_SetEntityPlayer1(lua_State* L)
{
    int n = lua_gettop(L);
    if (n != 2) {
        std::cout << "l_SetSpriteComponent takes 2 parameters" << std::endl;
        return 0;
    }
    if (!lua_isuserdata(L, 1)) {
        std::cout << "first parameter should be engine pointer" << std::endl;
        return 0;
    }
    Engine* e = (Engine*)lua_touserdata(L, 1);
    if (!lua_isinteger(L, 2)) {
        std::cout << "second parameter should be the entityID to change" << std::endl;
        return 0;
    }
    EntityID id = lua_tointeger(L, 2);
    e->_Player1 = id;
    return 0;
}



bool LuaLevelSerializer::CheckLua(lua_State* L, int returncode)
{
    if (returncode != LUA_OK) {
        std::string errormsg = lua_tostring(L, -1);
        std::cout << errormsg << std::endl;
        return false;
    }
    return true;
}

void LuaLevelSerializer::RegisterLuaAPI()
{
    registerFunction(l_LoadTilesFromImageFile, "C_LoadTilesFromFile");
    registerFunction(l_LoadLevelFromLuaTable, "C_LoadLevel");
    registerFunction(l_CreateEntity, "C_CreateEntity");
    registerFunction(l_SetTransformComponent, "C_SetTransformComponent");
    registerFunction(l_SetSpriteComponent, "C_SetSpriteComponent");
    registerFunction(l_SetAnimationComponent, "C_SetAnimationComponent");
    registerFunction(l_SetFloorColliderComponent, "C_SetFloorColliderComponent");
    registerFunction(l_SetEntityPlayer1, "C_SetEntityPlayer1");
}

inline void LuaLevelSerializer::registerFunction(int(*func)(lua_State* L), std::string func_name)
{
    lua_pushcfunction(_L, func);
    lua_setglobal(_L, func_name.c_str());
}
