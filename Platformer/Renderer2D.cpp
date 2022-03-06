#include "Renderer2D.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtx/transform.hpp>
#include "ErrorHandling.h"
#include "Camera2D.h"
#include <vector>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/random.hpp>
#include "Tileset.h"

#define EXPLODE_QUADS_ROWS 16
#define EXPLODE_QUADS_COLS 16
#define EXPLODE_QUADS_TOTAL EXPLODE_QUADS_ROWS*EXPLODE_QUADS_COLS
void Renderer2D::DrawWholeTexture(glm::vec2 pos, glm::vec2 scale, float rotation, TileID tileID, const Camera2D& cam) const
{
	GLClearErrors();
	
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(pos, 0.0f));
	//model = glm::translate(model, glm::vec3(0.5f * scale.x, 0.5f * scale.y, 0.0f));
	model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::translate(model, glm::vec3(-0.5f * scale.x, -0.5f * scale.y, 0.0f));
	model = glm::scale(model, glm::vec3(scale, 1.0f));

	_TextureShader.use();
	_TextureShader.setMat4("projection", cam.GetProjectionMatrix(_WindowW, _WindowH));
	_TextureShader.setMat4("model", model);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _Tileset->GetTextureByTileID(tileID));

	glBindVertexArray(_TextureVAO);
	glDrawArrays(GL_TRIANGLES,0,6);
	GLPrintErrors("DrawWholeTexture");
}

Renderer2D::Renderer2D() :_Tileset(new TileSet()) 
{  
}

Renderer2D::Renderer2D(GLuint w, GLuint h) : _WindowW(w), _WindowH(h), _Tileset(new TileSet()) 
{ 
	Init(); 
}

void Renderer2D::DrawWireframeRect(glm::vec2 pos, glm::vec2 scale, float rotation, glm::vec4 colour, const Camera2D& cam) const
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(pos, 0.0f));
	model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(scale, 1.0f));

	_WireframeShader.use();
	_WireframeShader.setMat4("projection", cam.GetProjectionMatrix(_WindowW, _WindowH));
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
	_WireframeShader.setMat4("projection", cam.GetProjectionMatrix(_WindowW, _WindowH));
	_WireframeShader.setMat4("model", model);
	_WireframeShader.setVec4("Colour", colour);

	glBindVertexArray(_SolidSquareVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer2D::DrawExplodingTexture(glm::vec2 pos, glm::vec2 scale, float rotation, TileID tileID, const Camera2D& cam, float time) const
{
	GLClearErrors();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(pos, 0.0f));
	model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(scale, 1.0f));

	_ExplodeShader.use();
	_ExplodeShader.setMat4("projection", cam.GetProjectionMatrix(_WindowW, _WindowH));
	_ExplodeShader.setMat4("model", model);
	//setExplodeDirectionsUniform(_ExplodeShader);
	_ExplodeShader.setFloat("time", time);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboHandle);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tileID);
	
	glBindVertexArray(_ExplodeVAO);
	glDrawArrays(GL_TRIANGLES, 0, EXPLODE_QUADS_TOTAL*6);
	GLPrintErrors();
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
	const unsigned int lineVerts[2] = {
		0,1
	};
	GLClearErrors();

	// line 
	unsigned int singleLineVBO;
	glGenVertexArrays(1, &_SingleLineVAO);
	glGenBuffers(1, &singleLineVBO);
	glBindVertexArray(_SingleLineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, singleLineVBO);
	GLPrintErrors("Init");
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineVerts), lineVerts, GL_STATIC_DRAW);
	GLPrintErrors("Init");
	glVertexAttribIPointer(0,1, GL_UNSIGNED_INT,sizeof(unsigned int),NULL);
	GLPrintErrors("Init");
	glEnableVertexAttribArray(0);
	GLPrintErrors("Init");

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

	// exploding texture
	generateExplodeVAO();

	// load shaders
	_TextureShader.LoadFromFile("vert.glsl", "frag.glsl");
	_WireframeShader.LoadFromFile("wireframe_vert.glsl", "wireframe_frag.glsl");
	_ExplodeShader.LoadFromFile("explode_vert.glsl", "explode_frag.glsl");
	_SingleLineShader.LoadFromFile("single_line_vert.glsl", "wireframe_frag.glsl");
	GLPrintErrors("Init");
	
	seedExplodeDirections();
	seedExplodeRotations();
	seedExplodeSpeeds();
	setExplodeShaderUBO(_ExplodeDirections,_ExplodeRotations,_ExplodeSpeeds);
	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
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
	for (int r = 0; r < EXPLODE_QUADS_ROWS; r++) {
		for (int c = 0; c < EXPLODE_QUADS_COLS; c++) {
			// top left
			verts.push_back(ExplodingVert{
				posTopLeft,
				uvTopLeft,
				r* EXPLODE_QUADS_ROWS + c
			});
			// top right
			verts.push_back(ExplodingVert{
				posTopLeft + vec2(1.0 / (float)EXPLODE_QUADS_COLS, 0),
				uvTopLeft + vec2(1.0 / (float)EXPLODE_QUADS_COLS, 0),
				r * EXPLODE_QUADS_ROWS + c
			});
			// bottom left
			verts.push_back(ExplodingVert{
				posTopLeft + vec2(0,-1.0 / (float)EXPLODE_QUADS_ROWS),
				uvTopLeft + vec2(0,-1.0 / (float)EXPLODE_QUADS_ROWS),
				r * EXPLODE_QUADS_ROWS + c
			});
			// top right
			verts.push_back(ExplodingVert{
				posTopLeft + vec2(1.0 / (float)EXPLODE_QUADS_COLS, 0),
				uvTopLeft + vec2(1.0 / (float)EXPLODE_QUADS_COLS, 0),
				r * EXPLODE_QUADS_ROWS + c
			});
			// bottom right
			verts.push_back(ExplodingVert{
				posTopLeft + vec2(1.0 / (float)EXPLODE_QUADS_COLS, -1.0 / (float)EXPLODE_QUADS_ROWS),
				uvTopLeft + vec2(1.0 / (float)EXPLODE_QUADS_COLS, -1.0 / (float)EXPLODE_QUADS_ROWS),
				r * EXPLODE_QUADS_ROWS + c
			});
			// bottom left
			verts.push_back(ExplodingVert{
				posTopLeft + vec2(0,-1.0 / (float)EXPLODE_QUADS_ROWS),
				uvTopLeft + vec2(0,-1.0 / (float)EXPLODE_QUADS_ROWS),
				r * EXPLODE_QUADS_ROWS + c
			});

			posTopLeft.x += 1.0 / (float)EXPLODE_QUADS_COLS;
			uvTopLeft.x += 1.0 / (float)EXPLODE_QUADS_COLS;
		}
		posTopLeft.x = -0.5f;
		uvTopLeft.x = 0.0f;
		posTopLeft.y -= 1.0 / (float)EXPLODE_QUADS_ROWS;
		uvTopLeft.y -= 1.0 / (float)EXPLODE_QUADS_ROWS;
	}
	unsigned int explodeVBO;
	glGenVertexArrays(1, &_ExplodeVAO);
	glGenBuffers(1, &explodeVBO);

	glBindVertexArray(_ExplodeVAO);
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
	for (int i = 0; i < EXPLODE_QUADS_TOTAL; i++) {
		std::string iAsString = std::to_string(i);
		explodeShader.setVec2("directions[" + iAsString + "]", _ExplodeDirections[i]);

	}
}

