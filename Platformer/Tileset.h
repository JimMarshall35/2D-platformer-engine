#pragma once
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <unordered_map>

struct Tile {
public:
	Tile(GLuint texture, unsigned int ID);
	Tile() {};
	~Tile();
	GLuint Texture = 0;
	unsigned int ID = 0;
};

struct ImgFileTilesetData {
	std::string path;
	unsigned int firstTileID;
	glm::ivec2 TileWidthAndHeightPx;
};

struct TileSet { // should be incorporated into renderer
	
	std::vector<Tile> Tiles;
	glm::ivec2 TileWidthAndHeightPx{ 16,16 };
	glm::ivec2 TileSetWidthAndHeightTiles{ 16,16 }; // should be in editorUI
	int LastId = 1;
	std::vector<ImgFileTilesetData> FilesList;

	int LoadTilesFromPxlData(const unsigned char* data, const unsigned int img_w, const unsigned int img_h, const unsigned int numchannels);
	bool LoadTilesFromImgFile(std::string filePath);
	glm::ivec2 GetTileDims(unsigned int tileID);
	std::unordered_map<std::string, std::vector<unsigned int>> AnimationsMap;
	void ClearTiles();

};