#include "coordinator.h"

void changeScene(GameData& g, Scene s)
{
    g.scn = s;
    switch (s)
    {
        case Scene::MAINMENU:
        {
            g.butts = {Button{{156, 206}, {80, 30}, txtrStrg().get("res/startbutton.png"), LIGHTGRAY, WHITE, GRAY, "", true}};
            break;
        }
        case Scene::INGAME:
        {
            g.apocalypse = false;
            g.butts.clear();
            initialize(g.p);
            initialize(g.m);
            initialize(g.z);
            g.timeStarted = getSystemTimeMil();            
            break;
        }
        case Scene::GAMEOVER:
        {
            g.butts = {Button{{156, 206}, {80, 30}, txtrStrg().get("res/restartbutton.png"), LIGHTGRAY, WHITE, GRAY, "", true}};
            StopSoundMulti();
            PlaySoundMulti(sndStrg().get("res/death.wav"));
            break;
        }
    }
}

void initialize(GameData& g)
{
    txtrStrg().init({"res/error.png", "res/bullet.png", "res/playerhappy.png", "res/playerangry.png", "res/backgroundapocalypse.png", "res/backgroundnonapocalypse.png", "res/sidebar.png", "res/wall.png", "res/vine.png", "res/zombie.png", "res/mine.png", "res/turret.png", "res/gun.png", "res/bloodsplatter.png", "res/explosion.png", "res/apocalypsestartscreen.png", "res/gameoverscreen.png", "res/titlescreen.png", "res/startbutton.png", "res/restartbutton.png"});
    fontStrg().init({"res/Hack-Regular.ttf", "res/Hack-Bold.ttf"});
    sndStrg().init({"res/errorsound.wav", "res/wall.wav", "res/vine.wav", "res/money.wav", "res/nomoney.wav", "res/shoot.wav", "res/explosion.wav", "res/levelup.wav", "res/turret.wav", "res/mineplace.wav", "res/minebeep.wav", "res/bulletwall.wav", "res/zombiehit.wav", "res/apocalypse.wav", "res/death.wav"});
    mscStrg().init({"res/errormusic.wav"});
    mscStrg().get("res/errormusic.wav").looping = true;

    changeScene(g, Scene::MAINMENU);
}

void update(GameData& g)
{
    for (auto& b : g.butts)
        b.update();

    switch (g.scn)
    {
        case Scene::MAINMENU:
        {
            if (g.butts[0].released())
                changeScene(g, Scene::INGAME);
            break;
        }
        case Scene::INGAME:
        {
            if (getMilTimeSince(g.timeStarted) > 120000 && !g.apocalypse)
            {
                g.apocalypse = true;
                StopSoundMulti();
                PlaySoundMulti(sndStrg().get("res/apocalypse.wav"));
            }

            if (g.p.dead)
                changeScene(g, Scene::GAMEOVER);
            
            update(g.m, g.apocalypse);
            update(g.p, g.m, g.z, g.apocalypse);
            if (g.apocalypse)
                update(g.z, g.m, g.timeStarted + 125000, g.p.pos);

            break;
        }
        case Scene::GAMEOVER:
        {
            if (g.butts[0].released())
                changeScene(g, Scene::INGAME);
        }
    }
}

void draw(GameData& g)
{
    switch (g.scn)
    {
        case Scene::MAINMENU:
        {
            DrawTexture(txtrStrg().get("res/titlescreen.png"), 0, 0, WHITE);
            DrawTextEx(fontStrg().get("res/Hack-Regular.ttf"), "Font - Hack by Chris Simpkins on dafont.com\nImages made with GIMP\nMusic made with Caustic 3.0\nSFX made with rFXGen\nMade with RayLib", {10, 10}, 10, 0, GRAY);
            break;
        }
        case Scene::INGAME:
        {
            if (g.apocalypse) DrawTexture(txtrStrg().get("res/backgroundapocalypse.png"), 0, 0, WHITE);
            else DrawTexture(txtrStrg().get("res/backgroundnonapocalypse.png"), 0, 0, WHITE);

            draw(g.m, g.apocalypse);
            draw(g.p, g.m, g.z, g.apocalypse);
            draw(g.z, g.m, g.timeStarted + 125000, g.p.pos);

            DrawTexture(txtrStrg().get("res/sidebar.png"), 192, 0, GRAY);

            drawHud(g.p, g.apocalypse);

            if (!g.apocalypse)
            {
                DrawTextEx(fontStrg().get("Hack-Regular.ttf"), "e-\nwall\n16$\n\nspace-\nvines\n2$", {196, 45}, 16, 0, WHITE);
            }
            else
            {
                DrawTextEx(fontStrg().get("Hack-Bold.ttf"), ("click-\bshoot\ne-\turret\n64$\n\nspace-\nmine\n4$\nlshift-\nupgrade gun\n32$\n(level " + std::to_string(g.p.gunLevel) + ")").c_str(), {196, 40}, 12, 0, WHITE);
            }

            long long secondTime{((std::abs(getMilTimeSince(g.timeStarted) - 120000)) / 1000) % 60};
            DrawTextEx(fontStrg().get("Hack-Regular.ttf"), ((getMilTimeSince(g.timeStarted) > 120000 ? "" : "-") + std::to_string(std::abs(getMilTimeSince(g.timeStarted) - 120000) / 60000) + ":" + (secondTime > 9 ? "" : "0") + std::to_string(secondTime)).c_str(), {200, 16}, 20, 0, RED);

            if (getMilTimeSince(g.timeStarted) > 120000)
            {
                if (getMilTimeSince(g.timeStarted) < 123000)
                    DrawTexture(txtrStrg().get("res/apocalypsestartscreen.png"), 0, 0, WHITE);
                else if (getMilTimeSince(g.timeStarted) < 125000)
                    DrawTexture(txtrStrg().get("res/apocalypsestartscreen.png"), 0, 0, {255, 255, 255, static_cast<unsigned char>(255 - ((static_cast<float>(getMilTimeSince(g.timeStarted)) - 123000) / 2000) * 255)});
            }

            break;
        }
        case Scene::GAMEOVER:
        {
            DrawTexture(txtrStrg().get("res/gameoverscreen.png"), 0, 0, WHITE);
            DrawTextEx(fontStrg().get("Hack-Regular.ttf"), "You have been infected!", {20, 20}, 20, 0, BLACK);
        }
    }

    for (auto& b : g.butts)
        b.draw();
}