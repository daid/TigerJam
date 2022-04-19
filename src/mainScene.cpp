#include "mainScene.h"
#include "ingameMenu.h"
#include "main.h"
#include "party.h"
#include "battlescene.h"

#include <sp2/graphics/gui/loader.h>
#include <sp2/audio/sound.h>
#include <sp2/random.h>
#include <sp2/engine.h>
#include <sp2/scene/camera.h>
#include <sp2/graphics/textureManager.h>
#include <sp2/graphics/meshdata.h>
#include <sp2/io/resourceProvider.h>
#include <sp2/scene/tilemap.h>
#include <sp2/tween.h>

#include <nlohmann/json.hpp>

sp::P<sp::Tilemap> tilemap;
class MapPlayer;
sp::P<MapPlayer> map_player;
std::unordered_map<int, bool> tile_type_blocked;

class MapEntity : public sp::Node
{
public:
    MapEntity(sp::P<sp::Node> parent, sp::Vector2i _pos, int tile_idx)
    : sp::Node(parent), pos(_pos)
    {
        auto tile_sx = 49;
        auto tile_sy = 22;
        auto u = 1.0f / float(tile_sx);
        auto v = 1.0f / float(tile_sy);
        render_data.mesh = sp::MeshData::createQuad({1, 1},
            {(tile_idx % tile_sx) * u, (tile_idx / tile_sx) * v},
            {(tile_idx % tile_sx) * u + u, (tile_idx / tile_sx) * v + v});
        render_data.shader = sp::Shader::get("internal:basic.shader");
        render_data.type = sp::RenderData::Type::Normal;
        render_data.texture = sp::texture_manager.get("tiles.png");
        setPosition({pos.x + 0.5, pos.y + 0.5});
        //map[pos.x][pos.y].entities.add(this);
    }

    void move(sp::Vector2i _pos)
    {
        //map[pos.x][pos.y].entities.remove(this);
        sp::Vector2d start = {pos.x + 0.5, pos.y + 0.5};
        pos = _pos;
        sp::Vector2d target = {pos.x + 0.5, pos.y + 0.5};
        //map[pos.x][pos.y].entities.add(this);

        queue([this, start, target](float time) {
            time *= 8.0f;
            setPosition(sp::Tween<sp::Vector2d>::easeInOutCubic(time, 0.0, 1.0, start, target) +
                sp::Vector2d(0, std::max(0.0, 0.1 * std::sin(time * sp::pi))));
            return time >= 1.0f;
        });
    }

    virtual void onUpdate(float delta) override
    {
        if (!animations.empty())
        {
            animation_time += delta * animations.size();
            if (animations[0](animation_time))
            {
                animations.erase(animations.begin());
                animation_time = 0.0f;
            }
        }
    }

    sp::Vector2i getPos()
    {
        return pos;
    }

    void queue(const std::function<bool(float)>& f)
    {
        animations.emplace_back(f);
    }

    virtual void act() {}
private:
    sp::Vector2i pos;
    float animation_time = 0.0f;
    std::vector<std::function<bool(float)>> animations;
};

class MapPlayer : public MapEntity
{
public:
    MapPlayer(sp::P<sp::Node> parent, sp::Vector2i pos)
    : MapEntity(parent, pos, 26)
    {
    }

    bool doAction()
    {
        if (controller.left.getDown())
            return trymove(getPos() + sp::Vector2i{-1, 0});
        if (controller.right.getDown())
            return trymove(getPos() + sp::Vector2i{1, 0});
        if (controller.up.getDown())
            return trymove(getPos() + sp::Vector2i{0, 1});
        if (controller.down.getDown())
            return trymove(getPos() + sp::Vector2i{0, -1});
        return false;
    }

    bool trymove(sp::Vector2i target)
    {
        if (target.x < 0 || target.x >= 20)
            return false;
        if (target.y < 0 || target.y >= 18)
            return false;
        auto it = tile_type_blocked.find(tilemap->getTileIndex(target) & 0xFFFF);
        if (it != tile_type_blocked.end())
        {
            if (it->second)
                return false;
        }
        move(target);
        return true;
    }
};

