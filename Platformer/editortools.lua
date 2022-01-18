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

function SetupSnake(pos)
	local entityID = C_CreateEntity(C_Engine,{
		ComponentType.CT_TRANSFORM,
		ComponentType.CT_PHYSICS,
		ComponentType.CT_ENEMYBEHAVIOR,
		ComponentType.CT_SPRITE,
		ComponentType.CT_ANIMATION
	})
	C_SetTransformComponent(C_Engine, entityID,{
		pos = pos,
		scale = {x=32.0,y=32.0},
		rot = 0
	})
	C_SetSpriteComponent(C_Engine,entityID,
		207
	)
	C_SetFloorColliderComponent(C_Engine,entityID,
		18, -- top
		0, -- bottom
		8, -- left
		8 -- right
	)
	C_SetAnimationComponent(C_Engine,entityID,{
		isanimating = 1,
		name = "slither",
		numframes = 4,
		timer = 0,
		shouldloop = 1,
		onframe = 0,
		fps = 5
	})
	return entityID
end

EditorTools = {
	{
		name = "Spawn Player",
		handlers = {
			[EditorToolInputRequirements.MouseButton] = function(button, action, mods, imGuiWantsMouse, camera, worldpos)
				if imGuiWantsMouse then 
					return
				end
				if button == GLFW_MOUSE_BUTTON_LEFT and action == GLFW_PRESS then
					SetupSnake(worldpos)
				end
			end,
			[EditorToolInputRequirements.KeyboardButton] = function(key, scancode, action, mods, wantKeyboardInput)
				print("keyboard")
			end,
			[EditorToolInputRequirements.CursorPositionMove] = function(xpos,ypos, imGuiWantsMouse, camera)
				print("move")
			end
		},
		inputRequirements =   EditorToolInputRequirements.MouseButton | EditorToolInputRequirements.CursorPositionMove
	}
}
print("fnbvuhd")