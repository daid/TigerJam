this.name = "Potion"
this.description = "Drink it to heal 150HP"
this.icon = 669
this.target = "ally"

include("items/common/util.lua")

function use(source, target)
    doMove(source, source.getPosition(), source.getFrontPosition(), 10)
    
    local heal_amount = 150
    target.textRain("+" .. heal_amount)
    target.hp = target.hp + heal_amount
    target.iconEffect(532, 12)
    for n=0, 25 do yield() end

    doMove(source, source.getFrontPosition(), source.getPosition(), 10)
    
    this.destroy()
end
