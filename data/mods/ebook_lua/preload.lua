gdebug.log_info("ebook_lua: preload online.")

local mod = game.mod_runtime[game.current_mod]
local stor = game.mod_storage[game.current_mod]

mod.stor = stor

mod.cache_static = function(category, name, binding_id)
  stor[category] = stor[category] or {}
  if not stor[category][name] then
    stor[category][name] = binding_id.new(name)
  end
  return stor[category][name]
end

-- I'm too lazy to write entire of the below.
mod.from_turns = function(integer_that_you_want_to_change_to_turns)
  return TimeDuration.new().from_turns(integer_that_you_want_to_change_to_turns)
end

-- mod.cache_static( "flags", "", JsonFlagId.new())

-- function should return false after all.
mod.turntimer_hook = function(turn, func)
  gdebug.log_info(string.format("Turn %d, Timer is on for %d turn", gapi.current_turn():to_turn(), turn))
  local the_when = gapi.current_turn() - gapi.turn_zero()
  the_when = the_when + mod.from_turns(turn)
  gapi.add_on_every_x_hook(the_when, func)
  --Assurance on game closing
  --stor.hook_assuarance[the_when:to_turns()] = func
  --Oops. lua table can't store function.
  stor.hook_assuarance = stor.hook_assuarance or {}
  stor.hook_assuarance[the_when:to_turns()] = "Requiem"
end

game.hooks.on_game_load[#game.hooks.on_game_load + 1] = function(...)
  return mod.assure_timer_hook(...)
end

game.iuse_functions["LUA_EBOOK"] = function(...)
  return mod.ebook_ui(...)
end
