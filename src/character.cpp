#include "character.h"
#include "battlescene.h"
#include <sp2/graphics/fontManager.h>

class TextRain : public sp::Node
{
public:
    TextRain(sp::P<sp::Node> parent, const sp::string& text)
    : sp::Node(parent->getParent())
    {
        auto font = sp::font_manager.get("gui/theme/font.txt");
        render_data.mesh = font->createString(text, 32, 0.5, {0, 0}, sp::Alignment::Center);
        render_data.shader = sp::Shader::get("internal:basic.shader");
        render_data.type = sp::RenderData::Type::Normal;
        render_data.texture = font->getTexture(32);
        render_data.order = 10;
        render_data.color = sp::Color(0xFFF2E205U);

        velocity = {1.5, 3.5};
        if (parent->getPosition2D().x < 0)
            velocity.x = -velocity.x;
        setPosition(parent->getPosition2D() + sp::Vector2d{0, 0.5});
    }

    virtual void onUpdate(float delta) override {
        setPosition(getPosition2D() + velocity * double(delta));
        velocity.y -= delta * 10.0;
        velocity.x *= std::pow(0.8, delta);
        if (velocity.y < -8.0)
            delete this;
    }

    sp::Vector2d velocity;
};


Character::Character(const sp::string& script_name)
{
    sp::script::Environment env;
    env.setGlobal("this", this);
    env.load(script_name).value();
    hp = active_stats.max_hp;
    mp = active_stats.max_mp;
}

Character::~Character()
{
    for(auto item : items)
        item.destroy();
}

void Character::nextItem()
{
    bool next = false;
    for(auto item : items) {
        if (next) {
            current_item = item;
            return;
        }
        if (item == current_item)
            next = true;
    }
    current_item = *items.begin();
}

void Character::prevItem()
{
    sp::P<Item> prev = nullptr;
    for(auto item : items) {
        if (item == current_item && prev) {
            current_item = prev;
            return;
        }
        prev = item;
    }
    current_item = prev;
}

void Character::onRegisterScriptBindings(sp::script::BindingClass& script_binding_class)
{
    script_binding_class.bind("setPosition", &Character::setPos);
    script_binding_class.bind("getPosition", &Character::getPos);
    script_binding_class.bind("getFrontPosition", &Character::getFrontPos);
    script_binding_class.bind("textRain", &Character::createTextRain);
    script_binding_class.bind("addItem", &Character::addItem);
    script_binding_class.bind("addBuff", &Character::addBuff);
    script_binding_class.bindProperty("name", name);
    script_binding_class.bindProperty("icon", icon);
    script_binding_class.bindProperty("hp", &Character::getHP, &Character::setHP);
    script_binding_class.bindProperty("mp", &Character::getMP, &Character::setMP);
    script_binding_class.bindProperty("max_hp", &Character::getMaxHP, &Character::setMaxHP);
    script_binding_class.bindProperty("max_mp", &Character::getMaxHP, &Character::setMaxMP);
    script_binding_class.bindProperty("strength", &Character::getStrength, &Character::setStrength);
    script_binding_class.bindProperty("agility", &Character::getAgility, &Character::setAgility);

    script_binding_class.bindProperty("defence", &Character::getDefence, &Character::setDefence);
    script_binding_class.bindProperty("evasion", &Character::getEvasion, &Character::setEvasion);
}

void Character::setPos(sp::Vector2d position)
{
    battle_entity->setPosition(position);
}

sp::Vector2d Character::getPos()
{
    return battle_entity->neutral_position;
}

sp::Vector2d Character::getFrontPos()
{
    if (battle_entity->neutral_position.x > 0)
        return battle_entity->neutral_position - sp::Vector2d{1.5, 0.0};
    return battle_entity->neutral_position + sp::Vector2d{1.5, 0.0};
}

void Character::createTextRain(sp::string text)
{
    new TextRain(battle_entity, text);
}

