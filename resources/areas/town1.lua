onmove("exit", "exit")
function exit()
    if visited_king then
        loadmap("areas/map00", "town")
    else
        message("We should visit the king before leaving.")
        for n=0,3 do moveplayer(0, 1) end
        moveplayer(-1, 0)
        moveplayer(0, 1)
        moveplayer(-1, 0)
        moveplayer(-1, 0)
        moveplayer(0, 1)
    end
end

onmove("castle", "entercastle")
function entercastle()
    loadmap("areas/castle", "entrance")
end

onmove("guild", "enterguild")
function enterguild()
    loadmap("areas/guild", "entrance")
end

shop = newnpc("shop", 74)
shop.onbump(function()
    message("Come back later when I'm implemented...")
end)
