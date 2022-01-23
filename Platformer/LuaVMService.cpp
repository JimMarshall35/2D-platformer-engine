#include "LuaVMService.h"
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
#include "Engine.h"

LuaVMService::LuaVMService()
{
    _L = luaL_newstate();
    luaL_openlibs(_L);
    RegisterLuaAPI();
}

void LuaVMService::RegisterLuaAPI()
{
    registerFunction(l_LoadLevelFromLuaTable, "C_LoadLevel");
    registerFunction(l_CreateEntity, "C_CreateEntity");
    registerFunction(l_SetTransformComponent, "C_SetTransformComponent");
    registerFunction(l_SetSpriteComponent, "C_SetSpriteComponent");
    registerFunction(l_SetAnimationComponent, "C_SetAnimationComponent");
    registerFunction(l_SetFloorColliderComponent, "C_SetFloorColliderComponent");
    registerFunction(l_SetEntityPlayer1, "C_SetEntityPlayer1");
    registerFunction(l_SetMovingPlatformComponent, "C_SetMovingPlatformComponent");
    registerFunction(l_LoadTilesetFile, "C_LoadTilesetFile");
    registerFunction(l_LoadTileLayer, "C_LoadTileLayer");
    registerFunction(l_LoadAnimationFrames, "C_LoadAnimationFrames");
    registerFunction(l_GetTileset, "C_GetTileset");
    registerFunction(l_GetEntities, "C_GetEntities");
    registerFunction(l_GetAnimations, "C_GetAnimations");
    registerFunction(l_GetTilelayers, "C_GetTilelayers");
    registerFunction(l_SetVelocity, "C_SetVelocity");
}


#pragma region lua exposed functions

int LuaVMService::l_LoadLevelFromLuaTable(lua_State* L)
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
        lua_rawgeti(L, -1, i + 1); // a tileset file table is top of the stack
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

int LuaVMService::l_CreateEntity(lua_State* L)
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
        lua_rawgeti(L, -1, i + 1);
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

int LuaVMService::l_SetTransformComponent(lua_State* L)
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



int LuaVMService::l_SetFloorColliderComponent(lua_State* L)
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

int LuaVMService::l_SetSpriteComponent(lua_State* L)
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

int LuaVMService::l_SetAnimationComponent(lua_State* L)
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

int LuaVMService::l_SetEntityPlayer1(lua_State* L)
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

