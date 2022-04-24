onmove("entrance", "entrance")
function entrance()
    loadmap("areas/castle", "jail")
end

if jail_wall_destroyed then
    settile("weak_wall", 846)
else
    onmove("weak_wall", "weak_wall")
    function weak_wall()
        if jail_wall_destroyed then return end
        message("This wall looks weak, maybe we can break it.")
        battle("monster/wall", "monster/rat", "monster/rat")
        yield()
        settile("weak_wall", 846)
        jail_wall_destroyed = true
    end
end

if not jailer_killed then
    jailer = newnpc("guard", 31)
    jailer.onbump(function()
        message("You escaped?!")
        battle("monster/jailer")
        yield()
        jailer.destroy()
        jailer_killed = true
        
        message("This is our chance to escape!")
        message("We should try to find that\n[Eye of the Tiger]")
    end)
end

encounters = {
    {"monster/bat", "monster/rat"},
    {"monster/rat", "monster/rat"},
    {"monster/rat", "monster/bat", "monster/rat"},
}
function randomencounter()
    battle(table.unpack(encounters[irandom(1, #encounters)]))
end

onmove("chest", "chest")
function chest()
    if not jail_chest_opened then
        message("Found: 2x [Potion]\nFound: 2x [Ether]")
        additem("items/potion.lua")
        additem("items/potion.lua")
        additem("items/ether.lua")
        additem("items/ether.lua")
        settile("chest", 303)
        jail_chest_opened = true
    end
end
if jail_chest_opened then
    settile("chest", 303)
end
