onmove("entrance", "entrance")
function entrance()
    loadmap("areas/castle", "jail")
end

if jail_wall_destroyed then
    setTile("weak_wall", 846)
else
    onmove("weak_wall", "weak_wall")
    function weak_wall()
        if jail_wall_destroyed then return end
        battle("monster/wall", "monster/rat", "monster/rat")
        yield()
        settile("weak_wall", 846)
        jail_wall_destroyed = true
    end
end

encounters = {
    {"monster/bat", "monster/rat"},
    {"monster/rat", "monster/rat"},
    {"monster/rat", "monster/bat", "monster/rat"},
}
function randomencounter()
    battle(table.unpack(encounters[irandom(1, #encounters)]))
end
