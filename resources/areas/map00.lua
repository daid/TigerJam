onmove("town", "town")
function town()
    loadmap("areas/town1", "entrance")
end

onmove("forest", "forest")
function forest()
    loadmap("areas/forest", "entrance")
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
