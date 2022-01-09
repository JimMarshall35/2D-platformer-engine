#include "JSONLevelSerializer.h"
#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "Engine.h"

void JSONLevelSerializer::Serialize(const Engine& engine, std::string filePath)
{
	using namespace rapidjson;
	Document doc;
	doc.SetObject();

	// tile dims
	Value tileDims;
	tileDims.SetObject();
	tileDims.AddMember("x", engine._Tileset.TileWidthAndHeightPx.x,doc.GetAllocator());
	tileDims.AddMember("y", engine._Tileset.TileWidthAndHeightPx.y, doc.GetAllocator());
	tileDims.AddMember("tileset_w", engine._Tileset.TileSetWidthAndHeightTiles.x,doc.GetAllocator());
	tileDims.AddMember("tileset_h", engine._Tileset.TileSetWidthAndHeightTiles.y, doc.GetAllocator());
	doc.AddMember("tiledims", tileDims, doc.GetAllocator());

	// tileset paths, first tile ID's
	Value tilesetPathsArray = Value(kArrayType);
	for (auto t : engine._Tileset.FilesList) {
		Value tileset_file;
		tileset_file.SetObject();
		Value path_val;
		path_val.SetString(t.path.c_str(), doc.GetAllocator());
		tileset_file.AddMember("path", path_val, doc.GetAllocator());
		tileset_file.AddMember("firstID", t.firstTileID, doc.GetAllocator());
		tileset_file.AddMember("tile_width_px", t.TileWidthAndHeightPx.x, doc.GetAllocator());
		tileset_file.AddMember("tile_height_px", t.TileWidthAndHeightPx.y, doc.GetAllocator());
		tilesetPathsArray.PushBack(tileset_file, doc.GetAllocator());
	}
	doc.AddMember("tileset", tilesetPathsArray, doc.GetAllocator());

	// tile layers
	Value layersArrayTopLevel = Value(kArrayType);
	for (const TileLayer& tl : engine._TileLayers) {
		Value tileLayer;
		tileLayer.SetObject();
		Value widthTiles(tl.GetWidth());
		Value heightTiles(tl.GetHeight());
		Value type((unsigned int)tl.Type);
		Value tilesArray = Value(kArrayType);
		for (unsigned int tile : tl.Tiles) {
			tilesArray.PushBack(Value(tile), doc.GetAllocator());
		}
		tileLayer.AddMember("widthtiles", widthTiles, doc.GetAllocator());
		tileLayer.AddMember("heighttiles", heightTiles, doc.GetAllocator());
		tileLayer.AddMember("type", type, doc.GetAllocator());
		tileLayer.AddMember("tiles", tilesArray, doc.GetAllocator());
		layersArrayTopLevel.PushBack(tileLayer, doc.GetAllocator());
	}
	doc.AddMember("tilelayers", layersArrayTopLevel, doc.GetAllocator());
	
	// animations
	Value animationsArray = Value(kArrayType);
	for (auto [key, val] : engine._Tileset.AnimationsMap) {
		Value anim_object;
		anim_object.SetObject();
		Value anim_name;
		anim_name.SetString(key.c_str(), doc.GetAllocator());
		anim_object.AddMember("name", anim_name, doc.GetAllocator());
		Value framesArray = Value(kArrayType);
		for (int i = 0; i < val.size(); i++) {
			framesArray.PushBack(Value(val[i]), doc.GetAllocator());
		}
		anim_object.AddMember("frames", framesArray, doc.GetAllocator());
		animationsArray.PushBack(anim_object, doc.GetAllocator());
	}
	doc.AddMember("animations", animationsArray, doc.GetAllocator());


	// 3. Stringify the DOM
	StringBuffer buffer;
	PrettyWriter<StringBuffer> writer(buffer);
	writer.SetFormatOptions(kFormatSingleLineArray);
	doc.Accept(writer);

	// Output {"project":"rapidjson","stars":11}
	std::cout << buffer.GetString() << std::endl;
	std::ofstream myfile;
	myfile.open(filePath);
	myfile << buffer.GetString();
	myfile.close();
}

bool JSONLevelSerializer::DeSerialize(Engine& engine, std::string filePath)
{
	using namespace rapidjson;
	engine._TileLayers.clear();
	engine._Tileset.ClearTiles();
	engine._Tileset.AnimationsMap.clear();
	std::ifstream ifs;
	ifs.open(filePath);
	std::string file_content((std::istreambuf_iterator<char>(ifs)),
		(std::istreambuf_iterator<char>()));
	Document doc;
	doc.Parse(file_content.c_str());

	const Value& tiledims = doc["tiledims"];
	
	engine._Tileset.TileWidthAndHeightPx.x = tiledims["x"].GetInt();
	engine._Tileset.TileWidthAndHeightPx.y = tiledims["y"].GetInt();
	engine._Tileset.TileSetWidthAndHeightTiles.x = tiledims["tileset_w"].GetInt();
	engine._Tileset.TileSetWidthAndHeightTiles.y = tiledims["tileset_h"].GetInt();

	const Value& tileset = doc["tileset"];
	for (SizeType i = 0; i < tileset.Size(); i++) {
		const Value& tilesetFile = tileset[i];
		auto path = std::string(tilesetFile["path"].GetString());
		engine._Tileset.TileWidthAndHeightPx.x =  tilesetFile["tile_width_px"].GetInt();
		engine._Tileset.TileWidthAndHeightPx.y = tilesetFile["tile_height_px"].GetInt();
		engine._Tileset.LoadTilesFromImgFile(path);
	}
	
	const Value& tilelayers = doc["tilelayers"];
	for (SizeType i = 0; i < tilelayers.Size(); i++) {
		const Value& tilelayer = tilelayers[i];
		TileLayer tl;
		int tl_w = tilelayer["widthtiles"].GetInt();
		int tl_h = tilelayer["heighttiles"].GetInt();
		unsigned int type = tilelayer["type"].GetUint();
		tl.Type = (TileLayerType)type;
		tl.SetWidthAndHeight(tl_w, tl_h);
		
		
		const Value& tiles = tilelayer["tiles"];
		for (SizeType j = 0; j < tiles.Size(); j++) {
			tl.Tiles[j] = tiles[j].GetInt();
		}
		engine._TileLayers.push_back(tl);
	}

	const Value& animations = doc["animations"];
	for (SizeType i = 0; i < animations.Size(); i++) {
		const Value& animation = animations[i];
		const std::string anim_name = animation["name"].GetString();
		std::vector<unsigned int> anim_frames;
		const Value& json_frames = animation["frames"];
		for (SizeType j = 0; j < json_frames.Size(); j++)
			anim_frames.push_back(json_frames[j].GetUint());
		engine._Tileset.AnimationsMap[anim_name] = anim_frames;
	}
	return true;
}
