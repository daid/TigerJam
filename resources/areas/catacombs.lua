onmove("entrance", "entrance")
function entrance()
    loadmap("areas/map00", "grave")
end

onmove("switch", "switch")
function switch()
    settile("switch", 494)
    settile("door", 443)
end

onmove("door" ,"door")
function door()
    if found_wedding_ring and not killed_golem then
        message("You hear a loud rumble...")
        message("'Who too my ring!'")
        --battle("monster/golem", "monster/ghost", "monster/ghost")
        yield()
        killed_golem = true
    end
end

if found_wedding_ring then
    settile("ring", -1)
else
    onmove("ring", "ring")
    function ring()
        if not found_wedding_ring then
            message("Picked up the wedding ring.")
            found_wedding_ring = true
            settile("ring", -1)
        end
    end
end

encounters = {
    {"monster/spider", "monster/ghost"},
    {"monster/skeleton", "monster/skeleton"},
    {"monster/skeleton", "monster/ghost"},
    {"monster/ghost", "monster/skeleton", "monster/skeleton"},
}

function randomencounter()
    --battle(table.unpack(encounters[irandom(1, #encounters)]))
end