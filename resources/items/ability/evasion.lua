this.name = "Evasion"
this.description = "Increases evasion for a number of turns"
this.icon = 717
this.target = "self"
this.type = "ability"

include("items/common/util.lua")

function use(source, target)
    doMove(source, source.getPosition(), source.getFrontPosition(), 10)
    
    if target.addBuff("EVASION", {evasion=15, threat=-30}, function() for n=0,15 do yield() end target.textRain("-EVA") end) then
        target.textRain("+EVA")
    else
        source.textRain("No effect")
    end
    for n=0, 25 do yield() end

    doMove(source, source.getFrontPosition(), source.getPosition(), 10)
end
