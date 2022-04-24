this.name = "Fire (5MP)"
this.description = "Fire spell, deals damage"
this.icon = 568
this.target = "enemy"
this.type = "magic"

include("items/common/util.lua")

function use(source, target)
    doMove(source, source.getPosition(), source.getFrontPosition(), 10)
    
    if source.mp >= 5 then
        local damage_amount = source.intelligence * 5 - target.stamina
        target.textRain(damage_amount)
        target.hp = target.hp - damage_amount
        source.mp = source.mp - 5
        target.iconEffect(568, 12)
    else
        source.textRain("NO MP")
    end
    for n=0, 25 do yield() end

    doMove(source, source.getFrontPosition(), source.getPosition(), 10)
end
