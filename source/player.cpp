#include "player.h"

void initialize(Player& p)
{
    p.dead = false;
    p.pos = Vector2{96, 128}; // middle of playing field
    p.money = 40;
    p.ms.clear();
    p.xplosionAnims.clear();
    p.lastBullet = getSystemTimeMil();
    p.lastBUZZ = getSystemTimeMil();
    p.lastBulletOrWallBUZZ = getSystemTimeMil();
    p.lastZombieHitBUZZ = getSystemTimeMil();

    initialize(p.bs);
}

void update(Player& p, Map& m, Zombies& z, bool apocalypse)
{
    Vector2 nextPos{p.pos};
    int movementSpeed{48}; // movement speed - 3 tiles per second

    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) // key check and movement
        nextPos.y -= movementSpeed * lowerLimitFrameTime();
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
        nextPos.y += movementSpeed * lowerLimitFrameTime();
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
        nextPos.x -= movementSpeed * lowerLimitFrameTime();
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        nextPos.x += movementSpeed * lowerLimitFrameTime();

    if (nextPos != p.pos)
        nextPos = p.pos + floatAngleToVec2(movementSpeed * lowerLimitFrameTime(), vec2ToAngle(nextPos - p.pos)); // normalize it

    for (auto& r : getTileRecs(Tile::WALL, m.t)) // collision
    {
        if (CheckCollisionCircleRec(nextPos, 5, r))
        {
            if (std::abs(r.x + r.width/2 - nextPos.x) > std::abs(r.y + r.height/2 - nextPos.y))
            {
                if (r.x + r.width/2 - nextPos.x > 0)
                    nextPos.x = r.x - 5;
                else
                    nextPos.x = r.x + r.width + 5;
            }
            else
            {
                if (r.y + r.height/2 - nextPos.y > 0)
                    nextPos.y = r.y - 5;
                else
                    nextPos.y = r.y + r.height + 5;
            }
        }
    }
    if (nextPos.x > 192 - 5) nextPos.x = 192 - 5;
    if (nextPos.x < 0 + 5) nextPos.x = 0 + 5;
    if (nextPos.y > 256 - 5) nextPos.y = 256 - 5;
    if (nextPos.y < 0 + 5) nextPos.y = 0 + 5;

    p.pos = nextPos;

    if (!apocalypse) // walls and vines
    {
        if (IsKeyDown(KEY_E) && GetMousePosition().x < 176 && GetMousePosition().x > 16 && GetMousePosition().y < 240 && GetMousePosition().y > 16 && m.t[GetMousePosition().x/16][GetMousePosition().y/16] != Tile::WALL && !(Coord{static_cast<int>(GetMousePosition().x)/16, static_cast<int>(GetMousePosition().y)/16} == Coord{static_cast<int>(p.pos.x)/16, static_cast<int>(p.pos.y)/16}))
        { // build wall
            if (p.money > 16 && wallBetween2Vector2s(p.pos, GetMousePosition(), m) == GetMousePosition())
            {
                m.t[static_cast<int>(GetMousePosition().x)/16][static_cast<int>(GetMousePosition().y)/16] = Tile::WALL;
                if (inEnclosedRegionNextToCoord({static_cast<int>(GetMousePosition().x)/16, static_cast<int>(GetMousePosition().y)/16}, m.t))
                {
                    m.t[static_cast<int>(GetMousePosition().x)/16][static_cast<int>(GetMousePosition().y)/16] = Tile::EMPTY;
                    if (getMilTimeSince(p.lastBUZZ) > 250)
                    {
                        p.lastBUZZ = getSystemTimeMil();
                        PlaySoundMulti(sndStrg().get("res/nomoney.wav"));
                        p.ms.push_back({getSystemTimeMil(), p.pos - Vector2{0, 10}, "cannot block in area", RED});
                    }
                }
                else
                {
                    p.money -= 16;
                    PlaySoundMulti(sndStrg().get("res/wall.wav"));
                }
            }
            else if (getMilTimeSince(p.lastBUZZ) > 250)
            {
                p.lastBUZZ = getSystemTimeMil();
                PlaySoundMulti(sndStrg().get("res/nomoney.wav"));
                if (wallBetween2Vector2s(p.pos, GetMousePosition(), m) != GetMousePosition())
                    p.ms.push_back({getSystemTimeMil(), p.pos - Vector2{0, 10}, "not in line of sight", RED});
                else if (p.money <= 16)
                    p.ms.push_back({getSystemTimeMil(), p.pos - Vector2{0, 10}, "not enough money", RED});
            }
        }
        if (IsKeyDown(KEY_SPACE) && !(wallNextToCoord({static_cast<int>(p.pos.x)/16, static_cast<int>(p.pos.y)/16}, m.t) == Coord{-1, -1}) && m.t[static_cast<int>(p.pos.x)/16][static_cast<int>(p.pos.y)/16] == Tile::EMPTY)
        { // place vine
            if (p.money >= 2)
            {
                m.t[static_cast<int>(p.pos.x)/16][static_cast<int>(p.pos.y)/16] = Tile::VINE;
                m.i[static_cast<int>(p.pos.x)/16][static_cast<int>(p.pos.y)/16] = 0;
                p.money -= 2;
                PlaySoundMulti(sndStrg().get("res/vine.wav"));
            }
            else if (getMilTimeSince(p.lastBUZZ) > 250)
            {
                p.lastBUZZ = getSystemTimeMil();
                PlaySoundMulti(sndStrg().get("res/nomoney.wav"));
                if (p.money < 2)
                    p.ms.push_back({getSystemTimeMil(), p.pos - Vector2{0, 10}, "not enough money", RED});
            }
        }
        if (m.t[static_cast<int>(p.pos.x)/16][static_cast<int>(p.pos.y)/16] == Tile::VINE && m.i[static_cast<int>(p.pos.x)/16][static_cast<int>(p.pos.y)/16] == 1)
        { // collect vine
            m.t[static_cast<int>(p.pos.x)/16][static_cast<int>(p.pos.y)/16] = Tile::EMPTY;
            m.i[static_cast<int>(p.pos.x)/16][static_cast<int>(p.pos.y)/16] = 0;
            p.money += 16;
            PlaySoundMulti(sndStrg().get("res/money.wav"));
            p.ms.push_back({getSystemTimeMil(), p.pos - Vector2{0, 10}, "+16$", YELLOW});
        }
    }
    else // apocalypse stuff
    {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && getMilTimeSince(p.lastBullet) > (p.gunLevel < 3 ? 500/(p.gunLevel + 1) : 500/3))
        {// shoot bullet
            float angle{vec2ToAngle(GetMousePosition() - p.pos)};
            p.bs.push_back({p.pos + floatAngleToVec2(11, angle), angle, static_cast<float>(p.gunLevel + 1)});
            p.lastBullet = getSystemTimeMil();
            if (getMilTimeSince(p.lastBulletOrWallBUZZ) > 100)
            {
                PlaySoundMulti(sndStrg().get("res/shoot.wav"));
                p.lastBulletOrWallBUZZ = getSystemTimeMil();
            }
        }
        if (IsKeyPressed(KEY_LEFT_SHIFT))
        {// upgrade gun
            if (p.money >= 32)
            {
                ++p.gunLevel;
                p.money -= 32;
                PlaySoundMulti(sndStrg().get("res/levelup.wav"));
            }
            else
            {
                PlaySoundMulti(sndStrg().get("res/nomoney.wav"));
            }
        }
        if (IsKeyDown(KEY_E) && GetMousePosition().x < 192 && GetMousePosition().x > 0 && GetMousePosition().y < 256 && GetMousePosition().y > 0 && m.t[GetMousePosition().x/16][GetMousePosition().y/16] != Tile::WALL && m.t[GetMousePosition().x/16][GetMousePosition().y/16] != Tile::MINE && m.t[GetMousePosition().x/16][GetMousePosition().y/16] != Tile::TURRET)
        { // build turret
            if (p.money >= 64 && wallBetween2Vector2s(p.pos, GetMousePosition(), m) == GetMousePosition())
            {
                m.t[static_cast<int>(GetMousePosition().x)/16][static_cast<int>(GetMousePosition().y)/16] = Tile::TURRET;
                m.i[static_cast<int>(GetMousePosition().x)/16][static_cast<int>(GetMousePosition().y)/16] = 0;
                m.m[static_cast<int>(GetMousePosition().x)/16][static_cast<int>(GetMousePosition().y)/16] = 0;
                p.money -= 64;
                PlaySoundMulti(sndStrg().get("res/turret.wav"));
            }
            else if (getMilTimeSince(p.lastBUZZ) > 250)
            {
                p.lastBUZZ = getSystemTimeMil();
                PlaySoundMulti(sndStrg().get("res/nomoney.wav"));
                if (wallBetween2Vector2s(p.pos, GetMousePosition(), m) != GetMousePosition())
                    p.ms.push_back({getSystemTimeMil(), p.pos - Vector2{0, 10}, "not in line of sight", RED});
                else if (p.money < 64)
                    p.ms.push_back({getSystemTimeMil(), p.pos - Vector2{0, 10}, "not enough money", RED});
            }
        }
        if (IsKeyDown(KEY_SPACE) && !(wallNextToCoord({static_cast<int>(p.pos.x)/16, static_cast<int>(p.pos.y)/16}, m.t) == Coord{-1, -1}) && (m.t[static_cast<int>(p.pos.x)/16][static_cast<int>(p.pos.y)/16] == Tile::EMPTY || m.t[GetMousePosition().x/16][GetMousePosition().y/16] == Tile::VINE))
        { // place mine
            if (p.money >= 4)
            {
                m.t[static_cast<int>(p.pos.x)/16][static_cast<int>(p.pos.y)/16] = Tile::MINE;
                m.i[static_cast<int>(p.pos.x)/16][static_cast<int>(p.pos.y)/16] = 0;
                m.m[static_cast<int>(p.pos.x)/16][static_cast<int>(p.pos.y)/16] = 69;
                p.money -= 4;
                PlaySoundMulti(sndStrg().get("res/mineplace.wav"));
            }
            else if (getMilTimeSince(p.lastBUZZ) > 250)
            {
                p.lastBUZZ = getSystemTimeMil();
                PlaySoundMulti(sndStrg().get("res/nomoney.wav"));
                if (p.money < 4)
                    p.ms.push_back({getSystemTimeMil(), p.pos - Vector2{0, 10}, "not enough money", RED});
            }
        }
        for (auto& t : getTileRecs(Tile::TURRET, m.t))
        { // update turrets
            int x{static_cast<int>(t.x)/16};
            int y{static_cast<int>(t.y)/16};

            int closestZomI{-1};
            for (int i{}; i < z.zs.size(); ++i)
            {
                if (wallBetween2Vector2s({t.x+8, t.y+8}, z.zs[i].pos, m) == z.zs[i].pos && (closestZomI == -1 || vec2distance({t.x+8, t.y+8}, z.zs[i].pos) < vec2distance({t.x+8, t.y+8}, z.zs[closestZomI].pos)) && z.zs[i].hp > 0)
                    closestZomI = i;
            }
            if (closestZomI != -1)
            {
                m.i[x][y] = vec2ToAngle(z.zs[closestZomI].pos - Vector2{t.x+8, t.y+8});
                if (getMilTimeSince(m.m[x][y]) > 250)
                {
                    p.bs.push_back({Vector2{t.x+8, t.y+8} + floatAngleToVec2(8, m.i[x][y]), m.i[x][y], 5});
                    m.m[x][y] = getSystemTimeMil();
                    if (getMilTimeSince(p.lastBulletOrWallBUZZ) > 100)
                    {
                        PlaySoundMulti(sndStrg().get("res/shoot.wav"));
                        p.lastBulletOrWallBUZZ = getSystemTimeMil();
                    }
                }
            }
            else
            {
                m.i[x][y] += lowerLimitFrameTime() * 90;
            }
        }
        for (auto& mi : getTileRecs(Tile::MINE, m.t))
        { // update mines
            int x{static_cast<int>(mi.x)/16};
            int y{static_cast<int>(mi.y)/16};

            for (auto& zm : z.zs)
            {
                if (Coord{static_cast<int>(zm.pos.x)/16, static_cast<int>(zm.pos.y)/16} == Coord{x, y} && m.m[x][y] == 69)
                {
                    m.m[x][y] = getSystemTimeMil();
                    PlaySoundMulti(sndStrg().get("res/minebeep.wav"));
                }
            }
            if (getMilTimeSince(m.m[x][y]) > 500 && m.m[x][y] != 69)
            {
                for (auto& zm : z.zs)
                {
                    if (CheckCollisionCircles(zm.pos, 5, {mi.x + 8, mi.y + 8}, 32))
                    {
                        zm.timeLastHit = getSystemTimeMil();
                        zm.hp -= 20;
                        if (getMilTimeSince(p.lastZombieHitBUZZ) > 137)
                        {
                            PlaySoundMulti(sndStrg().get("res/zombiehit.wav"));
                            p.lastZombieHitBUZZ = getSystemTimeMil();
                        }
                    }
                }

                p.xplosionAnims.push_back({txtrStrg().get("res/explosion.png"), 1, 4, 4});
                p.xplosionAnims[p.xplosionAnims.size() - 1].start({mi.x - 8, mi.y - 8}, 1, 1, WHITE);
                PlaySoundMulti(sndStrg().get("res/explosion.wav"));
                m.t[x][y] = Tile::EMPTY;
            }
        }
        for (auto& zm : z.zs)
        {
            if (CheckCollisionCircles(zm.pos, 2, p.pos, 2))
                p.dead = true;
        }
    }
    for (int i{}; i < p.ms.size(); ++i)
    {
        if (getMilTimeSince(p.ms[i].timeCreated) > 1500)
            p.ms.erase(p.ms.begin() + i--);
    }
    for (int i{}; i < p.xplosionAnims.size(); ++i)
    {
        p.xplosionAnims[i].update();
        if (p.xplosionAnims[i].justEnded())
            p.xplosionAnims.erase(p.xplosionAnims.begin() + i--);
    }
    update(p.bs, m.t, z, p.lastBulletOrWallBUZZ, p.lastZombieHitBUZZ);
}

