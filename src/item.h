#pragma once

#include <sp2/script/bindingObject.h>
#include <sp2/script/environment.h>


class Item : public sp::script::BindingObject
{
public:
    Item(const sp::string& script_name);

    virtual void onRegisterScriptBindings(sp::script::BindingClass& script_binding_class) override;

    sp::string getTarget() const;
    void setTarget(sp::string);

    sp::string name;
    int icon;
    sp::script::Environment script_env;
    enum class Target {
        Any,
        Enemy,
        Ally,
        Self,
    } target = Target::Any;
};