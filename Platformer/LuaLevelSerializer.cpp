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
    lua_pushlightuserdata(_L, (void*)&engine);
    lua_setglobal(_L, "_engine_");
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
    registerFunction(l_LoadTilesFromImageFile, "LoadTilesFromFile");
    registerFunction(l_LoadLevelFromLuaTable, "LoadLevel");
}

inline void LuaLevelSerializer::registerFunction(int(*func)(lua_State* L), std::string func_name)
{
    lua_pushcfunction(_L, func);
    lua_setglobal(_L, func_name.c_str());
}
