#ifndef COORDINATOR
#define COORDINATOR
#include "tilemap.h"
#include "zombies.h"
#include "player.h"
#include "button.h"
#include "texturestorage.h"
#include "fontstorage.h"
#include "soundstorage.h"
#include "musicstorage.h"
#include "rayextended.h"
#include "timer.h"

// so this file is the general game coordination,
// controlling what scene the program is in and updating and drawing
// things accordingly

enum class Scene
{
    MAINMENU,
    INGAME,
    GAMEOVER
};

struct GameData
{
    Scene scn;
    std::vector<Button> butts;
    Map m;
    Player p;
    Zombies z;
    bool apocalypse;
    long long timeStarted;
};

void initialize(GameData& g);
void update(GameData& g);
void draw(GameData& g);

#endif