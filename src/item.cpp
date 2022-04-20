#include "item.h"
#include "main.h"
#include "luafunc.h"

#include <sp2/random.h>
#include <sp2/audio/sound.h>


Item::Item(const sp::string& script_name)
{
    script_env.setGlobal("this", this);
    registerLuaFuncs(script_env);
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
