#include "battlescene.h"
#include "main.h"
#include "party.h"
#include "item.h"

#include <sp2/random.h>
#include <sp2/scene/node.h>
#include <sp2/scene/camera.h>
#include <sp2/graphics/meshdata.h>
#include <sp2/graphics/meshbuilder.h>
#include <sp2/graphics/textureManager.h>
#include <sp2/graphics/gui/loader.h>
#include <sp2/graphics/gui/widget/image.h>
#include <sp2/audio/music.h>
#include <sp2/tween.h>


BattleEntity::BattleEntity(sp::P<sp::Node> parent, sp::Vector2d _pos, int tile_idx)
: sp::Node(parent)
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
    setPosition(_pos);

    neutral_position = _pos;
    animation_time = sp::random(0.0, 4.0f);
    idle_step_offset = {_pos.x < 0 ? 0.2 : -0.2, 0.0};
}

void BattleEntity::onUpdate(float delta) {
    animation_time += delta;

    auto constexpr idleStepTime = 0.5f;
    auto constexpr idleStepDelay = 4.0f;
    switch(idle_state)
    {
    case IdleState::Idle:
        if (animation_time > idleStepDelay)
        {
            animation_time = 0.0;
            idle_state = IdleState::StepForward;
        }
        setPosition(neutral_position);
        setRotation(std::sin(animation_time / idleStepDelay * 4 * sp::pi) * 5.0);
        break;
    case IdleState::StepForward:
        setPosition(neutral_position +
            sp::Tween<sp::Vector2d>::easeInOutCubic(animation_time, 0.0, idleStepTime, {0.0, 0.0}, idle_step_offset) +
            sp::Vector2d(0, std::max(0.0, 0.1 * std::sin(animation_time / idleStepTime * sp::pi))));
        if (animation_time > idleStepTime) {
            animation_time = 0.0f;
            idle_state = IdleState::StepBack;
        }
        break;
    case IdleState::StepBack:
        setPosition(neutral_position +
            sp::Tween<sp::Vector2d>::easeInOutCubic(animation_time, idleStepTime, 0.0, {0.0, 0.0}, idle_step_offset) +
            sp::Vector2d(0, std::max(0.0, 0.1 * std::sin(animation_time / idleStepTime * sp::pi))));
        if (animation_time > idleStepTime) {
            animation_time = sp::random(0.0, 0.5);
            idle_state = IdleState::Idle;
        }
        break;
    case IdleState::Busy:
        break;
    }
}

class BattleCursor : public sp::Node
{
public:
    BattleCursor(sp::P<sp::Node> parent)
    : sp::Node(parent) {
        auto tile_idx = 979;
        auto tile_sx = 49;
        auto tile_sy = 22;
        auto u = 1.0f / float(tile_sx);
        auto v = 1.0f / float(tile_sy);

        sp::Vector2d uv0 = {(tile_idx % tile_sx) * u, (tile_idx / tile_sx) * v};
        sp::Vector2d uv1 = uv0 + sp::Vector2d{u * 0.5, v * 0.5};

        sp::MeshData::Vertices vertices;
        sp::MeshData::Indices indices{
            0,1,2,2,1,3,
            4,5,6,6,5,7,
            8,9,10,10,9,11,
            12,13,14,14,13,15,
        };
        vertices.reserve(4);

        float a = 0.75f;
        float b = 0.25f;
        
        vertices.emplace_back(sp::Vector3f(-a, -a, 0.0f), sp::Vector2f(uv0.x, uv0.y));
        vertices.emplace_back(sp::Vector3f(-b, -a, 0.0f), sp::Vector2f(uv1.x, uv0.y));
        vertices.emplace_back(sp::Vector3f(-a, -b, 0.0f), sp::Vector2f(uv0.x, uv1.y));
        vertices.emplace_back(sp::Vector3f(-b, -b, 0.0f), sp::Vector2f(uv1.x, uv1.y));

        vertices.emplace_back(sp::Vector3f( b, -a, 0.0f), sp::Vector2f(uv1.x, uv0.y));
        vertices.emplace_back(sp::Vector3f( a, -a, 0.0f), sp::Vector2f(uv0.x, uv0.y));
        vertices.emplace_back(sp::Vector3f( b, -b, 0.0f), sp::Vector2f(uv1.x, uv1.y));
        vertices.emplace_back(sp::Vector3f( a, -b, 0.0f), sp::Vector2f(uv0.x, uv1.y));

        vertices.emplace_back(sp::Vector3f(-b,  a, 0.0f), sp::Vector2f(uv1.x, uv0.y));
        vertices.emplace_back(sp::Vector3f(-a,  a, 0.0f), sp::Vector2f(uv0.x, uv0.y));
        vertices.emplace_back(sp::Vector3f(-b,  b, 0.0f), sp::Vector2f(uv1.x, uv1.y));
        vertices.emplace_back(sp::Vector3f(-a,  b, 0.0f), sp::Vector2f(uv0.x, uv1.y));

        vertices.emplace_back(sp::Vector3f( a,  a, 0.0f), sp::Vector2f(uv0.x, uv0.y));
        vertices.emplace_back(sp::Vector3f( b,  a, 0.0f), sp::Vector2f(uv1.x, uv0.y));
        vertices.emplace_back(sp::Vector3f( a,  b, 0.0f), sp::Vector2f(uv0.x, uv1.y));
        vertices.emplace_back(sp::Vector3f( b,  b, 0.0f), sp::Vector2f(uv1.x, uv1.y));

        render_data.mesh = sp::MeshData::create(std::move(vertices), std::move(indices));
        render_data.shader = sp::Shader::get("internal:basic.shader");
        render_data.type = sp::RenderData::Type::Normal;
        render_data.texture = sp::texture_manager.get("tiles.png");
    }
};

