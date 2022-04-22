
function doMove(source, start, target, steps)
    for n=0,steps do
        local delta = target - start
        source.setPosition(start + delta * n / steps + Vector2(0, math.sin(n / steps * math.pi) * 0.3))
        yield()
    end
    source.setPosition(target)
end
