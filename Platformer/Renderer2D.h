#pragma once
#include "Shader.h"
#include "IRenderer2D.h"
#include <vector>
#include "ITileset.h"

class Camera2D;
class Renderer2D : public IRenderer2D
{
public:
	Renderer2D();
	Renderer2D(GLuint w, GLuint h);
	~Renderer2D() { delete _Tileset; }
	// Inherited via IRenderer2D
	virtual void DrawLine(glm::vec2 point1, glm::vec2 point2, glm::vec4 colour, float width, const Camera2D& cam) const override;
	void DrawWholeTexture(glm::vec2 pos, glm::vec2 scale, float rotation, unsigned int texture, const Camera2D& cam) const override;
	void DrawWireframeRect(glm::vec2 pos, glm::vec2 scale, float rotation, glm::vec4 colour, const Camera2D& cam) const override;
	void DrawSolidRect(glm::vec2 pos, glm::vec2 scale, float rotation, glm::vec4 colour, const Camera2D& cam) const override;
	void DrawExplodingTexture(glm::vec2 pos, glm::vec2 scale, float rotation, unsigned int texture, const Camera2D& cam, float delta_t) const override;
	virtual ITileset* GetTileset() override { return _Tileset; };
	void SetW(unsigned int newW) { _WindowW = newW; }
	void SetH(unsigned int newH) { _WindowH = newH; }
	void Init();
	unsigned int GetW() const { return _WindowW; };
	unsigned int GetH() const { return _WindowH; };
private:
	void generateExplodeVAO();
	void setExplodeDirectionsUniform(const Shader& explodeShader) const;
	void seedExplodeDirections();
	void seedExplodeRotations();
	void seedExplodeSpeeds();
private:
	ITileset* _Tileset;
	GLuint _WireFrameSquareVAO;
	GLuint _TextureVAO;
	GLuint _SolidSquareVAO;
	GLuint _ExplodeVAO;
	GLuint _SingleLineVAO;
	GLuint uboHandle;
	unsigned int _WindowW = 800;
	unsigned int _WindowH = 600;
	
	Shader _TextureShader;
	Shader _WireframeShader;
	Shader _ExplodeShader;
	Shader _SingleLineShader;
	void setExplodeShaderUBO(const std::vector<glm::vec2>& explodeDirections, const std::vector<float>& explodeRotations, const std::vector<float>& explodeSpeeds);
	std::vector<glm::vec2> _ExplodeDirections;
	std::vector<float> _ExplodeRotations;
	std::vector<float> _ExplodeSpeeds;

	

	

};

