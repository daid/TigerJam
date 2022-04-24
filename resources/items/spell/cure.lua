this.name = "Cure (3MP)"
this.description = "Cure spell, heals HP"
this.icon = 532
this.target = "ally"
this.type = "magic"

include("items/common/util.lua")

function use(source, target)
    doMove(source, source.getPosition(), source.getFrontPosition(), 10)
    
    if source.mp >= 3 then
        local heal_amount = source.intelligence * 12
        target.textRain("+" .. heal_amount)
        target.hp = target.hp + heal_amount
        source.mp = source.mp - 3
        target.iconEffect(532, 12)
    else
        source.textRain("NO MP")
    end
    for n=0, 25 do yield() end

    doMove(source, source.getFrontPosition(), source.getPosition(), 10)
end
