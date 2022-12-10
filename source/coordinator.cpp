#include "coordinator.h"

void changeScene(GameData& g, Scene s)
{
    g.scn = s;
    switch (s)
    {
        case Scene::MAINMENU:
        {
            g.butts = {Button{{156, 206}, {80, 30}, txtrStrg().get("res/startbutton.png"), LIGHTGRAY, WHITE, GRAY, "", true}};
            PlayMusicStream(mscStrg().get("res/Thetitle.wav"));
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
            if (IsMusicStreamPlaying(mscStrg().get("res/Thetitle.wav")))
                StopMusicStream(mscStrg().get("res/Thetitle.wav"));
            if (IsMusicStreamPlaying(mscStrg().get("res/Peaceful.wav")))
                StopMusicStream(mscStrg().get("res/Peaceful.wav"));
            PlayMusicStream(mscStrg().get("res/Ps.wav"));
            break;
        }
        case Scene::GAMEOVER:
        {
            g.butts = {Button{{156, 206}, {80, 30}, txtrStrg().get("res/restartbutton.png"), LIGHTGRAY, WHITE, GRAY, "", true}};
            StopSoundMulti();
            PlaySoundMulti(sndStrg().get("res/death.wav"));
            StopMusicStream(mscStrg().get("res/Pano.wav"));
            PlayMusicStream(mscStrg().get("res/Peaceful.wav"));
            break;
        }
    }
}

void initialize(GameData& g)
{
    txtrStrg().init({"res/error.png", "res/bullet.png", "res/playerhappy.png", "res/playerangry.png", "res/backgroundapocalypse.png", "res/backgroundnonapocalypse.png", "res/sidebar.png", "res/wall.png", "res/vine.png", "res/zombie.png", "res/mine.png", "res/turret.png", "res/gun.png", "res/bloodsplatter.png", "res/explosion.png", "res/apocalypsestartscreen.png", "res/gameoverscreen.png", "res/titlescreen.png", "res/startbutton.png", "res/restartbutton.png"});
    fontStrg().init({"res/dogicapixel.ttf"});
    sndStrg().init({"res/errorsound.wav", "res/wall.wav", "res/vine.wav", "res/money.wav", "res/nomoney.wav", "res/shoot.wav", "res/explosion.wav", "res/levelup.wav", "res/turret.wav", "res/mineplace.wav", "res/minebeep.wav", "res/bulletwall.wav", "res/zombiehit.wav", "res/apocalypse.wav", "res/death.wav"});
    mscStrg().init({"res/errormusic.wav", "res/Thetitle.wav", "res/Ps.wav", "res/Pano.wav", "res/Peaceful.wav"});
    mscStrg().get("res/Thetitle.wav").looping = true;
    mscStrg().get("res/Ps.wav").looping = true;
    mscStrg().get("res/Pano.wav").looping = true;
    mscStrg().get("res/Peaceful.wav").looping = true;

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
            UpdateMusicStream(mscStrg().get("res/Thetitle.wav"));
            break;
        }
        case Scene::INGAME:
        {
            if (getMilTimeSince(g.timeStarted) > 120000 && !g.apocalypse)
            {
                g.apocalypse = true;
                StopSoundMulti();
                PlaySoundMulti(sndStrg().get("res/apocalypse.wav"));
                StopMusicStream(mscStrg().get("res/Ps.wav"));
            }
            if (getMilTimeSince(g.timeStarted) > 125000 && !IsMusicStreamPlaying(mscStrg().get("res/Pano.wav")))
            {
                PlayMusicStream(mscStrg().get("res/Pano.wav"));
            }


            if (g.p.dead)
                changeScene(g, Scene::GAMEOVER);
            
            update(g.m, g.apocalypse);
            update(g.p, g.m, g.z, g.apocalypse);
            if (g.apocalypse)
                update(g.z, g.m, g.timeStarted + 125000, g.p.pos);

            UpdateMusicStream(mscStrg().get("res/Ps.wav"));
            UpdateMusicStream(mscStrg().get("res/Pano.wav"));

            break;
        }
        case Scene::GAMEOVER:
        {
            if (g.butts[0].released())
                changeScene(g, Scene::INGAME);
            UpdateMusicStream(mscStrg().get("res/Peaceful.wav"));
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
                DrawTextEx(fontStrg().get("res/dogicapixel.ttf"), "e-\nwall\n16$\n\nspace-\nvines\n2$", {196, 45}, 8, 0, WHITE);
            }
            else
            {
                DrawTextEx(fontStrg().get("res/dogicapixel.ttf"), ("click-\nshoot\n\ne-\nturret\n64$\n\nspace-\nmine\n4$\n\nlshift-\nupgrade\ngun\n32$\n(lvl " + std::to_string(g.p.gunLevel) + ")").c_str(), {196, 40}, 8, 0, WHITE);
            }

            long long secondTime{((std::abs(getMilTimeSince(g.timeStarted) - 120000)) / 1000) % 60};
            DrawTextEx(fontStrg().get("res/dogicapixel.ttf"), ((getMilTimeSince(g.timeStarted) > 120000 ? "" : "-") + std::to_string(std::abs(getMilTimeSince(g.timeStarted) - 120000) / 60000) + ":" + (secondTime > 9 ? "" : "0") + std::to_string(secondTime)).c_str(), {200, 16}, 8, 0, RED);

            if (getMilTimeSince(g.timeStarted) > 120000)
            {
                Vector2 txtSize{MeasureTextEx(fontStrg().get("res/dogicapixel.ttf"), "Zombies!", 8, 0)};
                if (getMilTimeSince(g.timeStarted) < 123000)
                {
                    DrawTexture(txtrStrg().get("res/apocalypsestartscreen.png"), 0, 0, WHITE);
                    DrawTextEx(fontStrg().get("res/dogicapixel.ttf"), "Zombies!", {128 - txtSize.x/2, 30}, 8, 0, WHITE);
                }
                else if (getMilTimeSince(g.timeStarted) < 125000)
                {
                    DrawTexture(txtrStrg().get("res/apocalypsestartscreen.png"), 0, 0, {255, 255, 255, static_cast<unsigned char>(255 - ((static_cast<float>(getMilTimeSince(g.timeStarted)) - 123000) / 2000) * 255)});
                    DrawTextEx(fontStrg().get("res/dogicapixel.ttf"), "Zombies!", {128 - txtSize.x/2, 30}, 8, 0, {255, 255, 255, static_cast<unsigned char>(255 - ((static_cast<float>(getMilTimeSince(g.timeStarted)) - 123000) / 2000) * 255)});
                }
            }

            break;
        }
        case Scene::GAMEOVER:
        {
            DrawTexture(txtrStrg().get("res/gameoverscreen.png"), 0, 0, WHITE);
            Vector2 txtSize{MeasureTextEx(fontStrg().get("res/dogicapixel.ttf"), "You have been infected!", 8, 0)};
            DrawTextEx(fontStrg().get("res/dogicapixel.ttf"), "You have been infected!", {128 - txtSize.x/2, 30}, 8, 0, WHITE);
        }
    }

    for (auto& b : g.butts)
        b.draw();
}