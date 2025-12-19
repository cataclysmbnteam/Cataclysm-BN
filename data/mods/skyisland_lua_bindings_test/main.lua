gdebug.log_info("skyisland_lua_bindings_test: main")

local mod = game.mod_runtime[game.current_mod]

--[[
  Test: Item Storage for Cross-Map Teleportation
  Bindings: map:store_item, map:retrieve_stored_item, map:get_stored_item_count, map:clear_stored_items

  Real use case: Store items before teleporting player to new location,
  then retrieve them after the map has changed. Storage survives map changes.

  Test pattern:
  1. Store items from ground into C++ storage (simulating pre-teleport)
  2. Check storage count
  3. Retrieve to a different tile (simulating post-teleport placement)
  4. Clean up storage
]]
mod.test_item_storage = function(who, item, pos)
  local map = gapi.get_map()
  local items_at_pos = map:get_items_at(pos)

  if #items_at_pos == 0 then
    gapi.add_msg("No items at your feet. Drop some items first!")
    gapi.add_msg("Tip: Drop a container with items inside to test nested item preservation.")
    return 0
  end

  gapi.add_msg("=== Item Storage Test ===")
  gapi.add_msg("Found " .. #items_at_pos .. " item(s) at position.")

  -- Store all items (simulates grabbing items before teleport)
  local stored_indices = {}
  for _, it in ipairs(items_at_pos) do
    local idx = map:store_item(pos, it)
    if idx >= 0 then
      table.insert(stored_indices, idx)
      gapi.add_msg("Stored: index " .. idx)
    else
      gapi.add_msg("ERROR: Failed to store item!")
    end
  end

  local count = map:get_stored_item_count()
  gapi.add_msg("Storage count: " .. count)

  -- Retrieve to adjacent tile (simulates placing items after teleport)
  local target_pos = Tripoint.new(pos.x + 1, pos.y, pos.z)
  gapi.add_msg("Retrieving items one tile EAST...")

  for _, idx in ipairs(stored_indices) do
    local success = map:retrieve_stored_item(idx, target_pos)
    if success then
      gapi.add_msg("Retrieved index " .. idx .. " -> east tile")
    else
      gapi.add_msg("ERROR: Failed to retrieve index " .. idx)
    end
  end

  -- Always clean up storage
  map:clear_stored_items()
  gapi.add_msg("Storage cleared. Final count: " .. map:get_stored_item_count())
  gapi.add_msg("Check one tile EAST for your items!")

  return 1
end

--[[
  Test: Move Item Preserving State
  Binding: map:move_item_to

  Real use case: Move an item from one tile to another on the same map
  while preserving all item state including contents (nested items).

  This is simpler than store/retrieve but only works on the current map.
]]
mod.test_item_teleport = function(who, item, pos)
  local map = gapi.get_map()
  local items_at_pos = map:get_items_at(pos)

  if #items_at_pos == 0 then
    gapi.add_msg("No items at your feet. Drop some items first!")
    gapi.add_msg("Tip: Drop a backpack with items to verify contents are preserved.")
    return 0
  end

  gapi.add_msg("=== Move Item Test ===")

  -- Let player choose direction
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

  Real use case: On death/resurrection, strip all items from the character
  (inventory, worn items, wielded item) and drop them at current position.
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
  Bindings: overmapbuffer.find_all, find_closest, find_random, ter, check_ot,
            seen, set_seen, is_explored, OmtFindParams

  Real use cases:
  - Find specific terrain types (spawn points, destinations)
  - Check what terrain player is standing on
  - Reveal areas on the map (set_seen)
  - Filter searches by various criteria

  Tests multiple OtMatchType values: TYPE, PREFIX, CONTAINS
]]
mod.test_overmap_search = function(who, item, pos)
  local map = gapi.get_map()

  -- Convert to overmap coordinates
  local abs_pos = map:get_abs_ms(pos)
  local omt_pos = coords.ms_to_omt(abs_pos)

  gapi.add_msg("=== Overmap Buffer Test ===")
  gapi.add_msg("OMT Position: " .. tostring(omt_pos))

  -- Test ter() - get terrain at position
  local current_ter = overmapbuffer.ter(omt_pos)
  gapi.add_msg("Current terrain: " .. tostring(current_ter))

  -- Test seen(), is_explored()
  local is_seen = overmapbuffer.seen(omt_pos)
  local is_explored = overmapbuffer.is_explored(omt_pos)
  gapi.add_msg("Seen: " .. tostring(is_seen) .. ", Explored: " .. tostring(is_explored))

  -- Test check_ot() with different match types
  gapi.add_msg("--- check_ot() tests ---")
  local is_field = overmapbuffer.check_ot("field", OtMatchType.TYPE, omt_pos)
  local has_house = overmapbuffer.check_ot("house", OtMatchType.PREFIX, omt_pos)
  local contains_road = overmapbuffer.check_ot("road", OtMatchType.CONTAINS, omt_pos)
  gapi.add_msg("TYPE 'field': " .. tostring(is_field))
  gapi.add_msg("PREFIX 'house': " .. tostring(has_house))
  gapi.add_msg("CONTAINS 'road': " .. tostring(contains_road))

  -- Test find_closest() - common pattern for finding nearest POI
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

  -- Test find_all() with exclude_types - find buildings but not houses
  gapi.add_msg("--- find_all() with exclusions ---")
  local building_params = OmtFindParams.new()
  building_params:add_type("s_", OtMatchType.PREFIX) -- shops
  building_params:add_exclude_type("house", OtMatchType.PREFIX)
  building_params:set_search_range(0, 50)
  building_params.max_results = 5
  building_params.existing_only = true -- only search generated overmaps

  local shops = overmapbuffer.find_all(omt_pos, building_params)
  gapi.add_msg("Found " .. #shops .. " shop(s) within 50 tiles")

  -- Test find_random() - useful for random spawn locations
  gapi.add_msg("--- find_random() test ---")
  local road_params = OmtFindParams.new()
  road_params:add_type("road", OtMatchType.TYPE)
  road_params:set_search_range(0, 30)

  local random_road = overmapbuffer.find_random(omt_pos, road_params)
  if random_road then
    gapi.add_msg("Random road tile: " .. tostring(random_road))
  else
    gapi.add_msg("No roads within 30 tiles")
  end

  -- Test set_seen() - reveal a nearby tile
  gapi.add_msg("--- set_seen() test ---")
  local reveal_pos = Tripoint.new(omt_pos.x + 1, omt_pos.y, omt_pos.z)
  local was_seen = overmapbuffer.seen(reveal_pos)
  overmapbuffer.set_seen(reveal_pos, true)
  local now_seen = overmapbuffer.seen(reveal_pos)
  gapi.add_msg("Tile east: was_seen=" .. tostring(was_seen) .. " -> now_seen=" .. tostring(now_seen))

  -- Test z-level search with set_search_layers
  gapi.add_msg("--- Z-level search test ---")
  local underground_params = OmtFindParams.new()
  underground_params:add_type("subway", OtMatchType.PREFIX)
  underground_params:set_search_range(0, 50)
  underground_params:set_search_layers(-3, -1) -- underground only
  underground_params.max_results = 3

  local subways = overmapbuffer.find_all(omt_pos, underground_params)
  gapi.add_msg("Found " .. #subways .. " subway tile(s) underground")

  return 1
end
