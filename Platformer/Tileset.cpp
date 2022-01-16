#include "Tileset.h"
#include "stb/stb_image.h"
#include "ErrorHandling.h"
Tile::Tile(GLuint texture, unsigned int id)
{
	Texture = texture;
	ID = id;
}

Tile::~Tile()
{
	//glDeleteTextures(1, &Texture);
}

int TileSet::LoadTilesFromPxlData(const unsigned char* data, const unsigned int img_w, const unsigned int img_h, const unsigned int numchannels)
{
	TileSetWidthAndHeightTiles.x = img_w / TileWidthAndHeightPx.x;
	TileSetWidthAndHeightTiles.y = img_h / TileWidthAndHeightPx.y;
	int oldsize = Tiles.size();
	Tiles.resize(oldsize + (TileSetWidthAndHeightTiles.x * TileSetWidthAndHeightTiles.y));
	unsigned char* buffer = new unsigned char[TileWidthAndHeightPx.x * TileWidthAndHeightPx.y * numchannels];
	int firstID = LastId;
	glBindTexture(GL_TEXTURE_2D, 0);
	
	for (int row = 0; row < TileSetWidthAndHeightTiles.y; row++) {
		for (int col = 0; col < TileSetWidthAndHeightTiles.x; col++) {

			int start_pixel_x = col * TileWidthAndHeightPx.x;
			int start_pixel_y = row * TileWidthAndHeightPx.y;
			unsigned int start_pixel = (start_pixel_y * img_w) + start_pixel_x;

			const unsigned char* src_ptr = data + (start_pixel * numchannels);
			unsigned char* output_ptr = buffer;
			for (size_t i = 0; i < TileWidthAndHeightPx.y; i++) {
				memcpy(output_ptr, src_ptr, (TileWidthAndHeightPx.x * numchannels));
				output_ptr += (TileWidthAndHeightPx.x * numchannels);
				src_ptr += img_w * numchannels;
			}
			
			int ind = (row * TileSetWidthAndHeightTiles.x) + col;
			glGenTextures(1, &Tiles[oldsize + ind].Texture);
			glBindTexture(GL_TEXTURE_2D, Tiles[oldsize + ind].Texture);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TileWidthAndHeightPx.x, TileWidthAndHeightPx.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
			Tiles[oldsize+ind].ID = LastId++;

		}
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	delete[] buffer;
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

