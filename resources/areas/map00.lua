onmove("town", "town")
function town()
    loadmap("areas/town1", "entrance")
end

onmove("forest", "forest")
function forest()
    loadmap("areas/forest", "entrance")
end

onmove("grave", "grave")
function grave()
    if got_river_hint then
        message("Found a secret entrance to the catacombs")
        loadmap("areas/catacombs", "entrance")
    end
end

encounters = {
    {"monster/bat", "monster/bat"},
    {"monster/bat", "monster/bat", "monster/bat"},
    {"monster/scorpion", "monster/bat", "monster/bat"},
    {"monster/scorpion", "monster/scorpion"},
}

function randomencounter()
    battle(table.unpack(encounters[irandom(1, #encounters)]))
end

if bridge_repaired then
    settile("brokenbridge", 261)
else
    onmove("brokenbridge", "brokenbridge")
    function brokenbridge()
        message("The bridge is out.")
    end
end

onmove("bridge", "bridge")
function bridge()
    message("End of the game, this as far as I got to into making content.")
    message("Hope you enjoyed it.")
end
