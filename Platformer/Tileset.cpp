#include "Tileset.h"
#include "stb/stb_image.h"
#include "ErrorHandling.h"



int TileSet::LoadTilesFromPxlData(const unsigned char* data, const unsigned int img_w, const unsigned int img_h, const unsigned int numchannels)
{
	TileSetWidthAndHeightTiles.x = img_w / TileWidthAndHeightPx.x;
	TileSetWidthAndHeightTiles.y = img_h / TileWidthAndHeightPx.y;
	int oldsize = Tiles.size();
	Tiles.resize(oldsize + (TileSetWidthAndHeightTiles.x * TileSetWidthAndHeightTiles.y));
	std::vector<unsigned char> buffer = std::vector<unsigned char>(TileWidthAndHeightPx.x * TileWidthAndHeightPx.y * numchannels);
	int firstID = LastId;
	glBindTexture(GL_TEXTURE_2D, 0);
	
	for (int row = 0; row < TileSetWidthAndHeightTiles.y; row++) {
		for (int col = 0; col < TileSetWidthAndHeightTiles.x; col++) {

			int start_pixel_x = col * TileWidthAndHeightPx.x;
			int start_pixel_y = row * TileWidthAndHeightPx.y;
			unsigned int start_pixel = (start_pixel_y * img_w) + start_pixel_x;

			const unsigned char* src_ptr = data + (start_pixel * numchannels);
			unsigned char* output_ptr = buffer.data();
			for (size_t i = 0; i < TileWidthAndHeightPx.y; i++) {
				memcpy(output_ptr, src_ptr, (TileWidthAndHeightPx.x * numchannels));
				output_ptr += (TileWidthAndHeightPx.x * numchannels);
				src_ptr += img_w * numchannels;
			}
			
			int ind = (row * TileSetWidthAndHeightTiles.x) + col;
			/**/
			glGenTextures(1, &Tiles[oldsize + ind].Texture);
			glBindTexture(GL_TEXTURE_2D, Tiles[oldsize + ind].Texture);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TileWidthAndHeightPx.x, TileWidthAndHeightPx.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data());
			Tiles[oldsize+ind].ID = LastId++;

		}
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	return firstID;
}

bool TileSet::LoadTilesFromImgFile(std::string filePath)
{
	int n;
	const int NUM_CHANNELS = 4;
	int img_w, img_h;
	const unsigned char* data = stbi_load(filePath.c_str(), &img_w, &img_h, &n, NUM_CHANNELS);
	if (data == NULL)
		return false;
	ImgFileTilesetData imgFile;
	imgFile.path = filePath;
	imgFile.firstTileID = LoadTilesFromPxlData(data, img_w, img_h, NUM_CHANNELS);
	imgFile.TileWidthAndHeightPx = TileWidthAndHeightPx;
	FilesList.push_back(imgFile);
	stbi_image_free((void*)data);
}

glm::ivec2 TileSet::GetTileDims(unsigned int tileID)
{
	assert(tileID <= LastId);
	glm::ivec2 returnVal;
	
	for (int i = FilesList.size() - 1; i >= 0; i--) {
		auto firstID = FilesList[i].firstTileID;
		if (firstID  <= tileID) {
			return FilesList[i].TileWidthAndHeightPx;
		}
	}
}

void TileSet::ClearTiles()
{
	FilesList.clear();
	LastId = 1;
	for (auto& t : Tiles) {
		glDeleteTextures(1, &t.Texture);
	}
	Tiles.clear();
}

bool TileSet::LoadNamedSpritesFromFile(std::string filepath, const std::vector<NamedTileData>& sprites)
{
	int n;
	const int NUM_CHANNELS = 4;
	int img_w, img_h;
	const unsigned char* data = stbi_load(filepath.c_str(), &img_w, &img_h, &n, NUM_CHANNELS);
	if (data == NULL)
		return false;

	for (const auto& tiledata : sprites) {
		LoadSprite(data, img_w, img_h, NUM_CHANNELS, tiledata);
	}
	stbi_image_free((void*)data);
}

void TileSet::LoadSprite(const unsigned char* data, const unsigned int img_w, const unsigned int img_h, const unsigned int numchannels, const NamedTileData& spritedata)
{
	std::vector<unsigned char> buffer(spritedata.width * spritedata.height * numchannels);
	unsigned int start_pixel = (spritedata.top * img_w) + spritedata.left;

	const unsigned char* src_ptr = data + (start_pixel * numchannels);
	unsigned char* output_ptr = buffer.data();
	for (size_t i = 0; i < spritedata.height; i++) {
		memcpy(output_ptr, src_ptr, (spritedata.width * numchannels));
		output_ptr += (spritedata.width * numchannels);
		src_ptr += img_w * numchannels;
	}
	unsigned int tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, spritedata.width, spritedata.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data());
	NamedSpritesMap[spritedata.name] = tex;
}

void TileSet::FinishLoading()
{
	std::cout << "FinishLoading not implemented" << std::endl;
}

std::vector<ImgFileTilesetData>& TileSet::GetFilesListRef()
{
	return FilesList;
}

std::vector<Tile>& TileSet::GetTilesRef()
{
	return Tiles;
}

GLuint TileSet::GetTextureByTileID(TileID tileId)
{
	for (const Tile& tile : Tiles)
	{
		if (tile.ID == tileId)
			return tile.Texture;
	}
	return 0;
}

