log_info("SLT: main")

local mod = game.mod_runtime[ game.current_mod ]
local storage = game.mod_storage[ game.current_mod ]

--[[
    If we keep all our data simple and in mod.storage,
    we won't even have to register save/load hooks,
    it'll be saved/loaded automatically.
]]--
mod.storage = storage

--[[
    If we want to build complex state out of loaded data
    we may create a hook that would read loaded data from mod_storage
    and create our complex state in the mod_runtime.
]]--
mod.on_game_load_hook = function()
    log_info("SLT: on_load")
    
    if storage.num then
        log_info( "Data found! num = ", storage.num )
    end
    if storage.tri then
        log_info( "Data found! tri = ", storage.tri )
    else
        log_warn("Save/load of userdata is not implemented!")
        if storage.tri_as_str then
            log_info("Using HACK to load tri from string = ", storage.tri_as_str)
        else
            log_info("No HACKed version found, this must be a fresh save.")
        end
    end
end

--[[
    If we have complex enough state (e.g. recursive tables, or with custom metatables)
    we may create a hook that would write a simplified version into mod_storage,
    so the hardcoded JSON serializer would be able to handle it.
]]--
mod.on_game_save_hook = function()
    log_info("SLT: on_save")

    storage.num = 12.3
    -- Uncommenting this line will cause debugmsgs on save 
    --storage.tri = Tripoint.new(3, 4, 5)

    storage.tri_as_str = tostring( Tripoint.new(3, 4, 5) )
end
