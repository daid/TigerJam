#ifndef MAIN_SCENE_H
#define MAIN_SCENE_H

#include <sp2/scene/scene.h>
#include <sp2/script/environment.h>


class Scene : public sp::Scene
{
public:
    Scene();
    ~Scene();

    virtual void onUpdate(float delta) override;

    enum class State {
        Normal,
        BattleStart,
        BattleEnd,
    } state = State::Normal;
    float state_timer = 0.0f;
    int steps_till_battle = 5;

    sp::script::Environment script_env;
    sp::script::CoroutinePtr active_sequence;
};

#endif//MAIN_SCENE_H
