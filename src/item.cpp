#include "item.h"
#include "main.h"

#include <sp2/random.h>
#include <sp2/audio/sound.h>

static int luaYield(lua_State* L)
{
    return lua_yield(L, 0);
}

static int luaInclude(lua_State* L)
{
    auto name = luaL_checkstring(L, 1);
    auto stream = sp::io::ResourceProvider::get(name);
    if (!stream)
        return 0;
    if (luaL_loadstring(L, stream->readAll().c_str()))
    {
        return lua_error(L);
    }
    lua_pushvalue(L, lua_upvalueindex(1));
    //set the environment table it as 1st upvalue 
    lua_setupvalue(L, -2, 1);
    lua_call(L, 0, 0);
    return 0;
}

static void luaShake(double amount)
{
    screen_shake = amount;
}

static void luaSfx(sp::string name)
{
    sp::audio::Sound::play(name);
}

Item::Item(const sp::string& script_name)
{
    script_env.setGlobal("this", this);
    script_env.setGlobal("yield", luaYield);
    script_env.setGlobal("shake", luaShake);
    script_env.setGlobal("include", luaInclude);
    script_env.setGlobal("sfx", luaSfx);
    script_env.setGlobal("random", sp::random);
    script_env.setGlobal("irandom", sp::irandom);
    script_env.load(script_name).value();
}

void Item::onRegisterScriptBindings(sp::script::BindingClass& script_binding_class)
{
    script_binding_class.bindProperty("name", name);
    script_binding_class.bindProperty("icon", icon);
    script_binding_class.bindProperty("target", &Item::getTarget, &Item::setTarget);
}

sp::string Item::getTarget() const
{
    switch(target) {
    case Target::Any: return "any";
    case Target::Enemy: return "enemy";
    case Target::Ally: return "ally";
    case Target::Self: return "self";
    }
    return "";
}

void Item::setTarget(sp::string value)
{
    if (value == "any") target = Target::Any;
    if (value == "enemy") target = Target::Enemy;
    if (value == "ally") target = Target::Ally;
    if (value == "self") target = Target::Self;
}