Scene::Scene()
: sp::Scene("MAIN")
{
    sp::Scene::get("INGAME_MENU")->enable();

    player_party.destroy();
    player_party = new Party();
    player_party->members[0] = new Character("character/player/fighter.lua");
    player_party->members[1] = new Character("character/player/rogue.lua");
    player_party->members[2] = new Character("character/player/priest.lua");

    tilemap = new sp::Tilemap(getRoot(), "tiles.png", 1.0, 1.0, 49, 22);
    tilemap->setTile({0, 0}, 1);
    tilemap->setTilemapSpacingMargin(0.01, 0.0);

    auto camera = new sp::Camera(getRoot());
    camera->setOrtographic({10, 9});
    camera->setPosition({10, 9});
    setDefaultCamera(camera);

    auto json = nlohmann::json::parse(sp::io::ResourceProvider::get("map00.json")->readAll());
    for(int y=0; y<18; y++) {
        for(int x=0; x<20; x++) {
            auto i = uint32_t(json["layers"][0]["data"][x + y * 20]);
            if (i > 0) {
                int flags = 0;
                if (i & 0x20000000)
                    flags |= sp::Tilemap::flip_diagonal;
                if (i & 0x40000000)
                    flags |= sp::Tilemap::flip_vertical;
                if (i & 0x80000000)
                    flags |= sp::Tilemap::flip_horizontal;
                tilemap->setTile({x, 17 - y}, ((i & 0xFFFF) - 1) | flags);
            }
        }
    }
    tile_type_blocked.clear();
    for(const auto& tile : json["tilesets"][0]["tiles"]) {
        int id = tile["id"];
        for(const auto& prop : tile["properties"]) {
            std::string name = prop["name"];
            int value = prop["value"];
            if (name == "solid" && value) {
                tile_type_blocked[id] = true;
            }
        }
    }

    map_player = new MapPlayer(getRoot(), {8, 8});
}

Scene::~Scene()
{
    sp::Scene::get("INGAME_MENU")->disable();
}

void Scene::onUpdate(float delta)
{
    auto constexpr battle_start_time = 0.7f;
    auto constexpr battle_end_time = 0.5f;
    auto camera = getCamera();
    switch(state)
    {
    case State::Normal:
        if (map_player->doAction())
        {
            steps_till_battle -= 1;
            if (steps_till_battle <= 0) {
                state = State::BattleStart;
                state_timer = 0.0f;
                sp::audio::Sound::play("map_attacked.wav");
                steps_till_battle = sp::irandom(5, 15);
            }
        }
        break;
    case State::BattleStart:
        state_timer += delta;
        camera->setOrtographic(sp::Tween<sp::Vector2d>::easeOutQuad(state_timer, 0.0, battle_start_time, {10, 9}, {1, 1}));
        camera->setPosition(sp::Tween<sp::Vector2d>::easeOutQuad(state_timer, 0.0, battle_start_time, {10, 9}, map_player->getPosition2D()));
        if (state_timer >= battle_start_time) {
            if (sp::Scene::get("BATTLE"))
                sp::Scene::get("BATTLE")->enable();
            else
                new BattleScene();
            disable();
            state = State::BattleEnd;
            state_timer = 0.0f;
        }
        break;
    case State::BattleEnd:
        state_timer += delta;
        camera->setOrtographic(sp::Tween<sp::Vector2d>::easeOutQuad(state_timer, battle_end_time, 0.0, {10, 9}, {1, 1}));
        camera->setPosition(sp::Tween<sp::Vector2d>::easeOutQuad(state_timer, battle_end_time, 0.0, {10, 9}, map_player->getPosition2D()));
        if (state_timer >= battle_end_time) {
            state = State::Normal;
            state_timer = 0.0f;
        }
        break;
    }
}
