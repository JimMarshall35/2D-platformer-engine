#include "Renderer2D.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtx/transform.hpp>
#include "ErrorHandling.h"
#include "Camera2D.h"
void Renderer2D::DrawWholeTexture(glm::vec2 pos, glm::vec2 scale, float rotation, GLuint texture, const Camera2D& cam) const
{
	GLClearErrors();
	
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(pos, 0.0f));
	//model = glm::translate(model, glm::vec3(0.5f * scale.x, 0.5f * scale.y, 0.0f));
	model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::translate(model, glm::vec3(-0.5f * scale.x, -0.5f * scale.y, 0.0f));
	model = glm::scale(model, glm::vec3(scale, 1.0f));

	_TextureShader.use();
	_TextureShader.setMat4("projection", cam.GetProjectionMatrix(WindowW, WindowH));
	_TextureShader.setMat4("model", model);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(_TextureVAO);
	glDrawArrays(GL_TRIANGLES,0,6);
	GLPrintErrors("DrawWholeTexture");
}

void Renderer2D::DrawWireframeRect(glm::vec2 pos, glm::vec2 scale, float rotation, glm::vec4 colour, const Camera2D& cam) const
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(pos, 0.0f));
	model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(scale, 1.0f));

	_WireframeShader.use();
	_WireframeShader.setMat4("projection", cam.GetProjectionMatrix(WindowW, WindowH));
	_WireframeShader.setMat4("model", model);
	_WireframeShader.setVec4("Colour", colour);

	glBindVertexArray(_WireFrameSquareVAO);
	glDrawArrays(GL_LINE_LOOP, 0, 4);

}

void Renderer2D::DrawSolidRect(glm::vec2 pos, glm::vec2 scale, float rotation, glm::vec4 colour, const Camera2D& cam) const
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(pos, 0.0f));
	model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(scale, 1.0f));

	_WireframeShader.use();
	_WireframeShader.setMat4("projection", cam.GetProjectionMatrix(WindowW, WindowH));
	_WireframeShader.setMat4("model", model);
	_WireframeShader.setVec4("Colour", colour);

	glBindVertexArray(_SolidSquareVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer2D::Init()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	const float wireframeUnitSquareVerts[8] = {
		-0.5f,-0.5f, // bottom left

		-0.5f,0.5f, // top left
		0.5f,0.5f, // top right
		0.5f,-0.5f, // bottom right
		
	};
	const float unitSquareVerts[24] = {
		-0.5f, 0.5f, 0.0f, 1.0f, // top left
		0.5f, 0.5f, 1.0f, 1.0f,  // top right
		-0.5f, -0.5f, 0.0f, 0.0f,// bottom left

		0.5f, 0.5f, 1.0f, 1.0f, // top right
		0.5f, -0.5f, 1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f, 0.0f, // bottom left
	};
	GLClearErrors();

	// textured square
	unsigned int textureVBO;
	glGenVertexArrays(1, &_TextureVAO);
	glGenBuffers(1, &textureVBO);
	glBindVertexArray(_TextureVAO);
	glBindBuffer(GL_ARRAY_BUFFER, textureVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(unitSquareVerts), unitSquareVerts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	// wireframe square
	unsigned int wireframeUnitSquareVBO;
	glGenVertexArrays(1, &_WireFrameSquareVAO);
	glGenBuffers(1, &wireframeUnitSquareVBO);
	glBindVertexArray(_WireFrameSquareVAO);
	glBindBuffer(GL_ARRAY_BUFFER, wireframeUnitSquareVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(wireframeUnitSquareVerts), wireframeUnitSquareVerts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// solid square
	unsigned int solidSquareVBO;
	glGenVertexArrays(1, &_SolidSquareVAO);
	glGenBuffers(1, &solidSquareVBO);
	glBindVertexArray(_SolidSquareVAO);
	glBindBuffer(GL_ARRAY_BUFFER, solidSquareVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(unitSquareVerts), unitSquareVerts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4* sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	_TextureShader.LoadFromFile("vert.glsl", "frag.glsl");
	_WireframeShader.LoadFromFile("wireframe_vert.glsl", "wireframe_frag.glsl");
	GLPrintErrors("Init");
}
