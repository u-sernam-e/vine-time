#ifndef PLAYER
#define PLAYER
#include "zombies.h"
#include "tilemap.h"
#include "bullets.h"
#include "rayextended.h"
#include "texturestorage.h"
#include "soundstorage.h"
#include "fontstorage.h"
#include "timer.h"
#include "anim.h"

struct Message
{
    long long timeCreated;
    Vector2 pos;
    std::string txt;
    Color col;
};

struct Player
{
    bool dead;
    Vector2 pos;
    int money;
    int gunLevel;
    Bullets bs;
    std::vector<Message> ms;
    long long lastBullet;
    long long lastBUZZ; // so it doesn't go BUZZZZZ when you hold space at a wall with no money
    long long lastBulletOrWallBUZZ;
    long long lastZombieHitBUZZ;
    std::vector<Anim> xplosionAnims;
};

void initialize(Player& p);
void update(Player& p, Map& m, Zombies& z, bool apocalypse);
void draw(Player& p, Map& m, Zombies& z, bool apocalypse);
void drawHud(Player& p, bool apocalypse);

#endif