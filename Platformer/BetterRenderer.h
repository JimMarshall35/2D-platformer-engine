#include "BetterTileset.h"
#include "Shader.h"
class BetterRenderer {
public:
	BetterRenderer();
	unsigned int WindowW;
	unsigned int WindowH;
	void DrawAtlasTexture(glm::vec2 pos, glm::vec2 scale, float rotation, unsigned int tile, const Camera2D& cam) const;
private:
	BetterTileset _Tileset;
	void Init();
	unsigned int _AtlasTileVAO;
	Shader _AtlasTileShader;
	
};