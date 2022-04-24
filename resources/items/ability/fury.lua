this.name = "Fury of blows"
this.description = "Attack up to 3 times with your fists"
this.icon = 575
this.target = "enemy"
this.type = "ability"

weapon_strength = -2
weapon_hit = -5

include("items/common/util.lua")

function use(source, target)
    doMove(source, source.getPosition(), target.getFrontPosition(), 20)
    
    for n=1, 3 do
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
    end

    doMove(source, target.getFrontPosition(), source.getPosition(), 20)
end
