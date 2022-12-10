#include "zombies.h"

void initialize(Zombies& z)
{
    z.zs.clear();
    z.coordsFound.clear();
    z.lastSpawn = getSystemTimeMil();
}

Vector2 randomPositionJustOutsideTheScreen()
{
    bool vertOrHorz(GetRandomValue(0, 1) == 0);
    bool leftUpOrRightDown(GetRandomValue(0, 1) == 0);

    if (vertOrHorz)
    {
        if (leftUpOrRightDown) return {static_cast<float>(GetRandomValue(-8, 264)), -8};
        return {static_cast<float>(GetRandomValue(-8, 264)), 264};
    }
    if (leftUpOrRightDown) return {-8, static_cast<float>(GetRandomValue(-8, 264))};
    return {264, static_cast<float>(GetRandomValue(-8, 264))};
}

void spawnZombie(Zombies& z, long long apocalypseStartTime)
{
    z.zs.push_back(
    {
        randomPositionJustOutsideTheScreen(),
        0,
        getMilTimeSince(apocalypseStartTime) / 2000.0f,
        getMilTimeSince(apocalypseStartTime) / 3000.0f + 16,
        0
    });
}

bool canGoStraightTowardsPlayer(Vector2 zomPos, Vector2 playerPos, Map& m)
{
    return (
        wallBetween2Vector2s(floatAngleToVec2(5, vec2ToAngle(playerPos - zomPos) + 90) + zomPos, floatAngleToVec2(5, vec2ToAngle(playerPos - zomPos) + 90) + playerPos, m) == floatAngleToVec2(5, vec2ToAngle(playerPos - zomPos) + 90) + playerPos &&
        wallBetween2Vector2s(floatAngleToVec2(5, vec2ToAngle(playerPos - zomPos) - 90) + zomPos, floatAngleToVec2(5, vec2ToAngle(playerPos - zomPos) - 90) + playerPos, m) == floatAngleToVec2(5, vec2ToAngle(playerPos - zomPos) - 90) + playerPos
    );
}

void update(Zombies& z, Map& m, long long apocalypseStartTime, Vector2 playerPos)
{
    if (getMilTimeSince(apocalypseStartTime) > 5000 && getMilTimeSince(z.lastSpawn) > 10'000'000 / getMilTimeSince(apocalypseStartTime))
    {
        z.lastSpawn = getSystemTimeMil();
        spawnZombie(z, apocalypseStartTime);
    }

    if (!(z.lastPlayerCoord == Coord{static_cast<int>(playerPos.x)/16, static_cast<int>(playerPos.y)/16}))
    {
        z.coordsFound.clear();
        z.lastPlayerCoord = Coord{static_cast<int>(playerPos.x)/16, static_cast<int>(playerPos.y)/16};
    }

    for (int i{}; i < z.zs.size(); ++i)
    {
        if (z.zs[i].hp > 0)
        {
            if (canGoStraightTowardsPlayer(z.zs[i].pos, playerPos, m))
                z.zs[i].directionFacing = vec2ToAngle(playerPos - z.zs[i].pos);
            else
            {
                Coord nextCrd{-64, -64};
                for (auto& a : z.coordsFound)
                {
                    if (a[0] == Coord{static_cast<int>(z.zs[i].pos.x)/16, static_cast<int>(z.zs[i].pos.y)/16})
                        nextCrd = a[1];
                }
                if (nextCrd == Coord{-64, -64})
                {
                    z.coordsFound.push_back({Coord{static_cast<int>(z.zs[i].pos.x)/16, static_cast<int>(z.zs[i].pos.y)/16}, aSNext(Coord{static_cast<int>(z.zs[i].pos.x)/16, static_cast<int>(z.zs[i].pos.y)/16}, Coord{static_cast<int>(playerPos.x)/16, static_cast<int>(playerPos.y)/16}, m.t)});
                    nextCrd = z.coordsFound[z.coordsFound.size() - 1][1];
                }
                z.zs[i].directionFacing = vec2ToAngle(Vector2{nextCrd.x * 16.0f, nextCrd.y * 16.0f} + Vector2{8, 8} - z.zs[i].pos);
            }
            z.zs[i].pos += floatAngleToVec2(z.zs[i].speed * lowerLimitFrameTime(), z.zs[i].directionFacing);
        }
        else if (getMilTimeSince(z.zs[i].timeLastHit) > 900)
            z.zs.erase(z.zs.begin() + i--);
    }
}

void draw(Zombies& z, Map& m, long long apocalypseStartTime, Vector2 playerPos)
{
    for (auto& zm : z.zs)
    {
        if (zm.hp > 0)
        {
            Texture2D tx = txtrStrg().get("res/zombie.png");
            float bgOpac = (getMilTimeSince(zm.timeLastHit) / 500.0f > 1 ? 1 : getMilTimeSince(zm.timeLastHit) / 500.0f);
            DrawTexturePro(tx, {0, 0, static_cast<float>(tx.width), static_cast<float>(tx.height)}, {zm.pos.x, zm.pos.y, static_cast<float>(tx.width), static_cast<float>(tx.height)}, {static_cast<float>(tx.width)/2, static_cast<float>(tx.height)/2}, zm.directionFacing - 90, {255, static_cast<unsigned char>(255 * bgOpac), static_cast<unsigned char>(255 * bgOpac), 255});
        }
        else
        {
            long long frame{getMilTimeSince(zm.timeLastHit) / 300};
            if (frame <= 2)
                DrawTexturePro(txtrStrg().get("res/bloodsplatter.png"), {20.0f * frame, 0, 20, 20}, {zm.pos.x - 10, zm.pos.y - 10, 20, 20}, {0, 0}, 0, WHITE);
        }
    }
}