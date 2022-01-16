#include "LuaLevelSerializer.h"
#include <iostream>

#include "Engine.h"
#include "LuaVMService.h"

void LuaLevelSerializer::Serialize(const Engine& engine, std::string filePath)
{
    _VM->CallFunction1StringParameterNoReturnVal("OnSaveLevel", filePath);
}

bool LuaLevelSerializer::DeSerialize(Engine& engine, std::string filePath)
{
    std::vector<EntityID> ids;
    for (auto const& element : engine._Entities) {
        ids.push_back(element.first);
    }
    for (EntityID id : ids) {
        engine.DeleteEntity(id);
    }
    engine._TileLayers.clear();
    engine._Tileset.ClearTiles();
    engine._Tileset.AnimationsMap.clear();
    _VM->SetGlobalPointer((void*)&engine, "C_Engine");

    bool r = _VM->CallFunction1StringParameterNoReturnVal("OnLoadLevel", filePath);
    return r;//CheckLua(_L, r);
}

LuaLevelSerializer::LuaLevelSerializer(LuaVMService* vmService)
    :_VM(vmService)
{
    _VM->DoFile("levelloader.lua");
}

LuaLevelSerializer::~LuaLevelSerializer()
{
}



