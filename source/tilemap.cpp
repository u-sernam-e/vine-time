#include "tilemap.h"

bool operator==(const Coord& a, const Coord& b) { return a.x == b.x && a.y == b.y; }

Vector2 wallBetween2Vector2s(Vector2 a, Vector2 b, const Map& m)
{
    Vector2 outputPoint{b};
    Vector2 collPoint{};
    for (auto& r : getTileRecs(Tile::WALL, m.t))
    {
        if (CheckCollisionLines(a, b, {r.x, r.y}, {r.x + r.width, r.y}, &collPoint))
            if (vec2distance(collPoint, a) < vec2distance(outputPoint, a))
                outputPoint = collPoint;
        if (CheckCollisionLines(a, b, {r.x + r.width, r.y}, {r.x + r.width, r.y + r.height}, &collPoint))
            if (vec2distance(collPoint, a) < vec2distance(outputPoint, a))
                outputPoint = collPoint;
        if (CheckCollisionLines(a, b, {r.x + r.width, r.y + r.height}, {r.x, r.y + r.height}, &collPoint))
            if (vec2distance(collPoint, a) < vec2distance(outputPoint, a))
                outputPoint = collPoint;
        if (CheckCollisionLines(a, b, {r.x, r.y + r.height}, {r.x, r.y}, &collPoint))
            if (vec2distance(collPoint, a) < vec2distance(outputPoint, a))
                outputPoint = collPoint;
    }
    return outputPoint;
}

bool inEnclosedRegion(Coord a, const TileMap& map)
{
    std::vector<Coord> open{a};
    std::vector<Coord> closed{};

    if (a.x < 0 || a.x >= map.size() || a.y < 0 || a.y >= map[0].size())
        return false;
    if (map[a.x][a.y] == Tile::WALL) // the input coord is in a wall
        return true;

    while (true)
    {
        if (open.empty())
            return true;

        closed.push_back(open[open.size() - 1]);
        Coord current{closed[closed.size() - 1]};
        open.pop_back();

        std::vector<Coord> friends{
            Coord{current.x + 1, current.y},
            Coord{current.x - 1, current.y},
            Coord{current.x, current.y + 1},
            Coord{current.x, current.y - 1}
        };
        for (auto& f : friends)
        {
            if (f.x < 0 || f.x >= map.size() || f.y < 0 || f.y >= map[0].size())
                return false;
            
            if (map[f.x][f.y] == Tile::WALL || std::find(closed.begin(), closed.end(), f) != closed.end())
                continue;

            if (std::find(open.begin(), open.end(), f) == open.end())
            {
                open.push_back(f);
            }
        }
    }
}

bool inEnclosedRegionNextToCoord(Coord a, const TileMap& map)
{
    std::vector<Coord> friends{
        Coord{a.x + 1, a.y},
        Coord{a.x - 1, a.y},
        Coord{a.x, a.y + 1},
        Coord{a.x, a.y - 1}
    };
    for (auto& f : friends)
    {
        if (f.x < 0 || f.x >= map.size() || f.y < 0 || f.y >= map[0].size())
            continue;
        if (map[f.x][f.y] != Tile::WALL && inEnclosedRegion(f, map))
            return true;
    }
    return false;
}

struct Node
{
    Coord crd;
    Coord parent;
    int g;
    int h;
};

int getLowestFCost(const std::vector<Node>& a)
{
    int fcost{99999};
    int output{};
    for (int i{}; i < a.size(); ++i)
    {
        if (a[i].h + a[i].g < fcost)
        {
            fcost = a[i].h + a[i].g;
            output = i;
        }
    }
    return output;
}

int getHCost(Coord b, Coord node)
{
    return std::abs(b.x - node.x) + std::abs(b.y - node.y);
}

bool coordInNodeVector(const std::vector<Node>& a, Coord c)
{
    for (const auto& n : a)
    {
        if (n.crd == c)
            return true;
    }
    return false;
}

int getCoordInNodeVector(const std::vector<Node>& a, Coord c)
{
    for (int i{}; i < a.size(); ++i)
    {
        if (a[i].crd == c)
            return i;
    }
    return -1;
}

bool checkIfTheTileIsWall(Coord a, const TileMap& map)
{
    return !(a.x < 0 || a.x >= map.size() || a.y < 0 || a.y >= map[0].size()) && map[a.x][a.y] == Tile::WALL;
}

template<typename T>
void shuffleVector(std::vector<T>& v, int randomness)
{
    for (int i{}; i < randomness; ++i)
    {
        int a{GetRandomValue(0, v.size() - 1)};
        int b{GetRandomValue(0, v.size() - 1)};

        T tempStorage{v[a]};
        v[a] = v[b];
        v[b] = tempStorage;
    }
}

