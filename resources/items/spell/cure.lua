this.name = "Cure"
this.icon = 532
this.target = "ally"

function use(source, target)
    for n=0,20 do
        local delta = source.getFrontPosition() - source.getPosition()
        source.setPosition(source.getPosition() + delta * n / 20 + Vector2(0, math.sin(n / 20 * math.pi) * 0.3))
        yield()
    end
    
    local heal_amount = 100
    target.textRain("+" .. heal_amount)
    target.hp = target.hp + heal_amount
    for n=0, 5 do yield() end

    for n=0,20 do
        local delta = source.getPosition() - source.getFrontPosition()
        source.setPosition(source.getFrontPosition() + delta * n / 20 + Vector2(0, math.sin(n / 20 * math.pi) * 0.3))
        yield()
    end
end