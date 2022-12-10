#ifndef TILEMAP
#define TILEMAP
#include "rayextended.h"
#include "texturestorage.h"
#include "timer.h"
#include <array>
#include <vector>
#include <cmath>
#include <algorithm>

enum class Tile
{
    EMPTY,
    WALL,
    VINE,
    MINE,
    TURRET
};

struct Coord
{
    int x;
    int y;
};

bool operator==(const Coord& a, const Coord& b);

using TileMap = std::array<std::array<Tile, 16>, 12>;
using InfoMap = std::array<std::array<float, 16>, 12>;
using MoreInfoMap = std::array<std::array<long long, 16>, 12>;

struct Map
{
    TileMap t;
    InfoMap i;
    MoreInfoMap m;
};

Vector2 wallBetween2Vector2s(Vector2 a, Vector2 b, const Map& m);
bool inEnclosedRegion(Coord a, const TileMap& map);
bool inEnclosedRegionNextToCoord(Coord a, const TileMap& map);
Coord aSNext(Coord a, Coord b, const TileMap& map);
std::vector<Rectangle> getTileRecs(Tile t, const TileMap& tm);
Coord wallNextToCoord(Coord a, const TileMap& tm);

void initialize(Map& m);
void update(Map& m, bool apocalypse);
void draw(const Map& m, bool apocalypse);

#endif