float Character::speed()
{
    return 30 + active_stats.agility;
}

void Character::addItem(sp::string item)
{
    items.add(new Item(item));
}

int Character::addBuff(lua_State* L)
{
    sp::string name = luaL_checkstring(L, 1);
    if (buffs.find(name) != buffs.end()) // Buff already exists
        return 0;

    if (!lua_istable(L, 2)) {
        return luaL_error(L, "2nd argument isn't a table of stat buffs");
    }

    if (!lua_isfunction(L, 3)) {
        return luaL_error(L, "3th argument of addBuff not a function");
    }
    lua_State* L2 = lua_newthread(L);
    lua_pushvalue(L, 3);
    lua_xmove(L, L2, 1);

    int result = lua_resume(L2, nullptr, 0);
    if (result != LUA_YIELD)
    {
        if (result != LUA_OK) {
            LOG(Error, "Lua error in buff function:", lua_tostring(L2, -1));
        } else {
            LOG(Error, "Buff function did not yield, no getting a buff.");
        }
        // Didn't yield, so no buff to apply.
        lua_pop(L, 1);//pop the coroutine off the main stack.
        return 0;
    }
    auto coroutine = std::make_shared<sp::script::Coroutine>(L, L2);
    auto& buff = buffs[name];
    buff.func = coroutine;

    // Add/sub fixed number {"max_hp": 2}
    // Ratio number {"max_hp": {10}} (+10%)
    // Add and Ratio number {"max_hp": {2, 10}} (+2, +10%)
    auto parse = [L](const char* key, int& add, int& mul) {
        lua_getfield(L, 2, key);
        if (lua_istable(L, -1)) {
            lua_geti(L, -1, 2);
            if (lua_isnil(L, -1)) { // = {10}
                lua_pop(L, 1);
                lua_geti(L, -1, 1);
                add = 0;
                mul = lua_tointeger(L, -1);
                lua_pop(L, 1);
            } else { // = {2, 10}
                mul = lua_tointeger(L, -1);
                lua_pop(L, 1);
                lua_geti(L, -1, 1);
                add = lua_tointeger(L, -1);
                lua_pop(L, 1);
            }
        } else {
            add = lua_tointeger(L, -1);
            mul = 0;
        }
        lua_pop(L, 1);
    };
    parse("max_hp", buff.stats_add.max_hp, buff.stats_mul.max_hp);
    parse("max_mp", buff.stats_add.max_mp, buff.stats_mul.max_mp);

    parse("strength", buff.stats_add.strength, buff.stats_mul.strength);
    parse("agility", buff.stats_add.agility, buff.stats_mul.agility);
    parse("intelligence", buff.stats_add.intelligence, buff.stats_mul.intelligence);
    parse("stamina", buff.stats_add.stamina, buff.stats_mul.stamina);

    parse("defense", buff.stats_add.defense, buff.stats_mul.defense);
    parse("evasion", buff.stats_add.evasion, buff.stats_mul.evasion);

    lua_pushboolean(L, true);
    return 1;
}

void Character::tickBuffs()
{
    for(auto& [key, buff] : buffs) {
        if (!buff.func->resume().value())
            buff.func = nullptr;
    }
    for(auto it = std::begin(buffs); it != std::end(buffs);)
    {
        if (it->second.func == nullptr)
            it = buffs.erase(it);
        else
            ++it;
    }

    recalculate();
}

void Character::recalculate()
{
    active_stats = base_stats;
    Stats add;
    Stats mul;
    for(auto& [key, buff] : buffs) {
        add += buff.stats_add;
        mul += buff.stats_mul;
    }
    active_stats += active_stats * mul / 100;
    active_stats += add;

    active_stats.evasion += active_stats.agility / 2;
    hp = std::clamp(hp, 0, active_stats.max_hp);
    mp = std::clamp(mp, 0, active_stats.max_mp);
}
