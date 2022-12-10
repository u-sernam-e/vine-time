#ifndef BULLETS
#define BULLETS
#include "zombies.h"
#include "tilemap.h"
#include "rayextended.h"
#include "texturestorage.h"
#include "soundstorage.h"
#include <vector>

struct Bullet
{
    Vector2 pos;
    float angle;
    float dmg;
};

using Bullets = std::vector<Bullet>;

void initialize(Bullets& bs);
void update(Bullets& bs, const TileMap& tm, Zombies& z, long long& lastBulletOrWallBUZZ, long long& lastZombieHitBUZZ);
void draw(Bullets& bs);

#endif