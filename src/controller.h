#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <sp2/io/keybinding.h>

class Controller : sp::NonCopyable
{
public:
    Controller();

    sp::io::Keybinding up;
    sp::io::Keybinding down;
    sp::io::Keybinding left;
    sp::io::Keybinding right;

    sp::io::Keybinding primary_action;
    sp::io::Keybinding secondary_action;
    sp::io::Keybinding protect_action;
    sp::io::Keybinding special_action;

    sp::PList<sp::io::Keybinding> all;
};

#endif//CONTROLLER_H
