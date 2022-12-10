#ifndef ZOMBIES
#define ZOMBIES
#include "tilemap.h"
#include "rayextended.h"
#include "texturestorage.h"
#include "soundstorage.h"
#include "timer.h"

struct Zombie
{
    Vector2 pos;
    float directionFacing;
    float hp;
    float speed;
    long long timeLastHit; // for hit animation
};

struct Zombies
{
    std::vector<Zombie> zs;
    std::vector<std::array<Coord, 2>> coordsFound;
    Coord lastPlayerCoord;
    long long lastSpawn;
};

void initialize(Zombies& z);
void update(Zombies& z, Map& m, long long apocalypseStartTime, Vector2 playerPos);
void draw(Zombies& z, Map& m, long long apocalypseStartTime, Vector2 playerPos);

#endif