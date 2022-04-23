onmove("entrance", "entrance")
function entrance()
    loadmap("areas/map00", "forest")
end
onmove("chest", "chest")
function chest()
    message("Sorry, chests not implemented...")
end

encounters = {
    {"monster/spider", "monster/bat"},
    {"monster/spider", "monster/spider"},
    {"monster/spider", "monster/spider"},
    {"monster/baby_tiger"},
    {"monster/baby_tiger", "monster/spider"},
}

function randomencounter()
    battle(table.unpack(encounters[irandom(1, #encounters)]))
end

if not killed_tiger then
    tiger = newnpc("tiger", 868)
    tiger.onbump(function()
        message("There is the tiger!")
        battle("monster/tiger", "monster/baby_tiger", "monster/baby_tiger")
        yield()
        tiger.destroy()
        message("You killed the tiger\n\nAnd it's babies.")
        message("You cut out the eye of the tiger.")
        message("Got [Eye of the Tiger]")
        message("Let us return to the king.")
        killed_tiger = true
    end)
end