this.name = "Cure (3MP)"
this.icon = 532
this.target = "ally"
this.type = "magic"

function use(source, target)
    for n=0,10 do
        local delta = source.getFrontPosition() - source.getPosition()
        source.setPosition(source.getPosition() + delta * n / 10 + Vector2(0, math.sin(n / 10 * math.pi) * 0.3))
        yield()
    end
    
    if source.mp >= 3 then
        local heal_amount = 100
        target.textRain("+" .. heal_amount)
        target.hp = target.hp + heal_amount
        source.mp = source.mp - 3
    else
        source.textRain("NO MP")
    end
    for n=0, 25 do yield() end

    for n=0,10 do
        local delta = source.getPosition() - source.getFrontPosition()
        source.setPosition(source.getFrontPosition() + delta * n / 10 + Vector2(0, math.sin(n / 10 * math.pi) * 0.3))
        yield()
    end
end
