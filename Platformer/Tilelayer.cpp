#include "Tilelayer.h"
void TileLayer::SetWidthAndHeight(unsigned int w, unsigned int h)
{
	m_Width = w;
	m_Height = h;
	Tiles.resize(m_Width * m_Height, 0);
}
