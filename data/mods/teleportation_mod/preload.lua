gdebug.log_info("Tele: preload.")

-- Have to register iuse before data loading.
-- Actual implementation (function mod.iuse_function) will be defined later.

local mod = game.mod_runtime[game.current_mod]

-- Register our item use function
game.iuse_functions["teleporter_controller_use"] = function(...)
  return mod.iuse_function_controller(...)
end

game.iuse_functions["teleporter_anchor_use"] = function(...)
  return mod.iuse_function_anchor(...)
end

game.iuse_functions["teleporter_station_use"] = function(...)
  return mod.iuse_function_station(...)
end

game.hooks.on_game_load[#game.hooks.on_game_load + 1] = function(...)
  return mod.on_game_load_hook(...)
end

game.hooks.on_game_save[#game.hooks.on_game_save + 1] = function(...)
  return mod.on_game_save_hook(...)
end
