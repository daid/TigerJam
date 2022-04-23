#include "mainScene.h"
#include "ingameMenu.h"
#include "main.h"
#include "party.h"
#include "battlescene.h"
#include "luafunc.h"

#include <sp2/graphics/gui/loader.h>
#include <sp2/audio/sound.h>
#include <sp2/random.h>
#include <sp2/engine.h>
#include <sp2/scene/camera.h>
#include <sp2/graphics/textureManager.h>
#include <sp2/graphics/meshdata.h>
#include <sp2/graphics/gui/widget/image.h>
#include <sp2/io/resourceProvider.h>
#include <sp2/scene/tilemap.h>
#include <sp2/tween.h>

#include <nlohmann/json.hpp>

sp::P<sp::Tilemap> tilemap;
class MapPlayer;
sp::P<MapPlayer> map_player;
std::unordered_map<sp::string, sp::Vector2i> map_spots;
std::unordered_map<sp::Vector2i, sp::string> map_triggers;
std::unordered_map<int, bool> tile_type_blocked;
sp::P<sp::gui::Widget> messagebox;
sp::string messagebox_message;
float messagebox_progress = 0.0f;
std::queue<sp::string> script_queue;
std::queue<sp::string> script_function_queue;
class MapEntity;
sp::InfiniGrid<sp::P<MapEntity>> map_entities{nullptr};

