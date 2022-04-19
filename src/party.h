#pragma once

#include <array>
#include "character.h"


class Party : public sp::AutoPointerObject
{
public:
    Party();
    ~Party();

    bool alive();

    std::array<sp::P<Character>, 4> members;
};

extern sp::P<Party> player_party;
extern sp::P<Party> enemy_party;
