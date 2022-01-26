#pragma once
#include "Shader.h"
#include "IRenderer2D.h"
#include <vector>

class Camera2D;
class Renderer2D : public IRenderer2D
{
public:
	Renderer2D() {  }
	Renderer2D(GLuint w, GLuint h): _WindowW(w), _WindowH(h) { Init(); }
	void DrawWholeTexture(glm::vec2 pos, glm::vec2 scale, float rotation, unsigned int texture, const Camera2D& cam) const;
	void DrawWireframeRect(glm::vec2 pos, glm::vec2 scale, float rotation, glm::vec4 colour, const Camera2D& cam) const;
	void DrawSolidRect(glm::vec2 pos, glm::vec2 scale, float rotation, glm::vec4 colour, const Camera2D& cam) const;
	void DrawExplodingTexture(glm::vec2 pos, glm::vec2 scale, float rotation, unsigned int texture, const Camera2D& cam, float delta_t) const;
	void SetW(unsigned int newW) { _WindowW = newW; }
	void SetH(unsigned int newH) { _WindowH = newH; }
	void Init();
	unsigned int GetW() const { return _WindowW; };
	unsigned int GetH() const { return _WindowH; };
	//temp 
	unsigned int WindowW = 800;
	unsigned int WindowH = 600;

private:
	void generateExplodeVAO();
	void setExplodeDirectionsUniform(const Shader& explodeShader) const;
	void seedExplodeDirections();
	void seedExplodeRotations();
	void seedExplodeSpeeds();
private:
	GLuint _WireFrameSquareVAO;
	GLuint _TextureVAO;
	GLuint _SolidSquareVAO;
	GLuint _ExplodeVAO;
	GLuint uboHandle;
	unsigned int _WindowW = 800;
	unsigned int _WindowH = 600;
	
	Shader _TextureShader;
	Shader _WireframeShader;
	Shader _ExplodeShader;
	void setExplodeShaderUBO(const std::vector<glm::vec2>& explodeDirections, const std::vector<float>& explodeRotations, const std::vector<float>& explodeSpeeds);
	std::vector<glm::vec2> _ExplodeDirections;
	std::vector<float> _ExplodeRotations;
	std::vector<float> _ExplodeSpeeds;
};