int LuaVMService::l_SetMovingPlatformComponent(lua_State* L)
{
    int n = lua_gettop(L);

    //int num;
    if (n != 6) {
        std::cout << "l_SetMovingPlatformComponent takes 3 parameters" << std::endl;
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
    if (e->_Components.moving_platforms.find(id) != e->_Components.moving_platforms.end()) {
        MovingPlatform& mp = e->_Components.moving_platforms[id];
        lua_getfield(L, 3, "x"); // point1 x
        lua_getfield(L, 3, "y"); // point1 y
        mp.point1.x = luaL_checknumber(L, -2);
        mp.point1.y = luaL_checknumber(L, -1);
        lua_getfield(L, 4, "x");
        lua_getfield(L, 4, "y");
        mp.point2.x = luaL_checknumber(L, -2);
        mp.point2.y = luaL_checknumber(L, -1);
        mp.time_period = luaL_checknumber(L, 5);
        mp.timer = luaL_checknumber(L, 6);
    }
    else {
        std::cout << "you're trying to set an moving platform that hasn't been registered through createEntity" << std::endl;
    }
    return 0;
}

int LuaVMService::l_LoadTilesetFile(lua_State* L)
{
    int n = lua_gettop(L);
    if (n != 4) {
        std::cout << "4 arguments required l_LoadTileset, engine*, tileset_w, tileset_h, path" << std::endl;
        lua_pushboolean(L, false);
        return 1;
    }
    Engine* e = (Engine*)lua_touserdata(L, 1);
    e->_Tileset.TileWidthAndHeightPx.x = luaL_checkinteger(L, 2);
    e->_Tileset.TileWidthAndHeightPx.y = luaL_checkinteger(L, 3);
    std::string path = (std::string)luaL_checkstring(L, 4);
    e->_Tileset.LoadTilesFromImgFile(path);
    return 0;
}

int LuaVMService::l_LoadTileLayer(lua_State* L)
{
    int n = lua_gettop(L);
    if (n != 5) {
        std::cout << "5 arguments required l_LoadTileset, engine*, widthtiles, heighttiles, t_type, tiles" << std::endl;
        lua_pushboolean(L, false);
        return 1;
    }
    TileLayer tl;
    Engine* e = (Engine*)lua_touserdata(L, 1);
    tl.SetWidthAndHeight(luaL_checkinteger(L, 2), luaL_checkinteger(L, 3));
    tl.Type = (TileLayerType)luaL_checkinteger(L, 4);

    int tiles_size = lua_rawlen(L, -1);
    tl.Tiles.resize(tiles_size);
    for (int j = 0; j < tiles_size; j++) {
        lua_rawgeti(L, -1, j + 1);
        tl.Tiles[j] = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
    }
    e->_TileLayers.push_back(tl);
}

int LuaVMService::l_LoadAnimationFrames(lua_State* L)
{
    int n = lua_gettop(L);
    if (n != 3) {
        std::cout << "3 arguments required l_LoadAnimationFrames: engine*, name, frames" << std::endl;
        lua_pushboolean(L, false);
        return 1;
    }
    Engine* e = (Engine*)lua_touserdata(L, 1);
    std::string name = (std::string)luaL_checkstring(L, 2);

    int frames_size = lua_rawlen(L, -1);
    std::vector<unsigned int> anim_frames(frames_size);
    for (int j = 0; j < frames_size; j++) {
        lua_rawgeti(L, -1, j + 1);
        anim_frames[j] = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
    }
    e->_Tileset.AnimationsMap[name] = anim_frames;
    return 0;
}

int LuaVMService::l_GetTileset(lua_State* L)
{
    Engine* e = (Engine*)lua_touserdata(L, 1);
    lua_newtable(L);
    for (int i = 0; i < e->_Tileset.FilesList.size(); i++) {
        const auto& file = e->_Tileset.FilesList[i];
        lua_newtable(L);
        lua_pushstring(L, file.path.c_str());
        lua_setfield(L, -2, "path");
        lua_pushinteger(L, file.firstTileID);
        lua_setfield(L, -2, "firstID");
        lua_pushinteger(L, file.TileWidthAndHeightPx.x);
        lua_setfield(L, -2, "tile_width_px");
        lua_pushinteger(L, file.TileWidthAndHeightPx.y);
        lua_setfield(L, -2, "tile_height_px");
        lua_seti(L, -2, i + 1);
    }
    return 1;
}

int LuaVMService::l_GetEntities(lua_State* L)
{
    Engine* e = (Engine*)lua_touserdata(L, 1);

    lua_newtable(L);
    int onEntity = 1;
    const Transform* transform;
    const Physics* phys;
    const Sprite* sprite;
    const Animation* anim;
    const MovingPlatform* mp;
    for (auto& [entityID, val] : e->_Entities) {
        lua_newtable(L);
        lua_newtable(L);
        for (ComponentType t : val) {

            switch (t) {
            case CT_TRANSFORM:
                transform = &e->_Components.transforms[entityID];
                lua_newtable(L);
                // position
                lua_newtable(L);
                lua_pushnumber(L, transform->pos.x);
                lua_setfield(L, -2, "x");
                lua_pushnumber(L, transform->pos.y);
                lua_setfield(L, -2, "y");
                lua_setfield(L, -2, "pos");
                // scale
                lua_newtable(L);
                lua_pushnumber(L, transform->scale.x);
                lua_setfield(L, -2, "x");
                lua_pushnumber(L, transform->scale.y);
                lua_setfield(L, -2, "y");
                lua_setfield(L, -2, "scale");
                // rotation
                lua_pushnumber(L, transform->rot);
                lua_setfield(L, -2, "rot");

                lua_seti(L, -2, (lua_Integer)CT_TRANSFORM);
                break;

            case CT_PHYSICS:
                phys = &e->_Components.physicses[entityID];
                lua_newtable(L);
                // collider
                lua_newtable(L);
                lua_pushnumber(L, phys->collider.MinusPixelsTop);
                lua_seti(L, -2, 1);
                lua_pushnumber(L, phys->collider.MinusPixelsBottom);
                lua_seti(L, -2, 2);
                lua_pushnumber(L, phys->collider.MinusPixelsLeft);
                lua_seti(L, -2, 3);
                lua_pushnumber(L, phys->collider.MinusPixelsRight);
                lua_seti(L, -2, 4);
                lua_setfield(L, -2, "collider");
                // velocity
                lua_newtable(L);
                lua_pushnumber(L, phys->velocity.x);
                lua_setfield(L, -2, "x");
                lua_pushnumber(L, phys->velocity.y);
                lua_setfield(L, -2, "y");
                lua_setfield(L, -2, "velocity");

                lua_seti(L, -2, (lua_Integer)CT_PHYSICS);
                break;

            case CT_HEALTHS:
                lua_newtable(L);
                lua_seti(L, -2, (lua_Integer)CT_HEALTHS);
                break;

            case CT_SPRITE:
                sprite = &e->_Components.sprites[entityID];
                lua_newtable(L);
                lua_pushinteger(L, sprite->texture);
                lua_setfield(L, -2, "texture");
                lua_seti(L, -2, (lua_Integer)CT_SPRITE);
                break;

            case CT_ANIMATION:
                anim = &e->_Components.animations[entityID];
                lua_newtable(L);
                lua_pushnumber(L, anim->timer);
                lua_setfield(L, -2, "timer");

                lua_pushinteger(L, anim->shouldLoop);
                lua_setfield(L, -2, "shouldloop");

                lua_pushnumber(L, anim->fps);
                lua_setfield(L, -2, "fps");

                lua_pushinteger(L, anim->numframes);
                lua_setfield(L, -2, "numframes");

                lua_pushinteger(L, anim->onframe);
                lua_setfield(L, -2, "onframe");

                lua_pushinteger(L, anim->isAnimating);
                lua_setfield(L, -2, "isanimating");

                lua_pushstring(L, anim->animationName.c_str());
                lua_setfield(L, -2, "name");

                lua_seti(L, -2, (lua_Integer)CT_ANIMATION);
                break;

            case CT_PLAYERBEHAVIOR:
                lua_newtable(L);
                lua_seti(L, -2, (lua_Integer)CT_PLAYERBEHAVIOR);
                break;

            case CT_MOVINGPLATFORM:
                mp = &e->_Components.moving_platforms[entityID];
                lua_newtable(L);
                // point 1
                lua_newtable(L);
                lua_pushnumber(L, mp->point1.x);
                lua_setfield(L, -2, "x");
                lua_pushnumber(L, mp->point1.y);
                lua_setfield(L, -2, "y");
                lua_setfield(L, -2, "p1");
                // point 2
                lua_newtable(L);
                lua_pushnumber(L, mp->point2.x);
                lua_setfield(L, -2, "x");
                lua_pushnumber(L, mp->point2.y);
                lua_setfield(L, -2, "y");
                lua_setfield(L, -2, "p2");
                // time period
                lua_pushnumber(L, mp->time_period);
                lua_setfield(L, -2, "time_period");
                // timer
                lua_pushnumber(L, mp->timer);
                lua_setfield(L, -2, "timer");

                lua_seti(L, -2, (lua_Integer)CT_MOVINGPLATFORM);
                break;
            case CT_ENEMYBEHAVIOR:
                lua_newtable(L);
                lua_seti(L, -2, (lua_Integer)CT_ENEMYBEHAVIOR);
                break;
            case CT_COLLECTABLE:
                lua_newtable(L);
                lua_seti(L, -2, (lua_Integer)CT_COLLECTABLE);
                break;
            default:
                break;

            }
        }
        lua_setfield(L, -2, "components");
        lua_seti(L, -2, onEntity);
        onEntity++;
    }
    return 1;
}

int LuaVMService::l_GetAnimations(lua_State* L)
{
    Engine* e = (Engine*)lua_touserdata(L, 1);
    lua_newtable(L);
    int anim_index = 1;
    for (auto& [key, val] : e->_Tileset.AnimationsMap) {
        lua_newtable(L);
        lua_pushstring(L, key.c_str());
        lua_setfield(L, -2, "name");
        lua_newtable(L);
        for (int i = 0; i < val.size(); i++) {
            auto frame = val[i];
            lua_pushinteger(L, frame);
            lua_seti(L, -2, i + 1);
        }
        lua_setfield(L, -2, "frames");
        lua_seti(L, -2, anim_index);
        anim_index++;
    }
    return 1;
}

int LuaVMService::l_GetTilelayers(lua_State* L)
{
    Engine* e = (Engine*)lua_touserdata(L, 1);
    lua_newtable(L);
    for (int i = 0; i < e->_TileLayers.size(); i++) {
        const auto& tl = e->_TileLayers[i];
        lua_newtable(L);
        lua_pushinteger(L, tl.Type);
        lua_setfield(L, -2, "t_type");
        lua_pushinteger(L, tl.GetWidth());
        lua_setfield(L, -2, "widthtiles");
        lua_pushinteger(L, tl.GetHeight());
        lua_setfield(L, -2, "heighttiles");
        lua_newtable(L);
        for (int j = 0; j < tl.Tiles.size(); j++) {
            lua_pushinteger(L, tl.Tiles[j]);
            lua_seti(L, -2, j + 1);
        }
        lua_setfield(L, -2, "tiles");
        lua_seti(L, -2, i + 1);
    }
    return 1;
}

int LuaVMService::l_SetVelocity(lua_State* L)
{
    int n = lua_gettop(L);
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
    glm::vec2 vel;
    lua_getfield(L, 3, "x");
    vel.x = luaL_checknumber(L, -1);
    lua_getfield(L, 3, "y");
    vel.y = luaL_checknumber(L, -1);
    e->_Components.physicses[id].velocity = vel;
    return 0;
}

#pragma endregion

#pragma region public api

bool LuaVMService::DoFile(std::string filePath)
{
    int r = luaL_dofile(_L, filePath.c_str());
    return CheckLua(_L, r);
}

void LuaVMService::SetGlobalPointer(void* lightUserData, std::string name)
{
    lua_pushlightuserdata(_L, lightUserData);
    lua_setglobal(_L, name.c_str());
}

bool LuaVMService::CallFunction1StringParameterNoReturnVal(std::string funcname, std::string param)
{
    lua_getglobal(_L, funcname.c_str());
    lua_pushstring(_L, param.c_str());
    int r = lua_pcall(_L, 1, 0, 0);
    return CheckLua(_L, r);
}

#pragma endregion

#pragma region helpers

bool LuaVMService::CheckLua(lua_State* L, int returncode)
{
    if (returncode != LUA_OK) {
        std::string errormsg = lua_tostring(L, -1);
        std::cout << errormsg << std::endl;
        return false;
    }
    return true;
}


void LuaVMService::registerFunction(int(*func)(lua_State* L), std::string func_name)
{
    lua_pushcfunction(_L, func);
    lua_setglobal(_L, func_name.c_str());
}

#pragma endregion