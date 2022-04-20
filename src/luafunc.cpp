#include "luafunc.h"
#include "main.h"
#include <sp2/random.h>
#include <sp2/audio/sound.h>


static int luaYield(lua_State* L)
{
    return lua_yield(L, 0);
}

static int luaInclude(lua_State* L)
{
    sp::string name = luaL_checkstring(L, 1);
    auto stream = sp::io::ResourceProvider::get(name);
    if (!stream)
        return 0;
    auto code = stream->readAll();
    if (luaL_loadbufferx(L, code.c_str(), code.length(), ("@" + name).c_str(), "t"))
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


void registerLuaFuncs(sp::script::Environment& env)
{
    env.setGlobal("yield", luaYield);
    env.setGlobal("shake", luaShake);
    env.setGlobal("include", luaInclude);
    env.setGlobal("sfx", luaSfx);
    env.setGlobal("random", sp::random);
    env.setGlobal("irandom", sp::irandom);
}