void Renderer2D::seedExplodeDirections()
{
	_ExplodeDirections.resize(EXPLODE_QUADS_TOTAL);
	for (int i = 0; i < EXPLODE_QUADS_TOTAL; i++) {
		_ExplodeDirections[i] = glm::circularRand(1.0);
	}

}

void Renderer2D::seedExplodeRotations()
{
	_ExplodeRotations.resize(EXPLODE_QUADS_TOTAL);
	for (int i = 0; i < EXPLODE_QUADS_TOTAL; i++) {
		_ExplodeRotations[i] = 0;
	}
}

#define RandomFloatBetween(min,max) ((((float) rand()) / (float) RAND_MAX)*(max-min)) + min
void Renderer2D::seedExplodeSpeeds()
{
	_ExplodeSpeeds.resize(EXPLODE_QUADS_TOTAL);
	for (int i = 0; i < EXPLODE_QUADS_TOTAL; i++) {
		_ExplodeSpeeds[i] = RandomFloatBetween(50.0f,300.0f);
	}
}


void Renderer2D::setExplodeShaderUBO(const std::vector<glm::vec2>& explodeDirections, const std::vector<float>& explodeRotations, const std::vector<float>& explodeSpeeds)
{
	// TODO: make a more general version, templated version of this code for setting ubo's and add it to Shader
	GLClearErrors();
	_ExplodeShader.use();
	auto programHandle = _ExplodeShader.ID;
	GLuint blockIndex = glGetUniformBlockIndex(programHandle, "ParticleAttributesBlock");
	GLint blockSize;
	glGetActiveUniformBlockiv(programHandle, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
	GLbyte* blockBuffer = new GLbyte[blockSize];
#define NUM_PARTICLE_ATTRIBUTES 3
	const GLchar* names[] = { "directions", "rotations", "speeds"};
	GLuint indices[NUM_PARTICLE_ATTRIBUTES];
	glGetUniformIndices(programHandle, NUM_PARTICLE_ATTRIBUTES, names, indices);
	GLint offset[4];
	glGetActiveUniformsiv(programHandle, NUM_PARTICLE_ATTRIBUTES, indices, GL_UNIFORM_OFFSET, offset);
	memcpy(blockBuffer + offset[0], explodeDirections.data(), explodeDirections.size()*sizeof(glm::vec2));
	memcpy(blockBuffer + offset[1], explodeRotations.data(), explodeRotations.size() * sizeof(GLfloat));
	memcpy(blockBuffer + offset[2], explodeSpeeds.data(), explodeSpeeds.size() * sizeof(GLfloat));


	glGenBuffers(1, &uboHandle);
	glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);
	glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboHandle);
	delete[] blockBuffer;
	GLPrintErrors();
}

void Renderer2D::DrawLine(glm::vec2 point1, glm::vec2 point2, glm::vec4 colour, float width, const Camera2D& cam) const
{
	_SingleLineShader.use();
	_SingleLineShader.setMat4("projection", cam.GetProjectionMatrix(_WindowW, _WindowH));
	_SingleLineShader.setVec4("Colour", colour);
	_SingleLineShader.setVec2("point1world", point1);
	_SingleLineShader.setVec2("point2world", point2);
	glLineWidth(width);
	glBindVertexArray(_SingleLineVAO);
	glDrawArrays(GL_LINES, 0, 2);
	glLineWidth(1);
}

