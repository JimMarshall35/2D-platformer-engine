#pragma once
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <unordered_map>
#include "ITileset.h"




class TileSet : public TilesetBase {
public:
	int LoadTilesFromPxlData(const unsigned char* data, const unsigned int img_w, const unsigned int img_h, const unsigned int numchannels);
	bool LoadTilesFromImgFile(std::string filePath);
	glm::ivec2 GetTileDims(unsigned int tileID);
	
	void ClearTiles();


	// Inherited via ITileset
	virtual bool LoadNamedSpritesFromFile(std::string filepath, const std::vector<NamedTileData>& sprites) override;
	GLuint GetTextureByTileID(TileID tileId) override;
private:
	std::vector<Tile> Tiles;
	void LoadSprite(const unsigned char* data, const unsigned int img_w, const unsigned int img_h, const unsigned int numchannels,
		const NamedTileData& spritedata);
	

	// Inherited via TilesetBase
	virtual void FinishLoading() override;

	std::vector<ImgFileTilesetData> FilesList;
	// Inherited via TilesetBase
	virtual std::vector<ImgFileTilesetData>& GetFilesListRef() override;


	// Inherited via TilesetBase
	virtual std::vector<Tile>& GetTilesRef() override;

};