#pragma once
#include <vector>
enum TileLayerType : unsigned int {
	Decoration = 0,
	Solid = 1,
	Kills = 2,
	PickUp = 3,
	Ladder = 4
};

struct TileLayer {
public:
	TileLayer() {
		Tiles.resize(m_Width * m_Height, 0);
	}
	std::vector<unsigned int> Tiles;
	void SetWidthAndHeight(unsigned int w, unsigned int h);
	unsigned int GetWidth() const { return m_Width; }
	unsigned int GetHeight() const { return m_Height; }
	int LayerZPosition; // not used... yet
	TileLayerType Type = Decoration;
	bool Visible;
private:
	unsigned int m_Width = 10;
	unsigned int m_Height = 10;

};