sp::P<sp::gui::Widget> menu;

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
        map_entities.set(pos, this);
    }

    virtual ~MapEntity()
    {
        map_entities.clear(pos);
    }

    bool move(sp::Vector2i _pos, float move_speed=6.0)
    {
        if (map_entities.get(_pos)) {
            return false;
        }
        map_entities.clear(pos);
        sp::Vector2d start = {pos.x + 0.5, pos.y + 0.5};
        pos = _pos;
        sp::Vector2d target = {pos.x + 0.5, pos.y + 0.5};
        map_entities.set(pos, this);

        queue([this, start, target, move_speed](float time) {
            time *= move_speed;
            setPosition(sp::Tween<sp::Vector2d>::easeInOutCubic(time, 0.0, 1.0, start, target) +
                sp::Vector2d(0, std::max(0.0, 0.1 * std::sin(time * sp::pi))));
            return time >= 1.0f;
        });
        return true;
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

    bool animationBusy() const
    {
        return !animations.empty();
    }

    sp::Vector2i getPos()
    {
        return pos;
    }

    void setPos(sp::Vector2i p)
    {
        map_entities.clear(pos);
        pos = p;
        map_entities.set(pos, this);
        setPosition(sp::Vector2d{pos.x + 0.5, pos.y + 0.5});
        animations.clear();
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

class NpcEntity : public MapEntity
{
public:
    NpcEntity(sp::P<sp::Node> parent, sp::Vector2i pos, int tile)
    : MapEntity(parent, pos, tile)
    {
    }

    void moveRel(int x, int y)
    {
        move(getPos() + sp::Vector2i{x, y});
    }

    void destroy()
    {
        destroy_me = true;
    }

    virtual void onFixedUpdate() override
    {
        if (destroy_me) delete this;
    }

    virtual void onRegisterScriptBindings(sp::script::BindingClass& binding_class) override
    {
        binding_class.bind("move", &NpcEntity::moveRel);
        binding_class.bind("destroy", &NpcEntity::destroy);
        binding_class.bind("onbump", onbump);
    }

    bool destroy_me = false;
    sp::script::Callback onbump;
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
        if (map_triggers.find(target) != map_triggers.end()) {
            script_function_queue.push(map_triggers[target]);
        }
        sp::P<NpcEntity> npc = map_entities.get(target);
        if (npc) {
            sp::P<Scene> scene = sp::Scene::get("MAIN");
            scene->active_sequence = npc->onbump.callCoroutine().value();
            return false;
        }


        if (target.x < 0 || target.x >= 20)
            return false;
        if (target.y < 0 || target.y >= 18)
            return false;
        auto it = tile_type_blocked.find(tilemap->getTileIndex(target) & 0xFFFF);
        if (it != tile_type_blocked.end())
        {
            if (it->second) {
                return false;
            }
        }
        return move(target);
    }
};

void luaLoadmap(sp::string mapname, sp::string startpoint)
{
    for(auto e : sp::Scene::get("MAIN")->getRoot()->getChildren()) {
        sp::P<NpcEntity> npc = e;
        npc.destroy();
    }
    tile_type_blocked.clear();
    map_spots.clear();
    map_triggers.clear();

    auto json = nlohmann::json::parse(sp::io::ResourceProvider::get(mapname + ".json")->readAll());
    for(const auto& layer : json["layers"]) {
        if (static_cast<std::string>(layer["type"]) == "tilelayer") {
            int w = layer["width"];
            int h = layer["height"];
            for(int y=0; y<h; y++) {
                for(int x=0; x<w; x++) {
                    auto i = uint32_t(layer["data"][x + y * 20]);
                    if (i > 0) {
                        int flags = 0;
                        if (i & 0x20000000)
                            flags |= sp::Tilemap::flip_diagonal;
                        if (i & 0x40000000)
                            flags |= sp::Tilemap::flip_vertical;
                        if (i & 0x80000000)
                            flags |= sp::Tilemap::flip_horizontal;
                        tilemap->setTile({x, h - 1 - y}, ((i & 0xFFFF) - 1) | flags);
                    }
                    else
                    {
                        tilemap->setTile({x, h - 1 - y}, -1);
                    }
                }
            }
        }
        if (static_cast<std::string>(layer["type"]) == "objectgroup") {
            for(const auto& obj : layer["objects"]) {
                int x = static_cast<float>(obj["x"]) + static_cast<float>(obj["width"]) * 0.5f;
                int y = static_cast<float>(obj["y"]) + static_cast<float>(obj["height"]) * 0.5f;
                auto name = static_cast<std::string>(obj["name"]);
                x = x / 16;
                y = 17 - y / 16;
                map_spots[name] = {x, y};
            }
        }
    }

    if (map_spots.find(startpoint) != map_spots.end()) {
        map_player->setPos(map_spots[startpoint]);
    } else {
        LOG(Warning, "player start point: ", startpoint, " not found");
    }

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

    script_queue.push(mapname + ".lua");
}

int luaMovePlayer(lua_State* L)
{
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    float speed = luaL_optnumber(L, 3, 4.0f);
    map_player->move(map_player->getPos() + sp::Vector2i{x, y}, speed);
    return lua_yield(L, 0);
}

int luaMessage(lua_State* L)
{
    messagebox = sp::gui::Loader::load("gui/message.gui", "MESSAGE");
    messagebox_message = luaL_checkstring(L, 1);
    messagebox_progress = 0.0;
    return lua_yield(L, 0);
}

sp::P<NpcEntity> luaNewNPC(sp::string spot, int tile)
{
    if (map_spots.find(spot) != map_spots.end()) {
        return new NpcEntity(sp::Scene::get("MAIN")->getRoot(), map_spots[spot], tile);
    }
    return nullptr;
}

int luaBattle(lua_State* L)
{
    std::vector<sp::string> enemies;
    for(int n=0; n<lua_gettop(L); n++)
        enemies.push_back(luaL_checkstring(L, n + 1));
    sp::P<Scene> scene = sp::Scene::get("MAIN");
    scene->startBattle(enemies);
    return 0;//return lua_yield(L, 0); <- this corrupts heap, not sure why...
}

void luaOnMove(sp::string target, sp::string functionname)
{
    if (map_spots.find(target) == map_spots.end()) {
        LOG(Error, "Map spot not found for trigger: ", target);
        return;
    }
    map_triggers[map_spots[target]] = functionname;
}

Scene::Scene()
: sp::Scene("MAIN")
{
    sp::Scene::get("INGAME_MENU")->enable();

    player_party.destroy();
    player_party = new Party();
    player_party->members[0] = new Character("character/player/fighter.lua");
    player_party->members[1] = new Character("character/player/rogue.lua");
    player_party->members[2] = new Character("character/player/priest.lua");

    registerLuaFuncs(script_env);
    script_env.setGlobal("loadmap", luaLoadmap);
    script_env.setGlobal("moveplayer", luaMovePlayer);
    script_env.setGlobal("message", luaMessage);
    script_env.setGlobal("newnpc", luaNewNPC);
    script_env.setGlobal("onmove", luaOnMove);
    script_env.setGlobal("battle", luaBattle);

    tilemap = new sp::Tilemap(getRoot(), "tiles.png", 1.0, 1.0, 49, 22);
    tilemap->setTile({0, 0}, 1);
    tilemap->setTilemapSpacingMargin(0.01, 0.0);

    auto camera = new sp::Camera(getRoot());
    camera->setOrtographic({10, 9});
    camera->setPosition({10, 9});
    setDefaultCamera(camera);

    map_player = new MapPlayer(getRoot(), {8, 8});

    script_env.setGlobal("randomencounter", nullptr);
    active_sequence = script_env.loadCoroutine("start.lua").value();
}

Scene::~Scene()
{
    sp::Scene::get("INGAME_MENU")->disable();
    sp::Scene::get("BATTLE").destroy();
    messagebox.destroy();
    menu.destroy();
}

void Scene::onFixedUpdate()
{
    if (messagebox)
        messagebox->getWidgetWithID("NEXT")->setVisible(messagebox_progress > messagebox_message.length());
}

void Scene::onUpdate(float delta)
{
    auto constexpr battle_start_time = 0.7f;
    auto constexpr battle_end_time = 0.5f;
    auto camera = getCamera();
    switch(state)
    {
    case State::Normal:
        if (messagebox)
        {
            messagebox_progress += delta * 8.0f;
            messagebox->getWidgetWithID("TEXT")->setAttribute("caption", messagebox_message.substr(0, messagebox_progress));
            if (controller.primary_action.getDown()) {
                if (messagebox_progress > messagebox_message.length()) {
                    messagebox.destroy();
                } else {
                    messagebox_progress = messagebox_message.length();
                }
            }
        }
        else if (active_sequence)
        {
            if (!map_player->animationBusy()) {
                if (!active_sequence->resume().value())
                    active_sequence = nullptr;
            }
        }
        else if (!script_queue.empty())
        {
            if (!map_player->animationBusy()) {
                active_sequence = script_env.loadCoroutine(script_queue.front()).value();
                script_queue.pop();
            }
        }
        else if (!script_function_queue.empty())
        {
            if (!map_player->animationBusy()) {
                active_sequence = script_env.callCoroutine(script_function_queue.front()).value();
                script_function_queue.pop();
            }
        }
        else if (menu)
        {
            //Do nothing, menu is in control
        }
        else if (controller.primary_action.getDown())
        {
            menu = sp::gui::Loader::load("gui/menu.gui", "MAIN");
            for(auto n=0U; n<player_party->members.size(); n++) {
                auto info = menu->getWidgetWithID("INFO" + sp::string(n));
                auto member = player_party->members[n];
                if (!member) {
                    info->hide();
                } else {
                    info->getWidgetWithID("NAME")->setAttribute("caption", member->name);
                    sp::P<sp::gui::Image> icon = info->getWidgetWithID("ICON");
                    icon->setAttribute("texture", "tiles.png");
                    icon->setUV(tileUV(member->icon));
                    info->getWidgetWithID("HP")->setAttribute("caption", "HP:" + sp::string(member->hp) + "/" + sp::string(member->active_stats.max_hp));
                    info->getWidgetWithID("MP")->setAttribute("caption", "MP:" + sp::string(member->mp) + "/" + sp::string(member->active_stats.max_mp));
                }
            }
            menu->getWidgetWithID("EXIT")->setEventCallback([this](sp::Variant v) {
                menu.destroy();
                state = State::Delay;
            });
        }
        else if (map_player->doAction())
        {
            steps_till_battle -= 1;
            if (steps_till_battle <= 0 && script_function_queue.empty() && script_queue.empty()) {
                active_sequence = script_env.callCoroutine("randomencounter").value();
                steps_till_battle = sp::irandom(5, 15);
            }
        }
        break;
    case State::BattleStart:
        if (map_player->animationBusy())
            break;
        if (state_timer == 0.0f && delta > 0.0f)
            sp::audio::Sound::play("map_attacked.wav");
        state_timer += delta;
        camera->setOrtographic(sp::Tween<sp::Vector2d>::easeOutQuad(state_timer, 0.0, battle_start_time, {10, 9}, {1, 1}));
        camera->setPosition(sp::Tween<sp::Vector2d>::easeOutQuad(state_timer, 0.0, battle_start_time, {10, 9}, map_player->getPosition2D()));
        if (state_timer >= battle_start_time) {
            sp::Scene::get("BATTLE").destroy();
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
    case State::Delay:
        //Sometimes we just need to skip a cycle (usually for double input prevention)
        state = State::Normal;
        state_timer = 0.0f;
        break;
    }
}

void Scene::startBattle(const std::vector<sp::string>& enemies)
{
    if (enemies.size() < 1)
        return;
    enemy_party.destroy();
    enemy_party = new Party();
    for(unsigned int n=0; n<enemies.size() && n<enemy_party->members.size(); n++) {
        enemy_party->members[n] = new Character("character/" + enemies[n] + ".lua");
    }

    state = State::BattleStart;
    state_timer = 0.0f;
}
