gdebug.log_info("skyisland_lua_bindings_test: main")

local mod = game.mod_runtime[game.current_mod]

--[[
  Native Lua Item Storage Demo

  This demonstrates storing items in Lua tables using detached_ptr.
  When you call map:detach_item_at(), the item is removed from the map
  and returned as a detached_ptr owned by Lua. Store it in a table to
  keep it alive - Lua's GC will properly destroy it when removed.

  Bindings used:
  - map:detach_item_at(pos, item) -> detached_ptr<item> (ownership to Lua)
  - map:add_item(pos, detached_item) -> item* (ownership to map)

  IMPORTANT: Prefix your storage table names with your mod name!
]]

-- Initialize mod storage tables (these persist in mod.storage)
mod.storage = mod.storage or {}
mod.storage.bank = mod.storage.bank or {}
mod.storage.bag_of_holding = mod.storage.bag_of_holding or {}
mod.storage.next_key = mod.storage.next_key or 1

-- Helper to generate unique storage keys
local function next_storage_key()
  local key = mod.storage.next_key
  mod.storage.next_key = key + 1
  return key
end

-- Helper to count items in a storage table
local function count_items(storage)
  local count = 0
  for _ in pairs(storage) do
    count = count + 1
  end
  return count
end

-- Helper to get first item from storage
local function first_item(storage)
  for key, item in pairs(storage) do
    return key, item
  end
  return nil, nil
end

