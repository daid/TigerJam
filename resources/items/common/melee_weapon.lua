include("items/common/util.lua")

function use(source, target)
    doMove(source, source.getPosition(), target.getFrontPosition(), 20)
    
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
        onhit(source, target, dmg)
        if dmg > 0 then
            shake(0.1);
        end
    end
    for n=0, 5 do yield() end

    doMove(source, target.getFrontPosition(), source.getPosition(), 20)
end

function onhit(source, target, dmg)

end
