--Fist weapon, used by monsters as base attack
this.name = "Drop Kick"
this.icon = 89
this.target = "enemy"
this.type = "ability"

weapon_strength = 35
weapon_hit = 0

include("items/common/util.lua")

function use(source, target)
    local up_offset = Vector2(0, 1.5)
    doMove(source, source.getPosition(), target.getPosition() + up_offset, 15)
    for n=0, 15 do yield() end
    doMove(source, target.getPosition() + up_offset, target.getPosition(), 10)
    
    local hit_chance = 84 + weapon_hit - target.evasion
    if random(0, 100) > hit_chance then
        target.textRain("miss")
    else
        local dmg = source.strength + weapon_strength
        dmg = irandom(dmg, dmg * 2) - target.defence
        if dmg < 0 then dmg = 0 end
        
        target.textRain(dmg)
        sfx("basic_hit.wav")
        target.hp = target.hp - dmg
        if dmg > 0 then
            shake(0.1);
        end
    end
    for n=0, 5 do yield() end

    doMove(source, target.getPosition(), source.getPosition(), 20)
end
