#ifndef BATTLE_SCENE_H
#define BATTLE_SCENE_H

#include <sp2/scene/scene.h>
#include <sp2/graphics/gui/widget/widget.h>
#include <sp2/script/coroutine.h>

#include "item.h"


class Party;
class Character;
class BattleEntity : public sp::Node
{
public:
    BattleEntity(sp::P<sp::Node> parent, sp::Vector2d _pos, int tile_idx);
    virtual void onUpdate(float delta) override;

    float animation_time = 0.0f;
    sp::Vector2d neutral_position;
    sp::Vector2d idle_step_offset;
    enum class IdleState {
        Idle,
        StepForward,
        StepBack,
        Busy,
    } idle_state = IdleState::Idle;
    float turn_delay = 0.0f;

    sp::P<Party> party;
    int member_index;
    sp::P<Character> character;
};


class BattleScene : public sp::Scene
{
public:
    BattleScene();
    ~BattleScene();

    virtual void onEnable() override;
    virtual void onDisable() override;
    virtual void onUpdate(float delta) override;
    virtual void onFixedUpdate() override;
private:
    void createBattleEntities(sp::P<Party> party, double side);
    void findNextTurn();
    void buildItemList();
    void selectTarget(sp::Vector2d offset, Item::Target target_type);
    void prevTarget();
    sp::P<BattleEntity> randomTarget(sp::P<Party> party);

    sp::P<sp::gui::Widget> gui;
    sp::script::CoroutinePtr current_action;
    sp::PList<BattleEntity> battle_entities;

    enum class State {
        Wait,
        SelectAction,
        SelectTarget,
        Victory,
    } state = State::Wait;

    sp::P<BattleEntity> current_entity;
    sp::P<BattleEntity> current_target;

    sp::P<sp::Node> source_cursor;
    sp::P<sp::Node> target_cursor;
};

#endif//BATTLE_SCENE_H
