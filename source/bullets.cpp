#include "bullets.h"

void initialize(Bullets& bs)
{
    bs.clear();
}

void update(Bullets& bs, const TileMap& tm, Zombies& z, long long& lastBulletOrWallBUZZ, long long& lastZombieHitBUZZ)
{
    int speed{64}; // four tile per second
    for (int i{}; i < bs.size(); ++i)
    {
        bs[i].pos += floatAngleToVec2(speed * lowerLimitFrameTime(), bs[i].angle);

        if (bs[i].pos.x < -256 || bs[i].pos.x > 512 || bs[i].pos.y < -256 || bs[i].pos.y > 512) // i think this crashes the game ;)
        {
            bs.erase(bs.begin() + i--);
            continue;
        }
        
        bool haveToContinue{false};
        for (auto& r : getTileRecs(Tile::WALL, tm))
            if (CheckCollisionPointRec(bs[i].pos, r))
            {
                if (getMilTimeSince(lastBulletOrWallBUZZ) > 100)
                {
                    PlaySoundMulti(sndStrg().get("res/bulletwall.wav"));
                    lastBulletOrWallBUZZ = getSystemTimeMil();
                }
                bs.erase(bs.begin() + i--);
                haveToContinue = true;
                break;
            }
        if (haveToContinue) continue;
        for (auto& zm : z.zs)
        {
            if (CheckCollisionCircles(bs[i].pos, txtrStrg().get("res/bullet.png").height/2, zm.pos, 5) && zm.hp > 0)
            {
                zm.timeLastHit = getSystemTimeMil();
                zm.hp -= bs[i].dmg;
                if (getMilTimeSince(lastZombieHitBUZZ) > 137)
                {
                    PlaySoundMulti(sndStrg().get("res/zombiehit.wav"));
                    lastZombieHitBUZZ = getSystemTimeMil();
                }
                bs.erase(bs.begin() + i--);
                break;
            }
        }
    }
}

void draw(Bullets& bs)
{
    Texture2D tx{txtrStrg().get("res/bullet.png")};
    for (auto& b : bs)
    {
        DrawTexturePro(tx, {0, 0, static_cast<float>(tx.width), static_cast<float>(tx.height)}, {b.pos.x, b.pos.y, static_cast<float>(tx.width), static_cast<float>(tx.height)}, {static_cast<float>(tx.width)/2, static_cast<float>(tx.height)/2}, b.angle, WHITE);
    }
}