--[[
  Test: Native Lua Item Storage

  Uses Lua tables + detached_ptr for item storage.
  Items are owned by Lua and properly GC'd when removed from tables.
]]
mod.test_namespaced_storage = function(who, item, pos)
  local map = gapi.get_map()
  local items_at_pos = map:get_items_at(pos)

  gapi.add_msg("=== Native Lua Storage Demo ===")

  -- Menu to choose action
  local ui = UiList.new()
  ui:title("Lua Item Storage")
  ui:add(1, "Deposit to Bank")
  ui:add(2, "Deposit to Bag of Holding")
  ui:add(3, "View Bank Contents")
  ui:add(4, "View Bag of Holding Contents")
  ui:add(5, "Withdraw from Bank")
  ui:add(6, "Withdraw from Bag of Holding")
  ui:add(7, "Show Storage Stats")
  ui:add(8, "Clear ALL Storage")

  local choice = ui:query()
  if not choice or choice < 1 then
    gapi.add_msg("Cancelled.")
    return 0
  end

  if choice == 1 then
    -- Deposit to bank
    if #items_at_pos == 0 then
      gapi.add_msg("No items at your feet to deposit!")
      return 0
    end
    local first_item = items_at_pos[1]
    -- Detach from map - now owned by Lua
    local detached = map:detach_item_at(pos, first_item)
    if detached then
      local key = next_storage_key()
      mod.storage.bank[key] = detached
      gapi.add_msg("Deposited to bank (key " .. key .. ")")
      gapi.add_msg("Bank now has " .. count_items(mod.storage.bank) .. " item(s)")
    else
      gapi.add_msg("ERROR: Failed to detach item!")
    end
  elseif choice == 2 then
    -- Deposit to bag of holding
    if #items_at_pos == 0 then
      gapi.add_msg("No items at your feet to store!")
      return 0
    end
    local first_item = items_at_pos[1]
    local detached = map:detach_item_at(pos, first_item)
    if detached then
      local key = next_storage_key()
      mod.storage.bag_of_holding[key] = detached
      gapi.add_msg("Stored in bag of holding (key " .. key .. ")")
      gapi.add_msg("Bag now has " .. count_items(mod.storage.bag_of_holding) .. " item(s)")
    else
      gapi.add_msg("ERROR: Failed to detach item!")
    end
  elseif choice == 3 then
    -- View bank contents
    local bank = mod.storage.bank
    if count_items(bank) == 0 then
      gapi.add_msg("Bank is empty.")
    else
      gapi.add_msg("=== Bank Contents ===")
      for key, it in pairs(bank) do
        -- Access item properties directly - it's a real item reference
        local count_str = it.charges > 0 and (" x" .. it.charges) or ""
        gapi.add_msg(string.format("[%d] %s%s", key, it:tname(1, true, 0), count_str))
      end
    end
  elseif choice == 4 then
    -- View bag of holding contents
    local bag = mod.storage.bag_of_holding
    if count_items(bag) == 0 then
      gapi.add_msg("Bag of holding is empty.")
    else
      gapi.add_msg("=== Bag of Holding Contents ===")
      for key, it in pairs(bag) do
        local type_id = tostring(it:get_type())
        gapi.add_msg(string.format("[%d] %s (%s)", key, it:tname(1, true, 0), type_id))
      end
      gapi.add_msg(string.format("Total items: %d", count_items(bag)))
    end
  elseif choice == 5 then
    -- Withdraw from bank
    local key, detached = first_item(mod.storage.bank)
    if not key then
      gapi.add_msg("Bank is empty, nothing to withdraw.")
      return 0
    end
    -- Get name before placing (item stays valid until add_item moves it)
    local item_name = detached:tname(1, true, 0)
    -- Remove from our storage
    mod.storage.bank[key] = nil
    -- Place on map - ownership transfers to map
    local target_pos = Tripoint.new(pos.x, pos.y, pos.z)
    map:add_item(target_pos, detached)
    gapi.add_msg("Withdrew: " .. item_name)
    gapi.add_msg("Bank now has " .. count_items(mod.storage.bank) .. " item(s)")
  elseif choice == 6 then
    -- Withdraw from bag of holding
    local key, detached = first_item(mod.storage.bag_of_holding)
    if not key then
      gapi.add_msg("Bag of holding is empty.")
      return 0
    end
    local item_name = detached:tname(1, true, 0)
    mod.storage.bag_of_holding[key] = nil
    local target_pos = Tripoint.new(pos.x, pos.y, pos.z)
    map:add_item(target_pos, detached)
    gapi.add_msg("Retrieved: " .. item_name)
    gapi.add_msg("Bag now has " .. count_items(mod.storage.bag_of_holding) .. " item(s)")
  elseif choice == 7 then
    -- Show storage stats
    gapi.add_msg("=== Storage Stats ===")
    gapi.add_msg("Bank: " .. count_items(mod.storage.bank) .. " item(s)")
    gapi.add_msg("Bag of Holding: " .. count_items(mod.storage.bag_of_holding) .. " item(s)")
    gapi.add_msg("Next key: " .. mod.storage.next_key)
  elseif choice == 8 then
    -- Clear all storage (items will be GC'd and properly destroyed)
    local confirm_ui = UiList.new()
    confirm_ui:title("Clear ALL stored items? This cannot be undone!")
    confirm_ui:add(1, "Yes, clear everything")
    confirm_ui:add(2, "No, cancel")
    local confirm = confirm_ui:query()
    if confirm == 1 then
      mod.storage.bank = {}
      mod.storage.bag_of_holding = {}
      gapi.add_msg("All storage cleared! (Items will be GC'd)")
    else
      gapi.add_msg("Cancelled.")
    end
  end

  return 1
end

--[[
  Test: Detach/Attach with Teleport Testing

  Allows detaching items, teleporting, then reattaching.
  Items are held in mod.storage.held_items between operations.
]]

-- Storage for held items during teleport testing
mod.storage.held_items = mod.storage.held_items or {}

mod.test_item_storage = function(who, item, pos)
  local map = gapi.get_map()
  local items_at_pos = map:get_items_at(pos)

  local held_count = count_items(mod.storage.held_items)

  local ui = UiList.new()
  ui:title("Detach/Attach Test (held: " .. held_count .. ")")
  ui:add(1, "Detach items at feet")
  ui:add(2, "Reattach held items here")
  ui:add(3, "View held items")
  ui:add(4, "Drop held items (destroy)")

  local choice = ui:query()
  if not choice or choice < 1 then
    gapi.add_msg("Cancelled.")
    return 0
  end

  if choice == 1 then
    -- Detach items
    if #items_at_pos == 0 then
      gapi.add_msg("No items at your feet to detach!")
      return 0
    end

    -- Get frozen copy before modifying
    local items_copy = items_at_pos:items()

    for _, it in ipairs(items_copy) do
      local detached = map:detach_item_at(pos, it)
      if detached then
        local key = next_storage_key()
        mod.storage.held_items[key] = detached
        gapi.add_msg("Detached: " .. detached:tname(1, true, 0) .. " (key " .. key .. ")")
      end
    end
    gapi.add_msg("Now holding " .. count_items(mod.storage.held_items) .. " item(s). Teleport and reattach!")
  elseif choice == 2 then
    -- Reattach held items
    if held_count == 0 then
      gapi.add_msg("No held items to reattach!")
      return 0
    end

    local target_pos = Tripoint.new(pos.x, pos.y, pos.z)
    for key, detached in pairs(mod.storage.held_items) do
      local name = detached:tname(1, true, 0)
      map:add_item(target_pos, detached)
      mod.storage.held_items[key] = nil
      gapi.add_msg("Placed: " .. name)
    end
    gapi.add_msg("All items placed at your feet!")
  elseif choice == 3 then
    -- View held items
    if held_count == 0 then
      gapi.add_msg("No held items.")
    else
      gapi.add_msg("=== Held Items ===")
      for key, it in pairs(mod.storage.held_items) do
        gapi.add_msg(string.format("[%d] %s", key, it:tname(1, true, 0)))
      end
    end
  elseif choice == 4 then
    -- Drop (destroy) held items
    mod.storage.held_items = {}
    gapi.add_msg("All held items dropped (will be GC'd).")
  end

  return 1
end

--[[
  Test: Move Item Preserving State
  Binding: map:move_item_to

  Real use case: Move an item from one tile to another on the same map
  while preserving all item state including contents (nested items).
]]
mod.test_item_teleport = function(who, item, pos)
  local map = gapi.get_map()
  local items_at_pos = map:get_items_at(pos)

  if #items_at_pos == 0 then
    gapi.add_msg("No items at your feet. Drop some items first!")
    return 0
  end

  gapi.add_msg("=== Move Item Test ===")

  local ui = UiList.new()
  ui:title("Move first item which direction?")
  ui:add(1, "North")
  ui:add(2, "East")
  ui:add(3, "South")
  ui:add(4, "West")

  local choice = ui:query()
  if choice < 1 then
    gapi.add_msg("Cancelled.")
    return 0
  end

  local dx, dy = 0, 0
  local dir_name = ""
  if choice == 1 then
    dy = -1
    dir_name = "NORTH"
  elseif choice == 2 then
    dx = 1
    dir_name = "EAST"
  elseif choice == 3 then
    dy = 1
    dir_name = "SOUTH"
  elseif choice == 4 then
    dx = -1
    dir_name = "WEST"
  end

  local target_pos = Tripoint.new(pos.x + dx, pos.y + dy, pos.z)
  local first_item = items_at_pos[1]

  gapi.add_msg("Moving item " .. dir_name .. "...")
  map:move_item_to(pos, first_item, target_pos)
  gapi.add_msg("Done! Item moved one tile " .. dir_name .. ".")

  return 1
end

--[[
  Test: Drop All Character Items
  Binding: Character:drop_all_items
]]
mod.test_drop_all_items = function(who, item, pos)
  local player = gapi.get_avatar()

  gapi.add_msg("=== Drop All Items Test ===")
  gapi.add_msg("This will drop EVERYTHING: inventory, worn, wielded!")

  local ui = UiList.new()
  ui:title("Drop ALL items? (Cannot be undone!)")
  ui:add(1, "Yes, drop everything")
  ui:add(2, "No, cancel")

  local choice = ui:query()
  if choice ~= 1 then
    gapi.add_msg("Cancelled.")
    return 0
  end

  gapi.add_msg("Dropping all items...")
  player:drop_all_items()
  gapi.add_msg("All items dropped at your feet!")

  return 1
end

--[[
  Test: Overmap Buffer - Terrain Search & Inspection
]]
mod.test_overmap_search = function(who, item, pos)
  local map = gapi.get_map()

  local abs_pos = map:get_abs_ms(pos)
  local omt_pos = coords.ms_to_omt(abs_pos)

  gapi.add_msg("=== Overmap Buffer Test ===")
  gapi.add_msg("OMT Position: " .. tostring(omt_pos))

  local current_ter = overmapbuffer.ter(omt_pos)
  gapi.add_msg("Current terrain: " .. tostring(current_ter))

  local is_seen = overmapbuffer.seen(omt_pos)
  local is_explored = overmapbuffer.is_explored(omt_pos)
  gapi.add_msg("Seen: " .. tostring(is_seen) .. ", Explored: " .. tostring(is_explored))

  gapi.add_msg("--- find_closest() test ---")
  local town_params = OmtFindParams.new()
  town_params:add_type("house", OtMatchType.PREFIX)
  town_params:set_search_range(0, 100)

  local nearest_house = overmapbuffer.find_closest(omt_pos, town_params)
  if nearest_house then
    local dist = coords.rl_dist(omt_pos, nearest_house)
    gapi.add_msg("Nearest house: " .. tostring(nearest_house) .. " (dist: " .. dist .. ")")
  else
    gapi.add_msg("No houses within 100 tiles")
  end

  return 1
end
