#pragma once
#include "Shader.h"
#include "IRenderer2D.h"
class Camera2D;
class Renderer2D : public IRenderer2D
{
public:
	Renderer2D() {  }
	Renderer2D(GLuint w, GLuint h): _WindowW(w), _WindowH(h) { Init(); }
	void DrawWholeTexture(glm::vec2 pos, glm::vec2 scale, float rotation, unsigned int texture, const Camera2D& cam) const;
	void DrawWireframeRect(glm::vec2 pos, glm::vec2 scale, float rotation, glm::vec4 colour, const Camera2D& cam) const;
	void DrawSolidRect(glm::vec2 pos, glm::vec2 scale, float rotation, glm::vec4 colour, const Camera2D& cam) const;
	void SetW(unsigned int newW) { _WindowW = newW; }
	void SetH(unsigned int newH) { _WindowH = newH; }
	void Init();
	unsigned int GetW() const { return _WindowW; };
	unsigned int GetH() const { return _WindowH; };
	//temp 
	unsigned int WindowW = 800;
	unsigned int WindowH = 600;
private:
	GLuint _WireFrameSquareVAO;
	GLuint _TextureVAO;
	GLuint _SolidSquareVAO;
	unsigned int _WindowW = 800;
	unsigned int _WindowH = 600;
	
	Shader _TextureShader;
	Shader _WireframeShader;
};