void draw(Player& p, Map& m, Zombies& z, bool apocalypse)
{
    Texture2D tx{apocalypse ? txtrStrg().get("res/playerangry.png") : txtrStrg().get("res/playerhappy.png")};
    DrawTexturePro(tx, {0, 0, static_cast<float>(tx.width), static_cast<float>(tx.height)}, {p.pos.x, p.pos.y, static_cast<float>(tx.width), static_cast<float>(tx.height)}, {static_cast<float>(tx.width)/2, static_cast<float>(tx.height)/2}, vec2ToAngle(GetMousePosition() - p.pos) - 90, WHITE);

    if (apocalypse)
        DrawTexturePro(txtrStrg().get("res/gun.png"), {0, 0, 3, 6}, {p.pos.x, p.pos.y, 3, 6}, {1.5, -5}, vec2ToAngle(GetMousePosition() - p.pos) - 90, WHITE);

    Vector2 lineEndPoint{wallBetween2Vector2s(p.pos, GetMousePosition() + (GetMousePosition() - p.pos) * 64000, m)};
    for (auto& zm : z.zs)
    {
        CheckCollisionLines(floatAngleToVec2(5, vec2ToAngle(p.pos - zm.pos) + 90) + zm.pos, floatAngleToVec2(5, vec2ToAngle(p.pos - zm.pos) - 90) + zm.pos, p.pos, lineEndPoint, &lineEndPoint);
    }
    DrawLineEx(p.pos + floatAngleToVec2((apocalypse ? 11 : 5), vec2ToAngle(lineEndPoint - p.pos)), lineEndPoint, 2, (apocalypse ? Color{255, 0, 0, 175} : Color{0, 150, 255, 175}));

    for (auto& t : getTileRecs(Tile::TURRET, m.t))
    { // draw turret lasers
        Vector2 turLineEndPoint{wallBetween2Vector2s(Vector2{t.x+8, t.y+8}, Vector2{t.x+8, t.y+8} + floatAngleToVec2(64000, m.i[static_cast<int>(t.x)/16][static_cast<int>(t.y)/16]), m)};
        for (auto& zm : z.zs)
        {
            CheckCollisionLines(floatAngleToVec2(5, vec2ToAngle(Vector2{t.x+8, t.y+8} - zm.pos) + 90) + zm.pos, floatAngleToVec2(5, vec2ToAngle(Vector2{t.x+8, t.y+8} - zm.pos) - 90) + zm.pos, Vector2{t.x+8, t.y+8}, turLineEndPoint, &turLineEndPoint);
        }
        DrawLineEx(Vector2{t.x+8, t.y+8} + floatAngleToVec2(8, vec2ToAngle(turLineEndPoint - Vector2{t.x+8, t.y+8})), turLineEndPoint, 2, Color{255, 0, 0, 175});
    }

    draw(p.bs);

    for (int i{}; i < p.xplosionAnims.size(); ++i)
    {
        p.xplosionAnims[i].draw();
    }
}

void drawHud(Player& p, bool apocalypse)
{
    DrawTextEx(fontStrg().get("Hack-Regular.ttf"), (std::to_string(p.money) + "$").c_str(), {200, 225}, 20, 0, YELLOW);

    for (auto& m : p.ms)
    {
        Vector2 txtSize{MeasureTextEx(fontStrg().get("Hack-Bold.ttf"), m.txt.c_str(), 16, 0)};
        DrawTextEx(fontStrg().get("Hack-Bold.ttf"), m.txt.c_str(), {m.pos.x - txtSize.x/2, m.pos.y - txtSize.y/2}, 12, 0, {m.col.r, m.col.g, m.col.b, static_cast<unsigned char>((3 - (getMilTimeSince(m.timeCreated) / 1512.0f)) * 255)});
    }
}