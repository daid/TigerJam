#include "party.h"


sp::P<Party> player_party;
sp::P<Party> enemy_party;


Party::Party()
{
}

Party::~Party()
{
    for(auto member : members)
        member.destroy();
}

bool Party::alive()
{
    for(auto member : members)
        if (member && member->hp > 0)
            return true;
    return false;
}