gdebug.log_info("Ressurection: main")

local mod = game.mod_runtime[game.current_mod]
local storage = game.mod_storage[game.current_mod]

--Item id (static)

mod.storage = storage

-- Variable ids

-- Table of anchor positions
mod.anchor_list = {}
-- saved variables... these are separate from the active variables so I didn't mess with the saved tables constantly... also less chances of saves breaking on alt-f4, not that you'd do that ;)
storage.anchor_omt = {}
-- NOTE: due to an engine bug, the saved table keys need to be strings, not ints, that's why there's a mishmash of keys (tostring and tonumber functions used)... while this will be fixed in the foreseeable future (ty Olanti), there's no need to change this in the mod - everything should still work

-- helper function for counting tables with non-integer keys, #table does not work for those
mod.count_table = function(save_table)
  local count = 0
  for _, _ in pairs(save_table) do
    count = count + 1
  end
  return count
end

-- LOADING

mod.load_saved_anchors = function()
  if mod.count_table(storage.anchor_omt) == 0 then
    print("Data not loaded - no anchors are placed.")
  else
    for i in pairs(storage.anchor_omt) do
      mod.anchor_list[tonumber(i)] = storage.anchor_omt[i]
      print("Data found! anchor = " .. tostring(mod.anchor_list[tonumber(i)]))
    end
  end
end

mod.on_game_load_hook = function()
  gdebug.log_info("Teleporters: on_load")

  mod.load_saved_anchors()
end

-- SAVING

mod.save_anchor_omt = function()
  for i in pairs(mod.anchor_list) do
    storage.anchor_omt[tostring(i)] = mod.anchor_list[i]
    print("Saved to anchor list" .. tostring(mod.anchor_list[i]))
  end
end

mod.on_game_save_hook = function()
  gdebug.log_info("Teleporters: on_save")
  mod.save_anchor_omt()
end

mod.pre_death_hook = function()
  local who = gapi.get_avatar()
  local pos = who:pos()
  if mod.pick_teleport_destination(who, pos) == 1 then who:set_all_parts_hp_cur(10) end
end
-- main function

mod.add_anchor_to_list = function(pos)
  --print("adding to list")
  --print(pos)
  table.insert(mod.anchor_list, pos)
  --print(#mod.anchor_list, pos)
  mod.save_anchor_omt()
end

mod.iuse_function_anchor = function(who, item, pos)
  local a = { "teleporter_anchor_deployed" }

  local player_abs_pos = gapi.get_map():get_abs_ms(pos)
  --print(player_abs_pos)

  local player_map_pos = gapi.get_map():get_local_ms(player_abs_pos)
  --print(player_map_pos)
  local player_omt = coords.ms_to_omt(player_abs_pos)

  local no_furn = gapi.get_map():get_furn_at(player_map_pos)
  b = tostring(no_furn)
  --print(b)
  anchor_omt = tostring(player_omt)

  if b == "FurnIntId[0][f_null]" then
    --print (FurnId.new("teleporter_anchor_deployed"):int_id())
    gapi.get_map():set_furn_at(player_map_pos, FurnId.new(a[1]):int_id())

    mod.add_anchor_to_list(player_omt)

    gapi.add_msg("The teleporter anchor was placed at ")
    gapi.add_msg(anchor_omt)
  else
    gapi.add_msg("Can only be placed on a square with no existing furniture.")
    return 0
  end
  return 1
end

mod.pick_teleport_destination = function(who)
  local pos = who:get_pos_ms()
  local abs_pos = gapi.get_map():get_abs_ms(pos)
  local abs_omt = coords.ms_to_omt(abs_pos)
  local min_dist = math.maxinteger
  local eidx = 1
  local idx = 0

  for i in pairs(mod.anchor_list) do
    local anchor = mod.anchor_list[i]
    local distance = coords.rl_dist(abs_omt, anchor)
    if distance < min_dist then
      eidx = idx
      min_dist = distance
    end
    idx = idx + 1
  end

  if idx == 0 then return 0 end
  local anchor = mod.anchor_list[eidx]
  gapi.add_msg("Respawning Player at " .. tostring(anchor))

  --need exposed debug teleport for this to function
  gapi.place_player_overmap_at(anchor)
  return 1
end

mod.remove_placed_furniture = function(pos)
  local abs_pos = gapi.get_map():get_abs_ms(pos)
  local abs_omt = coords.ms_to_omt(abs_pos)
  local ui_remove_furn = UiList.new()
  for i in pairs(mod.anchor_list) do
    if mod.anchor_list[i] == abs_omt then
      mod.anchor_list[i] = nil
      for j in pairs(mod.anchor_list) do
        if j ~= 1 then
          if mod.anchor_list[j - 1] == nil then
            mod.anchor_list[j - 1] = mod.anchor_list[j]
            mod.anchor_list[j] = nil
          end
        end
      end
      mod.scan_omt_remove_furn(abs_omt, "anchor")
      return 0
    elseif i == mod.count_table(mod.anchor_list) then
      gapi.add_msg(locale.gettext("No anchors found on this tile."))
      return 0
    end
  end
end
