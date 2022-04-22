#pragma once

#include <sp2/pointerList.h>
#include <sp2/script/coroutine.h>
#include "item.h"


class BattleEntity;
class Stats {
public:
    int max_hp = 0;
    int max_mp = 0;

    int strength = 0;
    int agility = 0;
    int intelligence = 0;
    int stamina = 0;

    int defense = 0;
    int evasion = 0;

    int threat = 0;

    Stats& operator+=(const Stats& other) {
        max_hp += other.max_hp;
        max_mp += other.max_mp;
        strength += other.strength;
        agility += other.agility;
        intelligence += other.intelligence;
        stamina += other.stamina;
        defense += other.defense;
        evasion += other.evasion;
        threat += other.threat;
        return *this;
    }
    Stats operator*(const Stats& other) {
        return {
            max_hp * other.max_hp,
            max_mp * other.max_mp,

            strength * other.strength,
            agility * other.agility,
            intelligence * other.intelligence,
            stamina * other.stamina,

            defense * other.defense,
            evasion * other.evasion,

            threat * other.threat,
        };
    }
    Stats operator/(int other) {
        return {
            max_hp / other,
            max_mp / other,

            strength / other,
            agility / other,
            intelligence / other,
            stamina / other,

            defense / other,
            evasion / other,

            threat / other,
        };
    }
};
class Buff
{
public:
    Stats stats_add;
    Stats stats_mul;
    sp::script::CoroutinePtr func;
};
class Character : public sp::script::BindingObject
{
public:
    Character(const sp::string& script_name);
    ~Character();

    virtual void onRegisterScriptBindings(sp::script::BindingClass& script_binding_class) override;

    void nextItem();
    void prevItem();

    void setPos(sp::Vector2d);
    sp::Vector2d getPos();
    sp::Vector2d getFrontPos();
    void createTextRain(sp::string text);
    int getHP() const { return hp; }
    void setHP(int value) { hp = std::clamp(value, 0, active_stats.max_hp); }
    int getMaxHP() const { return active_stats.max_hp; }
    void setMaxHP(int value) { base_stats.max_hp = value; recalculate(); }
    int getMP() const { return mp; }
    void setMP(int value) { mp = std::clamp(value, 0, active_stats.max_mp); }
    int getMaxMP() const { return active_stats.max_mp; }
    void setMaxMP(int value) { base_stats.max_mp = value; recalculate(); }
    int getStrength() const { return active_stats.strength; }
    void setStrength(int value) { base_stats.strength = value; recalculate(); }
    int getAgility() const { return active_stats.agility; }
    void setAgility(int value) { base_stats.agility = value; recalculate(); }
    int getDefence() const { return active_stats.defense; }
    void setDefence(int value) { base_stats.defense = value; recalculate(); }
    int getEvasion() const { return active_stats.evasion; }
    void setEvasion(int value) { base_stats.evasion = value; recalculate(); }
    int getThreat() const { return active_stats.threat; }
    void setThreat(int value) { base_stats.threat = value; recalculate(); }
    void addItem(sp::string item);

    int addBuff(lua_State* L);
    void tickBuffs();
    void clearBuffs() { buffs.clear(); recalculate(); }

    float speed();
    void recalculate();

    sp::string name;
    int icon;

    int hp;
    int mp;
    Stats base_stats;
    Stats active_stats;
    std::unordered_map<sp::string, Buff> buffs;

    sp::PList<Item> items;
    sp::P<Item> current_item;
    sp::P<BattleEntity> battle_entity;
};
