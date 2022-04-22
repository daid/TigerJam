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

void Character::recalculate()
{
    active_stats = base_stats;
    active_stats.evasion += active_stats.agility / 2;
    hp = std::clamp(hp, 0, active_stats.max_hp);
    mp = std::clamp(mp, 0, active_stats.max_mp);
}
