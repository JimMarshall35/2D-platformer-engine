#include "Renderer2D.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtx/transform.hpp>
#include "ErrorHandling.h"
#include "Camera2D.h"
#include <vector>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/random.hpp>

#define EXPLODE_QUAD_ROWS 10
#define EXPLODE_QUAD_COLS 10
#define EXPLODE_QUAD_TOTAL EXPLODE_QUAD_ROWS*EXPLODE_QUAD_COLS
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

void Renderer2D::DrawExplodingTexture(glm::vec2 pos, glm::vec2 scale, float rotation, unsigned int texture, const Camera2D& cam, float time) const
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(pos, 0.0f));
	model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(scale, 1.0f));

	_ExplodeShader.use();
	_ExplodeShader.setMat4("projection", cam.GetProjectionMatrix(WindowW, WindowH));
	_ExplodeShader.setMat4("model", model);
	setExplodeDirectionsUniform(_ExplodeShader);
	_ExplodeShader.setFloat("time", time);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	
	glBindVertexArray(_SolidSquareVAO);
	glDrawArrays(GL_TRIANGLES, 0, EXPLODE_QUAD_TOTAL*6);
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
	_ExplodeShader.LoadFromFile("explode_vert.glsl", "explode_frag.glsl");
	GLPrintErrors("Init");
	generateExplodeVAO();
	seedExplodeDirections();
	
}

struct ExplodingVert {
	glm::vec2 pos;
	glm::vec2 uv;
	int quad_index;
};
void Renderer2D::generateExplodeVAO()
{
	using namespace glm;
	
	vec2 posTopLeft(-0.5, 0.5);
	vec2 uvTopLeft(0.0, 1.0);
	std::vector<ExplodingVert> verts;
	//int quad_index = 0;
	for (int r = 0; r < EXPLODE_QUAD_ROWS; r++) {
		for (int c = 0; c < EXPLODE_QUAD_COLS; c++) {
			// top left
			verts.push_back(ExplodingVert{
				posTopLeft,
				uvTopLeft,
				r* EXPLODE_QUAD_ROWS + c
			});
			// top right
			verts.push_back(ExplodingVert{
				posTopLeft + vec2(1.0 / (float)EXPLODE_QUAD_COLS, 0),
				uvTopLeft + vec2(1.0 / (float)EXPLODE_QUAD_COLS, 0),
				r * EXPLODE_QUAD_ROWS + c
			});
			// bottom left
			verts.push_back(ExplodingVert{
				posTopLeft + vec2(0,-1.0 / (float)EXPLODE_QUAD_ROWS),
				uvTopLeft + vec2(0,-1.0 / (float)EXPLODE_QUAD_ROWS),
				r * EXPLODE_QUAD_ROWS + c
			});
			// top right
			verts.push_back(ExplodingVert{
				posTopLeft + vec2(1.0 / (float)EXPLODE_QUAD_COLS, 0),
				uvTopLeft + vec2(1.0 / (float)EXPLODE_QUAD_COLS, 0),
				r * EXPLODE_QUAD_ROWS + c
			});
			// bottom right
			verts.push_back(ExplodingVert{
				posTopLeft + vec2(1.0 / (float)EXPLODE_QUAD_COLS, -1.0 / (float)EXPLODE_QUAD_ROWS),
				uvTopLeft + vec2(1.0 / (float)EXPLODE_QUAD_COLS, -1.0 / (float)EXPLODE_QUAD_ROWS),
				r * EXPLODE_QUAD_ROWS + c
			});
			// bottom left
			verts.push_back(ExplodingVert{
				posTopLeft + vec2(0,-1.0 / (float)EXPLODE_QUAD_ROWS),
				uvTopLeft + vec2(0,-1.0 / (float)EXPLODE_QUAD_ROWS),
				r * EXPLODE_QUAD_ROWS + c
			});

			posTopLeft.x += 1.0 / (float)EXPLODE_QUAD_COLS;
			uvTopLeft.x += 1.0 / (float)EXPLODE_QUAD_COLS;
		}
		posTopLeft.x = -0.5f;
		uvTopLeft.x = 0.0f;
		posTopLeft.y -= 1.0 / (float)EXPLODE_QUAD_ROWS;
		uvTopLeft.y -= 1.0 / (float)EXPLODE_QUAD_ROWS;
	}
	unsigned int explodeVBO;
	glGenVertexArrays(1, &_ExplodeVAO);
	glGenBuffers(1, &explodeVBO);

	glBindVertexArray(_SolidSquareVAO);
	glBindBuffer(GL_ARRAY_BUFFER, explodeVBO);

	glBufferData(GL_ARRAY_BUFFER, verts.size()*sizeof(ExplodingVert), verts.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ExplodingVert), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ExplodingVert), (void*)offsetof(ExplodingVert,uv));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(ExplodingVert), (void*)offsetof(ExplodingVert, quad_index));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Renderer2D::setExplodeDirectionsUniform(const Shader& explodeShader) const
{
	for (int i = 0; i < EXPLODE_QUAD_TOTAL; i++) {
		std::string iAsString = std::to_string(i);
		explodeShader.setVec2("directions[" + iAsString + "]", _ExplodeDirections[i]);

	}
	//std::cout << "this is my united states of whatever" << std::endl;
}

void Renderer2D::seedExplodeDirections()
{
	_ExplodeDirections.resize(EXPLODE_QUAD_TOTAL);
	for (int i = 0; i < EXPLODE_QUAD_TOTAL; i++) {
		_ExplodeDirections[i] = glm::circularRand(1.0);
	}
}
