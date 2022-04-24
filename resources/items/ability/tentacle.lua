this.name = "Tentacle"
this.description = "Latch a tentacle to the enemy, draining life"
this.icon = 417
this.target = "enemy"
this.type = "ability"

weapon_strength = -10
weapon_hit = -10

include("items/common/melee_weapon.lua")

function onhit(source, target, dmg)
    target.iconEffect(617, 7)
    source.textRain("+" .. dmg)
    source.hp = source.hp + dmg
end
