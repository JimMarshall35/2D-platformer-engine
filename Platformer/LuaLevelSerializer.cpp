#include "LuaLevelSerializer.h"
#include <iostream>

#include "Engine.h"
#include "LuaVMService.h"
#include "IRenderer2D.h"
#include "ITileset.h"

void LuaLevelSerializer::Serialize(const Engine& engine, std::string filePath)
{
    _VM->CallFunction1StringParameterNoReturnVal("OnSaveLevel", filePath);
}

bool LuaLevelSerializer::DeSerialize(Engine& engine, std::string filePath)
{
    ITileset* tileset = engine.Renderer->GetTileset();
    std::vector<EntityID> ids;
    for (auto const& element : engine._Entities) {
        ids.push_back(element.first);
    }
    for (EntityID id : ids) {
        engine.DeleteEntity(id);
    }
    engine.TileLayers.clear();
    tileset->ClearTiles();
    tileset->AnimationsMap.clear();
    _VM->SetGlobalPointer((void*)&engine, "C_Engine");

    bool r = _VM->CallFunction1StringParameterNoReturnVal("OnLoadLevel", filePath);
    return r;
}

LuaLevelSerializer::LuaLevelSerializer(LuaVMService* vmService)
    :_VM(vmService)
{
    _VM->DoFile("levelloader.lua");
}

LuaLevelSerializer::~LuaLevelSerializer()
{
}



