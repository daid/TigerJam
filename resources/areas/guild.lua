for n=1,6 do onmove("bed" .. n, "bed") end
function bed()
    message("Took a rest\nHP/MP recovered")
    recoverHPMP();
end

onmove("entrance", "entrance")
function entrance()
    if partyMemberCount() < 1 then
        message("You should recruit some party members before leaving")
    else
        loadmap("areas/town1", "guild")
    end
end

function selectMember(msg)
    local options = {}
    for n=0,3 do
        local name = partyMemberName(n)
        if name ~= "" then
            table.insert(options, name)
        end
    end
    table.insert(options, "Cancel")
    local choice = menu(msg, table.unpack(options))
    if choice == "Cancel" then return nil end
    return choice
end

if not recruit_options then
    recruit_options = {
        {"FIGHT", "character/player/fighter.lua"},
        {"ROGUE", "character/player/rogue.lua"},
        {"PRIEST", "character/player/priest.lua"},
        {"MONK", "character/player/monk.lua"},
        {"WIZARD", "character/player/wizard.lua"},
    }
    recruit_options2 = {
        {"SPIDER", "character/player/spider.lua"},
        {"CHICKEN", "character/player/chicken.lua"},
        {"SQUID", "character/player/octopus.lua"},
        {"MERMAID", "character/player/mermaid.lua"},
    }
    
    -- Shuffle the options
    for i = #recruit_options, 2, -1 do
        local j = irandom(1, i)
        recruit_options[i], recruit_options[j] = recruit_options[j], recruit_options[i]
    end
    for i = #recruit_options2, 2, -1 do
        local j = irandom(1, i)
        recruit_options2[i], recruit_options2[j] = recruit_options2[j], recruit_options2[i]
    end
    for _,v in ipairs(recruit_options2) do 
        table.insert(recruit_options, v)
    end
    recruit_options2 = nil
end

guildmaster = newnpc("guildmaster", 224)
onmove("guildmastertable", "guildmastertable")
function guildmastertable()
    local choice = menu("Welcome", "Recruit", "Dismiss", "Leave")
    if choice == "Recruit" then
        if partyMemberCount() > 3 then
            message("Sorry, your party is already full")
        else
            local options = {}
            for idx, data in ipairs(recruit_options) do
                if #options < 4 then
                    table.insert(options, data[1])
                end
            end
            table.insert(options, "Cancel")
            choice = menu("Which member would you want?", table.unpack(options))
            for idx, data in ipairs(recruit_options) do
                if data[1] == choice then
                    partyMemberAdd(data[2])
                    table.remove(recruit_options, idx)
                    table.insert(recruit_options, data)
                    return
                end
            end
        end
    elseif choice == "Dismiss" then
        if partyMemberCount() < 1 then
            message("You do not have any members")
        else
            choice = selectMember("Select a member to dismiss\nNote that all items will be lost.")
            for n=0,3 do
                local name = partyMemberName(n)
                if name ~= "" and name == choice then
                    partyMemberDestroy(n)
                    return
                end
            end
        end
    end
end
