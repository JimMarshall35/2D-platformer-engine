#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <unordered_map>

using TextureID = unsigned int;
using TileID = unsigned int;

struct Tile {
public:
	Tile(TextureID texture, TileID ID);
	Tile() {};
	~Tile();
	TextureID Texture = 0;
	TileID ID = 0;
};

struct ImgFileTilesetData {
	std::string path;
	unsigned int firstTileID;
	glm::ivec2 TileWidthAndHeightPx;
};

struct NamedTileData {
	std::string name;
	int top;
	int left;
	int width;
	int height;
};
class ITileset {
public:
	std::vector<ImgFileTilesetData> FilesList;
	std::vector<Tile> Tiles;
	glm::ivec2 TileWidthAndHeightPx{ 16,16 };
	glm::ivec2 TileSetWidthAndHeightTiles{ 16,16 }; // should be in editorUI
	int LastId = 1;
	std::unordered_map<std::string, std::vector<TextureID>> AnimationsMap;
	std::unordered_map<std::string, TextureID> NamedSpritesMap;

	virtual int LoadTilesFromPxlData(const unsigned char* data, const unsigned int img_w, const unsigned int img_h, const unsigned int numchannels) = 0;
	virtual bool LoadTilesFromImgFile(std::string filePath) = 0;
	virtual bool LoadNamedSpritesFromFile(std::string filepath, const std::vector<NamedTileData>& sprites) = 0;
	virtual glm::ivec2 GetTileDims(unsigned int tileID) = 0;
	virtual void ClearTiles() = 0;
	virtual TextureID GetTextureByTileID(TileID tileId) = 0;
};