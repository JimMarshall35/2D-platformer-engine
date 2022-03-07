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
	C_SetVelocity(C_Engine,entityID,{x=20.0,y=0.0})
	return entityID
end

function SetupCoin(pos)
	local entityID = C_CreateEntity(C_Engine,{
		ComponentType.CT_TRANSFORM,
		ComponentType.CT_SPRITE,
		ComponentType.CT_ANIMATION,
		ComponentType.CT_COLLECTABLE
	})
	C_SetTransformComponent(C_Engine, entityID,{
		pos = pos,
		scale = {x=16.0,y=16.0},
		rot = 0
	})
	C_SetSpriteComponent(C_Engine,entityID,
		128
	)
	C_SetCollectableType(C_Engine, entityID, CollectableType.Coin)
	C_SetCollectableValueInt(C_Engine, entityID, 1)
	C_SetAnimationComponent(C_Engine,entityID,{
		isanimating = 1,
		name = "coin",
		numframes = 7,
		timer = 0,
		shouldloop = 1,
		onframe = 0,
		fps = 10
	})
	return entityID
end


function CreateDynamicBox(pos)
	local entityID = C_CreateEntity(C_Engine,{
		ComponentType.CT_BOX2DPHYSICS,
		ComponentType.CT_TRANSFORM,
		ComponentType.CT_SPRITE
	})
	C_SetSpriteComponent(C_Engine,entityID,10)
	C_SetTransformComponent(C_Engine, entityID,{
		pos = pos,
		scale = {x=16.0,y=16.0},
		rot = 0
	})
	C_CreateBox2DCircleBody(
		C_Engine,
		entityID,
		8.0,
		pos.x,
		pos.y,
		0,
		false
	);
	return entityID
end

function SetupPhysicsStatic(pos)
	local entityID = C_CreateEntity(C_Engine,{
		ComponentType.CT_BOX2DPHYSICS,
		ComponentType.CT_TRANSFORM,
		ComponentType.CT_SPRITE
	})
	C_SetSpriteComponent(C_Engine,entityID,10)
	C_SetTransformComponent(C_Engine, entityID,{
		pos = pos,
		scale = {x=48.0,y=16.0},
		rot = 0
	})
	C_CreateBox2dBoxBody(
		C_Engine,
		entityID,
		24.0,
		8.0,
		pos.x,
		pos.y,
		0,
		true
	);
	return entityID
end


EditorTools = {
	{
		name = "Spawn Snake",
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
				--print("keyboard")
			end,
			[EditorToolInputRequirements.CursorPositionMove] = function(xpos,ypos, imGuiWantsMouse, camera)
				--print("move")
			end
		},
		inputRequirements =   EditorToolInputRequirements.MouseButton | EditorToolInputRequirements.CursorPositionMove
	},
	{
		name = "Spawn Coin",
		handlers = {
			[EditorToolInputRequirements.MouseButton] = function(button, action, mods, imGuiWantsMouse, camera, worldpos)
				if imGuiWantsMouse then 
					return
				end
				if button == GLFW_MOUSE_BUTTON_LEFT and action == GLFW_PRESS then
					SetupCoin(worldpos)
				end
			end,
			[EditorToolInputRequirements.KeyboardButton] = function(key, scancode, action, mods, wantKeyboardInput)
				--print("keyboard")
			end,
			[EditorToolInputRequirements.CursorPositionMove] = function(xpos,ypos, imGuiWantsMouse, camera)
				--print("move")
			end
		},
		inputRequirements =   EditorToolInputRequirements.MouseButton 
	},
	{
		name = "Physics Test Box",
		handlers = {
			[EditorToolInputRequirements.MouseButton] = function(button, action, mods, imGuiWantsMouse, camera, worldpos)
				if imGuiWantsMouse then 
					return
				end
				if button == GLFW_MOUSE_BUTTON_LEFT and action == GLFW_PRESS then
					CreateDynamicBox(worldpos)
				end
			end,
			[EditorToolInputRequirements.KeyboardButton] = function(key, scancode, action, mods, wantKeyboardInput)
				--print("keyboard")
			end,
			[EditorToolInputRequirements.CursorPositionMove] = function(xpos,ypos, imGuiWantsMouse, camera)
				--print("move")
			end
		},
		inputRequirements =   EditorToolInputRequirements.MouseButton 
	},
	{
		name = "Physics Test static Box",
		handlers = {
			[EditorToolInputRequirements.MouseButton] = function(button, action, mods, imGuiWantsMouse, camera, worldpos)
				if imGuiWantsMouse then 
					return
				end
				if button == GLFW_MOUSE_BUTTON_LEFT and action == GLFW_PRESS then
					SetupPhysicsStatic(worldpos)
				end
			end,
			[EditorToolInputRequirements.KeyboardButton] = function(key, scancode, action, mods, wantKeyboardInput)
				--print("keyboard")
			end,
			[EditorToolInputRequirements.CursorPositionMove] = function(xpos,ypos, imGuiWantsMouse, camera)
				--print("move")
			end
		},
		inputRequirements =   EditorToolInputRequirements.MouseButton 
	}
}
