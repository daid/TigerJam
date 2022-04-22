this.name = "Guard"
this.icon = 3 * 49 + 39
this.target = "self"

function use(source, target)
    for n=0,10 do
        local delta = source.getFrontPosition() - source.getPosition()
        source.setPosition(source.getPosition() + delta * n / 20 + Vector2(0, math.sin(n / 20 * math.pi) * 0.3))
        yield()
    end
    
    target.textRain("+DEF")
    --TODO: Actual buff
    --buff("GUARD", function() for n=0,10 do yield() end)
    for n=0, 25 do yield() end

    for n=0,10 do
        local delta = source.getPosition() - source.getFrontPosition()
        source.setPosition(source.getFrontPosition() + delta * n / 20 + Vector2(0, math.sin(n / 20 * math.pi) * 0.3))
        yield()
    end
end
