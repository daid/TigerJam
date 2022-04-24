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

if got_river_hint and not bridge_repaired then
    bridgerepair = newnpc("bridgerepair", 222)
    bridgerepair.onbump(function()
        if bridge_repaired then
            message("See you later.")
        elseif found_wedding_ring then
            message("Thanks for the ring. Grave robbing is a profitable business.")
            message("I will repaire the bridge for you.")
            bridge_repaired = true
        else
            message("You need the bridge repaired?\nI can do so for a price.")
            message("Find me the [Diamond Wedding Ring] in the catacombs.")
            message("The catacombs are located underneath the graveyard.")
        end
    end)
end
