#include "BetterTileset.h"
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include <glad/glad.h>
const int NUM_CHANNELS = 4;


bool BetterTileset::LoadTilesetFile(unsigned int widthOfTile, unsigned int heightOfTile, std::string filePath)
{
	const int BYTES_IN_TILE_ROW = widthOfTile * NUM_CHANNELS;
	const int BYTES_IN_TILE = heightOfTile * BYTES_IN_TILE_ROW;
	
	int img_w, img_h;
	int n;
	const unsigned char* data = stbi_load(filePath.c_str(), &img_w, &img_h, &n, NUM_CHANNELS);
	if (data == NULL)
		return false;
	const int BYTES_IN_IMAGE_ROW = img_w * NUM_CHANNELS;

	FileTilesetData tilesetData;
	tilesetData.path = filePath;
	tilesetData.tilePixelDims.x = widthOfTile;
	tilesetData.tilePixelDims.y = heightOfTile;
	for (int row = 0; row < img_h / (heightOfTile); row++) {
		for (int col = 0; col < img_w / (widthOfTile); col++){
			//int start_pixel_x = y 
			int start_pixel_x = col * widthOfTile;
			int start_pixel_y = row * heightOfTile;
			unsigned int start_pixel = (start_pixel_y * img_w) + start_pixel_x;
			const unsigned char* src_ptr = data + (start_pixel * NUM_CHANNELS);
			auto tileData = std::vector<unsigned char>(BYTES_IN_TILE);
			unsigned char* output_ptr = tileData.data();
			for (size_t i = 0; i < heightOfTile; i++) {
				memcpy(output_ptr, src_ptr, (widthOfTile * NUM_CHANNELS));
				output_ptr += (widthOfTile * NUM_CHANNELS);
				//if(src_ptr < 
				src_ptr += img_w * NUM_CHANNELS;
			}
			
			tilesetData.tiles.push_back(tileData);
			//stbi_write_png("tile_dump.png", widthOfTile, heightOfTile, NUM_CHANNELS, tileData.data(), widthOfTile*NUM_CHANNELS);
			
		}
	}
	stbi_image_free((void*)data);
	_FileTilesetDatas.push_back(tilesetData);
}


void BetterTileset::BuildAtlas()
{
	auto& widestTileBytes = GetWidestTile();
	_AtlasHeightPixels = 0;
	auto buffersize = GetAtlasBufferSize(widestTileBytes, _AtlasHeightPixels);
	
	_AtlasWidthPixels = widestTileBytes.tilePixelDims.x;
	_AtlasWidthBytes = _AtlasWidthPixels * NUM_CHANNELS;
	_AtlasBuffer.resize(buffersize);
	unsigned char* writePointer = _AtlasBuffer.data();
	glm::vec2 lastTileTopLeftUv;
	lastTileTopLeftUv.x = 0.0;
	lastTileTopLeftUv.y = 1.0;

	glm::vec2 lastTileBottomRightUV;
	lastTileBottomRightUV.x = 0.0;
	lastTileBottomRightUV.x = 1.0;

	for (const FileTilesetData& tilesetFile : _FileTilesetDatas) {
		int numPerRow = _AtlasWidthPixels / tilesetFile.tilePixelDims.x;
		
		if (numPerRow == 1) {
			BlitFileToAtlasOnePerRow(writePointer, tilesetFile, lastTileBottomRightUV, lastTileBottomRightUV);
		}
		else {
			BlitFileIntoAtlasMultipleTilesPerRow(writePointer, tilesetFile, lastTileBottomRightUV, lastTileBottomRightUV, numPerRow);
		}
	}
	stbi_write_png("atlas_dump.png", _AtlasWidthPixels, _AtlasHeightPixels, NUM_CHANNELS, _AtlasBuffer.data(), _AtlasWidthBytes);
	glGenTextures(1, &_AtlasTexture);
	glBindTexture(GL_TEXTURE_2D, _AtlasTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _AtlasWidthPixels, _AtlasHeightPixels, 0, GL_RGBA, GL_UNSIGNED_BYTE, _AtlasBuffer.data());
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

}

const AtlasTile& BetterTileset::GetAtlasTile(unsigned int index) const
{
	return _Tiles[index];
}

glm::vec2 BetterTileset::GetAtlasWidthAndHeight()
{
	return glm::vec2(_AtlasWidthPixels, _AtlasHeightPixels);
}

FileTilesetData& BetterTileset::GetWidestTile()
{
	int largestWidth = 0;
	size_t largestWidthIndex = 0;
	
	for (int i = 0; i < _FileTilesetDatas.size(); i++) {
		auto& ttb = _FileTilesetDatas[i];
		if (ttb.tilePixelDims.x > largestWidth) {
			largestWidth = ttb.tilePixelDims.x;
			largestWidthIndex = i;
		}
	}
	return _FileTilesetDatas[largestWidthIndex];
}