Coord aSNext(Coord a, Coord b, const TileMap& map) // look at sebastian league video for how this works
{
    std::vector<Node> open{Node{a, a, 0, getHCost(b, a)}};
    std::vector<Node> closed{};

    while (true) // this could be an infinite loop if node a and node b can't be connected
    {
        int lfco{getLowestFCost(open)};
        closed.push_back(open[lfco]);
        Node current{open[lfco]};
        open.erase(open.begin() + lfco);

        if (Coord{current.crd.x, current.crd.y} == b)
            break;

        Coord friends[4]{
            Coord{current.crd.x + 1, current.crd.y},
            Coord{current.crd.x - 1, current.crd.y},
            Coord{current.crd.x, current.crd.y + 1},
            Coord{current.crd.x, current.crd.y - 1}
        };
        for (auto& f : friends)
        {
            if (!(checkIfTheTileIsWall(f, map) || coordInNodeVector(closed, f)))
            {
                if (!coordInNodeVector(open, f))
                {
                    open.push_back(Node{f, current.crd, current.g + 1, getHCost(b, f)});
                    continue;
                }
                Node fNode{open[getCoordInNodeVector(open, f)]};
                if (current.g + 1 < fNode.g)
                {
                    fNode.parent = current.crd;
                    fNode.g = current.g + 1;
                }
            }
        }
    }
    // go back the parent chain
    Node outputNode{closed[closed.size() - 1]};
    while (true)
    {
        if (outputNode.parent == a)
        {
            return outputNode.crd;
        }
        outputNode = closed[getCoordInNodeVector(closed, outputNode.parent)];
    }
}

std::vector<Rectangle> getTileRecs(Tile t, const TileMap& tm)
{
    std::vector<Rectangle> output{};
    for (int x{}; x < tm.size(); ++x)
        for (int y{}; y < tm[x].size(); ++y)
            if (tm[x][y] == t)
                output.push_back({static_cast<float>(x * 16), static_cast<float>(y * 16), 16, 16});
    return output;
}

Coord wallNextToCoord(Coord a, const TileMap& tm)
{
    std::vector<Coord> friends{
        Coord{a.x + 1, a.y},
        Coord{a.x - 1, a.y},
        Coord{a.x, a.y + 1},
        Coord{a.x, a.y - 1}
    };
    for (auto& f : friends)
    {
        if (f.x < 0 || f.x >= tm.size() || f.y < 0 || f.y >= tm[0].size())
            continue;
        if (tm[f.x][f.y] == Tile::WALL)
            return f;
    }
    return {-1, -1};
}

void initialize(Map& m)
{
    for (int x{}; x < m.t.size(); ++x)
        for (int y{}; y < m.t[x].size(); ++y)
            m.t[x][y] = Tile::EMPTY;
}

void update(Map& m, bool apocalypse)
{
    for (int x{}; x < m.t.size(); ++x)
        for (int y{}; y < m.t[x].size(); ++y)
        {
            if (m.t[x][y] == Tile::VINE && !apocalypse)
            {
                m.i[x][y] += lowerLimitFrameTime() / 9; // grow 1/9 per second
                if (m.i[x][y] > 1)
                    m.i[x][y] = 1;
            }
            if (m.t[x][y] == Tile::MINE)
            {
                m.i[x][y] += lowerLimitFrameTime();
                if (m.i[x][y] > 1)
                    m.i[x][y] = 0;
            }
        }
}

void draw(const Map& m, bool apocalypse)
{
    for (int x{}; x < m.t.size(); ++x)
        for (int y{}; y < m.t[x].size(); ++y)
        {
            if (m.t[x][y] == Tile::WALL)
            {
                DrawTexture(txtrStrg().get("res/wall.png"), x * 16, y * 16, WHITE);
            }
            if (m.t[x][y] == Tile::VINE)
            {
                Coord wallNextToIt(wallNextToCoord({x, y}, m.t));
                Vector2 growPoint{((Vector2{static_cast<float>(wallNextToIt.x), static_cast<float>(wallNextToIt.y)} * 16 + Vector2{8, 8}) + (Vector2{static_cast<float>(x), static_cast<float>(y)} * 16 + Vector2{8, 8})) / 2};
                int growStage{static_cast<int>(m.i[x][y] * 2)};
                DrawTexturePro(txtrStrg().get("res/vine.png"),
                    {static_cast<float>(16 * growStage), 0, 16, 16},
                    {growPoint.x, growPoint.y, 16, 16},
                    {16, 8},
                    vec2ToAngle(Vector2{static_cast<float>(wallNextToIt.x), static_cast<float>(wallNextToIt.y)} * 16 - Vector2{static_cast<float>(x), static_cast<float>(y)} * 16),
                    (apocalypse ? ORANGE : (m.i[x][y] == 1 ? WHITE : YELLOW)));
            }
            if (m.t[x][y] == Tile::MINE)
            {
                Coord wallNextToIt(wallNextToCoord({x, y}, m.t));
                Vector2 growPoint{((Vector2{static_cast<float>(wallNextToIt.x), static_cast<float>(wallNextToIt.y)} * 16 + Vector2{8, 8}) + (Vector2{static_cast<float>(x), static_cast<float>(y)} * 16 + Vector2{8, 8})) / 2};
                DrawTexturePro(txtrStrg().get("res/mine.png"),
                    {0, 0, 16, 16},
                    {growPoint.x, growPoint.y, 16, 16},
                    {16, 8},
                    vec2ToAngle(Vector2{static_cast<float>(wallNextToIt.x), static_cast<float>(wallNextToIt.y)} * 16 - Vector2{static_cast<float>(x), static_cast<float>(y)} * 16),
                    ((m.i[x][y] > .5) ? WHITE : Color{50, 255, 255, 255}));
            }
            if (m.t[x][y] == Tile::TURRET)
            {
                DrawTexturePro(txtrStrg().get("res/turret.png"), {0, 0, 16, 16}, {static_cast<float>(x * 16 + 8), static_cast<float>(y * 16 + 8), 16, 16}, {8, 8}, m.i[x][y] - 90, WHITE);
            }
        }
}