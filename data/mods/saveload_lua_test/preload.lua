log_info("SLT: preload")

local mod = game.mod_runtime[ game.current_mod ]

game.on_game_load_hooks[ #game.on_game_load_hooks + 1 ] = function( ... )
    return mod.on_game_load_hook( ... )
end

game.on_game_save_hooks[ #game.on_game_save_hooks + 1 ] = function( ... )
    return mod.on_game_save_hook( ... )
end
