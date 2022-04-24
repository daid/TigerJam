this.name = "Summon Rat"
this.description = ""
this.icon = 423
this.target = "self"
this.type = "ability"

skip = true

function use(source, target)
    if skip then skip = false return end -- skip the first turn
    if source.newMember("character/monster/rat.lua") then
        source.textRain("Rat!")
        for n=0,40 do yield() end
    end
end