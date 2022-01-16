require "defs"

function SetupMovingPlatform(p1,p2, scale,time_period,timer)
	local entityID = C_CreateEntity(C_Engine,{
		ComponentType.CT_TRANSFORM,
		ComponentType.CT_MOVINGPLATFORM,
		ComponentType.CT_PHYSICS,
		ComponentType.CT_SPRITE
	})
	C_SetTransformComponent(C_Engine, entityID,{
		pos = p1,
		scale = scale,
		rot = 0
	})
	C_SetSpriteComponent(C_Engine, entityID,
		9
	)
	C_SetFloorColliderComponent(C_Engine,entityID,0,0,0,0)
	
	C_SetMovingPlatformComponent(C_Engine, entityID,
		p1, 
		p2,
		time_period,
		timer
	)
	
end


function SetupPlayer(inpos, inscale)
	local entityID = C_CreateEntity(C_Engine,{
		ComponentType.CT_TRANSFORM,
		ComponentType.CT_PHYSICS,
		ComponentType.CT_PLAYERBEHAVIOR,
		ComponentType.CT_SPRITE,
		ComponentType.CT_ANIMATION
	})
	C_SetTransformComponent(C_Engine, entityID,{
		pos = inpos,
		scale = inscale,
		rot = 0
	})
	C_SetSpriteComponent(C_Engine,entityID,
		138
	)
	C_SetFloorColliderComponent(C_Engine,entityID,
		11, -- top
		0, -- bottom
		8, -- left
		8 -- right
	)
	C_SetAnimationComponent(C_Engine,entityID,{
		isanimating = 1,
		name = "walk",
		numframes = 4,
		timer = 0,
		shouldloop = 1,
		onframe = 0,
		fps = 10
	})
	C_SetEntityPlayer1(C_Engine,entityID)
	return entityID
end

EditorTools = {
	{
		name = "Spawn Player"
		handlers = {
			[EditorToolInputRequirements.MouseButton] = function(button, action, mods, imGuiWantsMouse, camera)
				print("hellos")
			end
		}
	}
}