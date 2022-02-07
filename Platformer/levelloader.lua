require "persistance"
require "defs"





function OnLoadLevel(path)
	local level = persistence.load(path)
	for i,ts in ipairs(level.tileset) do
		if ts.named_sprites == nil then
			C_LoadTilesetFile(C_Engine, ts.tile_width_px, ts.tile_height_px, ts.path)
		else
			C_LoadNamedTiles(C_Engine, ts.path, ts.named_sprites)
		end
	end
	for i,tl in ipairs(level.tilelayers) do
		C_LoadTileLayer(C_Engine, tl.widthtiles, tl.heighttiles, tl.t_type, tl.tiles)
	end
	for i,anim in ipairs(level.animations) do
		C_LoadAnimationFrames(C_Engine, anim.name, anim.frames)
	end
	local entityIDs = {}
	-- create entities, storing the new ID's in entityIDs
	for i,ent in ipairs(level.entities) do
		local components = {}
		for key, val in pairs(ent.components) do
			table.insert(components, key)
		end

		entityIDs[i] = C_CreateEntity(C_Engine, components)
	end
	-- set the new entities components' initial values
	for i,ent in ipairs(level.entities) do
		local id = entityIDs[i]
		for key, val in pairs(ent.components) do
			if key == ComponentType.CT_TRANSFORM then
				C_SetTransformComponent(C_Engine, id, val)
			elseif key == ComponentType.CT_PHYSICS then
				C_SetFloorColliderComponent(C_Engine, id, val.collider[1],val.collider[2],val.collider[3],val.collider[4])
				C_SetVelocity(C_Engine, id, {x = val.velocity.x, y = val.velocity.y})
			elseif key == ComponentType.CT_HEALTHS then
			elseif key == ComponentType.CT_SPRITE then
				C_SetSpriteComponent(C_Engine, id, val.texture)
			elseif key == ComponentType.CT_ANIMATION then
				C_SetAnimationComponent(C_Engine, id, val)
			elseif key == ComponentType.CT_PLAYERBEHAVIOR then
				C_SetEntityPlayer1(C_Engine,id)
			elseif key == ComponentType.CT_MOVINGPLATFORM then
				C_SetMovingPlatformComponent(C_Engine, id,
					val.p1,
					val.p2,
					val.time_period,
					val.timer
				)
			end
		end
	end
end

function OnSaveLevel(path)
	local level = {}
	level["tileset"] = C_GetTileset(C_Engine)
	level["entities"] = C_GetEntities(C_Engine)
	level["animations"] = C_GetAnimations(C_Engine)
	level["tilelayers"] = C_GetTilelayers(C_Engine)
	persistence.store(path,level)
end

function dump(o)
   if type(o) == 'table' then
      local s = '{ '
      for k,v in pairs(o) do
         if type(k) ~= 'number' then k = '"'..k..'"' end
         s = s .. '['..k..'] = ' .. dump(v) .. ','
      end
      return s .. '} '
   else
      return tostring(o)
   end
end