size_t BetterTileset::GetAtlasBufferSize(const FileTilesetData& widestTilesetFile, size_t& atlasHeightPixels)
{
	size_t atlasWidthPixels = widestTilesetFile.tilePixelDims.x;
	atlasHeightPixels = 0;
	for (int i = 0; i < _FileTilesetDatas.size(); i++) {
		auto& ttb = _FileTilesetDatas[i];
		int numPerRow = atlasWidthPixels / ttb.tilePixelDims.x;
		atlasHeightPixels += (ttb.tilePixelDims.y * ttb.tiles.size()) / numPerRow;

	}
	return atlasHeightPixels * atlasWidthPixels * NUM_CHANNELS;
}

void BetterTileset::BlitFileToAtlasOnePerRow(unsigned char* writePointer, const FileTilesetData& tilesetFile, glm::vec2& lastTileBottomRightUV, glm::vec2 lastTileTopLeftUv)
{
	const size_t TILE_WIDTH_BYTES = tilesetFile.tilePixelDims.x * NUM_CHANNELS;
	for (const auto& tileBytes : tilesetFile.tiles) {
		for (int i = 0; i < tilesetFile.tilePixelDims.y; i++) {
			memcpy(writePointer, tileBytes.data() + i * TILE_WIDTH_BYTES, TILE_WIDTH_BYTES);
			if (writePointer + _AtlasWidthBytes < _AtlasBuffer.data() + _AtlasBuffer.size())
				writePointer += _AtlasWidthBytes;
		}
		glm::vec2 bottomRightUv;
		bottomRightUv.x = lastTileTopLeftUv.x + ((float)tilesetFile.tilePixelDims.x / (float)_AtlasWidthPixels);
		bottomRightUv.y = lastTileTopLeftUv.y - ((float)tilesetFile.tilePixelDims.y / (float)_AtlasHeightPixels);
		_Tiles.push_back(AtlasTile{ lastTileTopLeftUv, bottomRightUv, tilesetFile.tilePixelDims });
		lastTileTopLeftUv.y -= (float)tilesetFile.tilePixelDims.y / (float)_AtlasHeightPixels;
		lastTileTopLeftUv.x = 0.0;
	}
}

void BetterTileset::BlitFileIntoAtlasMultipleTilesPerRow(unsigned char* writePointer, const FileTilesetData& tilesetFile, glm::vec2& lastTileBottomRightUV, glm::vec2 lastTileTopLeftUv, size_t numPerRow)
{
	const size_t TILE_WIDTH_BYTES = tilesetFile.tilePixelDims.x * NUM_CHANNELS;
	int onTile = 0;
	for (int i = 0; i < tilesetFile.tiles.size() / numPerRow; i++) {
		lastTileTopLeftUv.x = 0.0;
		for (int j = 0; j < numPerRow; j++) {
			auto& tileBytes = tilesetFile.tiles[onTile++];
			for (int i = 0; i < tilesetFile.tilePixelDims.y; i++) {
				memcpy(writePointer, tileBytes.data() + i * TILE_WIDTH_BYTES, TILE_WIDTH_BYTES);
				if (writePointer + _AtlasWidthBytes < _AtlasBuffer.data() + _AtlasBuffer.size())
					writePointer += _AtlasWidthBytes;
			}
			glm::vec2 bottomRightUv;
			bottomRightUv.x = lastTileTopLeftUv.x + ((float)tilesetFile.tilePixelDims.x / (float)_AtlasWidthPixels);
			bottomRightUv.y = lastTileTopLeftUv.y - ((float)tilesetFile.tilePixelDims.y / (float)_AtlasHeightPixels);
			_Tiles.push_back(AtlasTile{ lastTileTopLeftUv, bottomRightUv, tilesetFile.tilePixelDims });
			lastTileTopLeftUv.x += (float)tilesetFile.tilePixelDims.x / (float)_AtlasWidthPixels;
			if (j < numPerRow - 1) {
				// return to copy the next one in the row
				writePointer -= _AtlasWidthBytes * tilesetFile.tilePixelDims.y;
				writePointer += (j + 1) * TILE_WIDTH_BYTES;
			}
			else {
				//writePointer -= TILE_WIDTH_BYTES * numPerRow;
				writePointer += TILE_WIDTH_BYTES;
			}
		}
		lastTileTopLeftUv.y -= (float)tilesetFile.tilePixelDims.y / (float)_AtlasHeightPixels;
	}
}