static sp::Vector2d nominal_position_offset[4] = {
    {3.8,-0.75},
    {3.6, 0.75},
    {4.0,-2.25},
    {3.4, 2.25},
};

BattleScene::BattleScene()
: sp::Scene("BATTLE")
{
    auto camera = new sp::Camera(getRoot());
    camera->setOrtographic({5, 4});
    camera->setPosition({0, -1});
    setDefaultCamera(camera);

    onEnable();
}

BattleScene::~BattleScene()
{
    gui.destroy();
}

void BattleScene::onEnable()
{
    createBattleEntities(player_party, -1);
    createBattleEntities(enemy_party, 1);

    gui = sp::gui::Loader::load("gui/battle.gui", "BATTLE");

    sp::audio::Music::play("music/tough-consequence.ogg");
}

void BattleScene::onDisable()
{
    sp::audio::Music::stop();
    for(auto e : battle_entities)
        e.destroy();
    gui.destroy();
}

void BattleScene::createBattleEntities(sp::P<Party> party, double side)
{
    for(auto n=0U; n<party->members.size(); n++) {
        if (party->members[n] && party->members[n]->hp > 0) {
            auto be = new BattleEntity(getRoot(), {nominal_position_offset[n].x * side, nominal_position_offset[n].y}, party->members[n]->icon);
            be->turn_delay = sp::random(0, 1);
            be->party = party;
            be->member_index = n;
            be->character = party->members[n];
            party->members[n]->battle_entity = be;
            battle_entities.add(be);
        }
    }
}

void BattleScene::onUpdate(float delta)
{
    getCamera()->setPosition({0, -1});
    if (screen_shake > 0.0) {
        getCamera()->setPosition({sp::random(-0.1, 0.1), sp::random(-0.1, 0.1)-1});
        screen_shake = std::max(0.0, screen_shake - delta);
    }

    for(auto n=0U; n<player_party->members.size(); n++) {
        auto ui = gui->getWidgetWithID("STAT" + sp::string(n));
        auto character = player_party->members[n];
        if (!character) {
            ui->hide();
        } else {
            ui->getWidgetWithID("NAME")->setAttribute("caption", character->name);
            if (character->hp <= 0) {
                ui->getWidgetWithID("HP")->setAttribute("caption", "DEAD/" + sp::string(character->active_stats.max_hp));
            } else {
                ui->getWidgetWithID("HP")->setAttribute("caption", sp::string(character->hp) + "/" + sp::string(character->active_stats.max_hp));
            }
        }
    }
}

void BattleScene::onFixedUpdate()
{
    if (current_action) {
        auto result = current_action->resume().value();
        if (!result) {
            current_entity->character->tickBuffs();

            current_action = nullptr;
            for(auto be : battle_entities) {
                if (be->idle_state == BattleEntity::IdleState::Busy)
                    be->idle_state = BattleEntity::IdleState::Idle;
                if (be->character->hp <= 0)
                    be.destroy();
            }

            if (!player_party->alive() || !enemy_party->alive()) {
                state = State::Victory;
            }
        }
    } else {
        switch(state)
        {
        case State::Wait:
            findNextTurn();
            gui->getWidgetWithID("QUICK_TEXT")->setAttribute("caption", "");
            break;
        case State::SelectAction:{
            if (controller.right.getDown()) {
                current_entity->character->nextItem();
                buildItemList();
            }
            if (controller.left.getDown()) {
                current_entity->character->prevItem();
                buildItemList();
            }
            if (controller.primary_action.getDown()) {
                state = State::SelectTarget;

                selectTarget({0, 0}, current_entity->character->current_item->target);
                source_cursor.destroy();
                target_cursor = new BattleCursor(current_target);

                for(auto e : gui->getWidgetWithID("ITEM_LIST")->getChildren())
                    e.destroy();
            }
            auto name = current_entity->character->name;
            if (current_entity->character->current_item->type == Item::Type::Magic)
                name += " " + sp::string(current_entity->character->mp) + "/" + sp::string(current_entity->character->active_stats.max_mp) + "MP";
            gui->getWidgetWithID("QUICK_TEXT")->setAttribute("caption", name + ": " + current_entity->character->current_item->name);
            }break;
        case State::SelectTarget:
            if (controller.left.getDown()) selectTarget({-1, 0}, current_entity->character->current_item->target);
            if (controller.right.getDown()) selectTarget({1, 0}, current_entity->character->current_item->target);
            if (controller.up.getDown()) selectTarget({0, 1}, current_entity->character->current_item->target);
            if (controller.down.getDown()) selectTarget({0, -1}, current_entity->character->current_item->target);
            if (controller.secondary_action.getDown()) {
                state = State::SelectAction;
                buildItemList();
            } else if (controller.primary_action.getDown() && current_target) {
                current_action = current_entity->character->current_item->script_env.callCoroutine("use", current_entity->character, current_target->character).value();
                current_entity->idle_state = BattleEntity::IdleState::Busy;
                current_target->idle_state = BattleEntity::IdleState::Busy;
                state = State::Wait;

                for(auto e : gui->getWidgetWithID("ITEM_LIST")->getChildren())
                    e.destroy();
                target_cursor.destroy();
                gui->getWidgetWithID("QUICK_TEXT")->setAttribute("caption", "");
            } else if (current_target) {
                gui->getWidgetWithID("QUICK_TEXT")->setAttribute("caption", current_target->character->name);
            }
            break;
        case State::Victory:
            gui->getWidgetWithID("QUICK_TEXT")->setAttribute("caption", "Victory!");
            if (controller.primary_action.getDown()) {
                for(auto member : player_party->members)
                    if (member)
                        member->clearBuffs();
                for(auto member : enemy_party->members)
                    if (member)
                        member->clearBuffs();
                sp::Scene::get("MAIN")->enable();
                disable();
            }
            break;
        }
    }
}

