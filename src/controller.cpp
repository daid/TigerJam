#include "controller.h"


Controller::Controller()
: up("UP"), down("DOWN"), left("LEFT"), right("RIGHT")
, primary_action("PRIMARY"), secondary_action("SECONDARY"), protect_action("PROTECT"), special_action("SPECIAL")
{
    up.setKeys({"up", "keypad 8", "gamecontroller:0:button:dpup", "gamecontroller:0:axis:lefty"});
    down.setKeys({"down", "keypad 2", "gamecontroller:0:button:dpdown"});
    left.setKeys({"left", "keypad 4", "gamecontroller:0:button:dpleft"});
    right.setKeys({"right", "keypad 6", "gamecontroller:0:button:dpright", "gamecontroller:0:axis:leftx"});

    primary_action.setKeys({"space", "z", "gamecontroller:0:button:a"});
    secondary_action.setKeys({"x", "gamecontroller:0:button:x"});
    protect_action.setKeys({"left shift", "gamecontroller:0:button:b"});
    special_action.setKeys({"tab", "gamecontroller:0:button:y"});

    all.add(&up);
    all.add(&down);
    all.add(&left);
    all.add(&right);

    all.add(&primary_action);
    all.add(&secondary_action);
    all.add(&protect_action);
    all.add(&special_action);
}
