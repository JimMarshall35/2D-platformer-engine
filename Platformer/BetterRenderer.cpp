#include "BetterRenderer.h"
#include "ErrorHandling.h"
#include <glm/gtx/transform.hpp>
#include "Camera2D.h"


BetterRenderer::BetterRenderer()
{
	
	_Tileset.LoadTilesetFile(32, 32, "C:\\Users\\james.marshall\\source\\repos\\Platformer\\Platformer\\characters.png");
	_Tileset.LoadTilesetFile(128, 128, "C:\\Users\\james.marshall\\source\\repos\\Platformer\\Platformer\\sheet.png");
	_Tileset.BuildAtlas();
	Init();
}

struct AtlasTileVert {
	glm::vec2 pos;
	GLuint uvIndex;
};
void BetterRenderer::Init()
{
	GLClearErrors();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	static const AtlasTileVert atlasTileVertices[]{
		AtlasTileVert{glm::vec2(-0.5f, 0.5f), 0}, // top left
		AtlasTileVert{glm::vec2(0.5f, 0.5f),  1}, // top right
		AtlasTileVert{glm::vec2(-0.5f, -0.5f),3}, // bottom left

		AtlasTileVert{glm::vec2(0.5f, 0.5f),  1}, // top right
		AtlasTileVert{glm::vec2(0.5f, -0.5f), 2}, // bottom right
		AtlasTileVert{glm::vec2(-0.5f, -0.5f),3}, // bottom left
	};

	unsigned int atlasTileVBO;
	glGenVertexArrays(1, &_AtlasTileVAO);
	glGenBuffers(1, &atlasTileVBO);
	glBindVertexArray(_AtlasTileVAO);
	glBindBuffer(GL_ARRAY_BUFFER, atlasTileVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(atlasTileVertices), atlasTileVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(AtlasTileVert), (void*)offsetof(AtlasTileVert, pos));
	glEnableVertexAttribArray(0);
	auto indexOffset = offsetof(AtlasTileVert, uvIndex);
	glVertexAttribPointer(1, 1, GL_INT, GL_FALSE, sizeof(AtlasTileVert), (void*)offsetof(AtlasTileVert, uvIndex));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	_AtlasTileShader.LoadFromFile("AtlasTileVert.glsl", "AtlasTileFrag.glsl");
	GLPrintErrors("init");
}

void BetterRenderer::DrawAtlasTexture(glm::vec2 pos, glm::vec2 scale, float rotation, unsigned int tile, const Camera2D& cam) const
{
	GLClearErrors();

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(pos, 0.0f));
	model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(scale, 1.0f));

	const AtlasTile& atlasTile = _Tileset.GetAtlasTile(tile);
	glm::vec2 uvs[4]{
		atlasTile.uvTopLeft,                                                 // top left
		glm::vec2(atlasTile.uvBottomRight.x, atlasTile.uvTopLeft.y),         // top right
		atlasTile.uvBottomRight,                                             // bottom right
		glm::vec2(atlasTile.uvTopLeft.x, atlasTile.uvBottomRight.y)          // bottom left
	};

	_AtlasTileShader.use();
	_AtlasTileShader.setMat4("projection", cam.GetProjectionMatrix(WindowW, WindowH));
	_AtlasTileShader.setMat4("model", model);
	//temporary hack - use ubo
	for (int i = 0; i < 4; i++) {
		std::string uniformName = "uvValues[" + std::to_string(i) + "]";
		_AtlasTileShader.setVec2(uniformName, uvs[i]);
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _Tileset.GetAtlasTextureID());

	glBindVertexArray(_AtlasTileVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	GLPrintErrors("DrawWholeTexture");
}