void BattleScene::findNextTurn()
{
    float delay_min = 1.0f;
    for(auto be : battle_entities)
        delay_min = std::min(delay_min, be->turn_delay / be->character->speed());
    for(auto be : battle_entities)
        be->turn_delay -= delay_min * be->character->speed();
    current_entity = nullptr;
    for(auto be : battle_entities)
        if (be->turn_delay <= 0.0f)
            current_entity = be;
    if (!current_entity)
        return;
    current_entity->turn_delay = 1.0f;
    
    if (current_entity->party == enemy_party)
    {
        // Pick an target for the AI attack (could be done better)
        // TODO: Pick other attacks then the first.
        auto it = current_entity->character->items.begin();
        auto target_be = randomTarget(player_party);
        if ((*it)->target == Item::Target::Ally)
            target_be = randomTarget(enemy_party);
        if ((*it)->target == Item::Target::Self)
            target_be = current_entity;
        if (target_be) {
            current_action = (*it)->script_env.callCoroutine("use", current_entity->character, target_be->character).value();
            current_entity->idle_state = BattleEntity::IdleState::Busy;
            target_be->idle_state = BattleEntity::IdleState::Busy;
        }
    } else {
        state = State::SelectAction;

        if (!current_entity->character->current_item)
            current_entity->character->current_item = *current_entity->character->items.begin();

        buildItemList();

        source_cursor = new BattleCursor(current_entity);
    }
}

void BattleScene::buildItemList()
{
    for(auto e : gui->getWidgetWithID("ITEM_LIST")->getChildren())
        e.destroy();

    auto item_list = gui->getWidgetWithID("ITEM_LIST");
    for(auto item : current_entity->character->items) {
        auto img = new sp::gui::Image(item_list);
        img->setAttribute("texture", "tiles.png");
        img->layout.alignment = sp::Alignment::Bottom;
        img->setUV(tileUV(item->icon));
        if (current_entity->character->current_item == item)
            img->setSize({75, 75});
        else
            img->setSize({50, 50});
    }
}

void BattleScene::selectTarget(sp::Vector2d offset, Item::Target target_type)
{
    auto pos = current_target ? current_target->getPosition2D() + offset : offset;
    auto angle = offset.angle();
    auto best_score = 1000.0;
    auto best = current_target;
    for(auto target : battle_entities) {
        if (target == current_target) continue;
        if (target_type == Item::Target::Enemy && target->party != enemy_party) continue;
        if (target_type == Item::Target::Ally && target->party != player_party) continue;
        if (target_type == Item::Target::Self && target != current_entity) continue;
        auto dist = (pos - target->getPosition2D()).length();
        auto a = (target->getPosition2D() - pos).angle();
        auto score = dist * 15.0 + std::abs(sp::angleDifference(a, angle));
        if (score < best_score) {
            best_score = score;
            best = target;
        }
    }
    current_target = best;
    if (target_cursor && current_target)
        target_cursor->setParent(current_target);
}

sp::P<BattleEntity> BattleScene::randomTarget(sp::P<Party> party)
{
    if (!party->alive())
        return nullptr;
    while(true) {
        int idx = sp::irandom(0, party->members.size() - 1);
        if (party->members[idx] && party->members[idx]->hp > 0 && party->members[idx]->battle_entity)
            return party->members[idx]->battle_entity;
    }
    return nullptr;
}
