
function use(source, target)
    for n=0,20 do
        local delta = target.getFrontPosition() - source.getPosition()
        source.setPosition(source.getPosition() + delta * n / 20 + Vector2(0, math.sin(n / 20 * math.pi) * 0.3))
        yield()
    end
    
    local hit_chance = 84 + weapon_hit - target.evasion
    if random(0, 100) > hit_chance then
        target.textRain("miss")
    else
        local dmg = source.strength + weapon_strength
        dmg = irandom(dmg, dmg * 2) - target.defence
        if dmg < 0 then dmg = 0 end
        
        target.textRain(dmg)
        target.hp = target.hp - dmg
        if dmg > 0 then
            shake(0.1);
        end
    end
    for n=0, 5 do yield() end

    for n=0,20 do
        local delta = source.getPosition() - target.getFrontPosition()
        source.setPosition(target.getFrontPosition() + delta * n / 20 + Vector2(0, math.sin(n / 20 * math.pi) * 0.3))
        yield()
    end
end
