this.name = "Ether"
this.description = "Drink it to heal 30MP"
this.icon = 964
this.target = "ally"

include("items/common/util.lua")

function use(source, target)
    doMove(source, source.getPosition(), source.getFrontPosition(), 10)
    
    target.textRain("+30MP")
    target.mp = target.mp + 30
    for n=0, 25 do yield() end

    doMove(source, source.getFrontPosition(), source.getPosition(), 10)

    this.destroy()
end
