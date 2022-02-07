#pragma once
#include <glm/glm.hpp>
class Camera2D;
class ITileset;
class IRenderer2D {
public:
	virtual void DrawWholeTexture(glm::vec2 pos, glm::vec2 scale, float rotation, unsigned int texture, const Camera2D& cam) const = 0;
	virtual void DrawWireframeRect(glm::vec2 pos, glm::vec2 scale, float rotation, glm::vec4 colour, const Camera2D& cam) const = 0;
	virtual void DrawSolidRect(glm::vec2 pos, glm::vec2 scale, float rotation, glm::vec4 colour, const Camera2D& cam) const = 0;
	virtual void DrawExplodingTexture(glm::vec2 pos, glm::vec2 scale, float rotation, unsigned int texture, const Camera2D& cam, float time) const = 0;
	virtual void SetW(unsigned int w) = 0;
	virtual void SetH(unsigned int h) = 0;
	virtual unsigned int GetW() const  = 0;
	virtual unsigned int GetH() const = 0;
	virtual void Init() = 0;
	virtual ITileset* GetTileset() = 0;
};