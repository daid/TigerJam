this.name = "Splash"
this.description = "Splash, but nothing happened."
this.icon = 615
this.target = "enemy"
this.type = "ability"

include("items/common/util.lua")

function use(source, target)
    doMove(source, source.getPosition(), source.getFrontPosition(), 10)
    
    for n=1, 5 do
        doMove(source, source.getFrontPosition(), source.getFrontPosition() + Vector2(0, 0.25), 10)
        doMove(source, source.getFrontPosition() + Vector2(0, 0.25), source.getFrontPosition(), 10)
        source.textRain("Splash")
        for n=0, 5 do yield() end
    end
    doMove(source, source.getFrontPosition(), source.getPosition(), 10)
end
