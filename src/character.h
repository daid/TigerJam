#pragma once

#include <sp2/pointerList.h>
#include "item.h"


class BattleEntity;
class Stats {
public:
    int max_hp = 10;

    int strength = 0;
    int agility = 0;
    int intelligence = 0;
    int stamina = 0;

    int evasion = 0;
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
    int getStrength() const { return active_stats.strength; }
    void setStrength(int value) { base_stats.strength = value; recalculate(); }
    int getAgility() const { return active_stats.agility; }
    void setAgility(int value) { base_stats.agility = value; recalculate(); }
    int getDefence() const { return 0; }
    void setDefence(int value) { }
    int getEvasion() const { return active_stats.evasion; }
    void setEvasion(int value) { base_stats.evasion = value; recalculate(); }
    void addItem(sp::string item);

    float speed();
    void recalculate();

    sp::string name;
    int icon;

    int hp;
    Stats base_stats;
    Stats active_stats;

    sp::PList<Item> items;
    sp::P<Item> current_item;
    sp::P<BattleEntity> battle_entity;
};
