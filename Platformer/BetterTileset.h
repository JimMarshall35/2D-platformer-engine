#pragma once
#include "ITileset.h"
#include <glm/glm.hpp>

class Camera2D;
struct FileTilesetData {
    std::string path;
    std::vector<std::vector<unsigned char>> tiles;
    glm::ivec2 tilePixelDims;
};
struct AtlasTile {
    glm::vec2 uvTopLeft;
    glm::vec2 uvBottomRight;
    glm::vec2 pixelsDims;
};
class IBetterTileset {

};
class BetterTileset :
    public IBetterTileset
{
public:
    bool LoadTilesetFile(unsigned int widthOfTile, 
        unsigned int heightOfTile,
        std::string filePath);
    void BuildAtlas();
    const AtlasTile& GetAtlasTile(unsigned int index) const;
    unsigned int GetAtlasTextureID() const { return _AtlasTexture; }
    glm::vec2 GetAtlasWidthAndHeight();
private:
    FileTilesetData& GetWidestTile();
    size_t GetAtlasBufferSize(const FileTilesetData& widestTilesetFile, size_t& atlasHeightPixels);
    void BlitFileToAtlasOnePerRow(unsigned char* writePointer, const FileTilesetData& tilesetFile, glm::vec2& lastTileBottomRightUV, glm::vec2 lastTileTopLeftUv);
    void BlitFileIntoAtlasMultipleTilesPerRow(unsigned char* writePointer, const FileTilesetData& tilesetFile, glm::vec2& lastTileBottomRightUV, glm::vec2 lastTileTopLeftUv, size_t numPerRow);

    std::vector<FileTilesetData> _FileTilesetDatas;
    TextureID _Atlas = 0;
    std::vector<AtlasTile> _Tiles;
    unsigned int _AtlasTexture;

    size_t _AtlasHeightPixels;
    size_t _AtlasWidthPixels;
    size_t _AtlasWidthBytes;
    std::vector<unsigned char> _AtlasBuffer;
};

