this.name = "Web"
this.description = "Try to entangle an enemy into a web, slowing it down"
this.icon = 576
this.target = "enemy"
this.type = "ability"

include("items/common/util.lua")

function use(source, target)
    doMove(source, source.getPosition(), target.getFrontPosition(), 20)
    
    local hit_chance = 84 - target.evasion
    if random(0, 100) > hit_chance then
        target.textRain("miss")
    else
        if target.addBuff("WEB", {agility=-15}, function() for n=0,15 do yield() end target.textRain("+AGI") end) then
            target.iconEffect(576, 12)
            target.textRain("-AGI")
        else
            source.textRain("No effect")
        end
        
        shake(0.1);
    end
    for n=0, 5 do yield() end

    doMove(source, target.getFrontPosition(), source.getPosition(), 20)
end
