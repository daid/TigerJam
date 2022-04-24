onmove("entrance", "entrance")
function entrance()
    loadmap("areas/town1", "castle")
end

onmove("jail", "enterjail")
function enterjail()
    loadmap("areas/jail", "entrance")
end

if not jail_wall_destroyed then
    king = newnpc("king", 175)
    king.onbump(function()
        visited_king = true
        message("Welcome heroes.")
        if not killed_tiger then
            message("We meet in dire times. As you can see, all colors of the world have been corrupted")
            message("We need you to retrieve the\n[Eye of the Tiger]")
            message("Which is said to have the power to remove this corruption from the world")
            message("Good luck on your quest")
        else
            message("What the HELL?")
            message("You guys killed the tiger? What for?")
            message("And what is with the bloody eye? How disgusting.")
            message("The [Eye of the Tiger] is a gemstone hidden in the ruins of Eramthgin.")
            message("Guards!\nArrest these people for animal cruelty!")
            
            guard1.move(0, -1)
            guard2.move(0, -1)
            for n=0,10 do yield() end
            guard1.move(1, 0)
            guard2.move(-1, 0)
            for n=0,10 do yield() end
            guard1.move(0, -1)
            guard2.move(0, -1)
            for n=0,10 do yield() end
            guard1.move(1, 0)
            guard2.move(-1, 0)
            for n=0,10 do yield() end
            message("Off to jail with you!")
            loadmap("areas/jail", "cell")
            message("Until, never.\nAnd don't you try to escape!")
        end
    end)

    queen = newnpc("queen", 176)

    guard1 = newnpc("guard1", 29)
    guard2 = newnpc("guard2", 29)
else
    king = newnpc("king2", 175)
    queen = newnpc("queen2", 176)
    guard1 = newnpc("guard3", 29)
    settile("jaildoor", -1)
end

onmove("books1", "books")
onmove("books2", "books")
onmove("books3", "books")
function books()
    message("The ruins of Eramthgin are located beyond the river Stinks")
    message("These ruins are said to be the resting ground of the\n[Eye of the Tiger]")
    got_river_hint = true
end
