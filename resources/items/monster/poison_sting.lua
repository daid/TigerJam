this.name = "Poison Sting"
this.icon = 566
this.target = "enemy"
this.type = "ability"

weapon_strength = -4
weapon_hit = 0

include("items/common/melee_weapon.lua")

function onhit(target)
    if target.addBuff("POISON", {}, function()
        for n=0,5 do
            yield()

            local dmg = 15
            dmg = irandom(dmg, dmg * 2) - target.stamina
            if dmg < 0 then dmg = 0 end
            
            if dmg > 0 then
                target.textRain(dmg)
                target.hp = target.hp - dmg
                shake(0.1);
            end
        end
    end) then
        for n=0, 5 do yield() end
        target.textRain("POISON")
    end
end
