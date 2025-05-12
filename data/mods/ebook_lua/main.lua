gdebug.log_info("ebook_lua: main online.")

local mod = game.mod_runtime[game.current_mod]
---@class storage
---@field hook_assuarance table<integer, string>
local storage = game.mod_storage[game.current_mod]

mod.storage = storage

-- activities
local ACT_READ = mod.cache_static("activities", "ACT_READ", ActivityTypeId)
local ACT_CRAFT = mod.cache_static("activities", "ACT_CRAFT", ActivityTypeId)

-- flags
local flag_ETHEREAL_ITEM = mod.cache_static("flags", "ETHEREAL_ITEM", JsonFlagId)
local flag_TRADER_AVOID = mod.cache_static("flags", "TRADER_AVOID", JsonFlagId)
local flag_MC_USED = mod.cache_static("flags", "MC_USED", JsonFlagId)

--NEVER use <item *> as UID. Just make UID like below.
mod.item_uid = function(item)
  if item:get_var_str("tablet_uid", "nope") == "nope" then
    local uid_str = tostring(gapi.current_turn():to_turn()) .. string.format("%03d", gapi.rng(0, 999))
    item:set_var_str("tablet_uid", uid_str)
  end
  return item:get_var_str("tablet_uid", "nope")
  -- local uid_str = item:get_var_str( "tablet_uid", string.format("%d%03d", gapi.current_turn():to_turn(), gapi.rng(0,999) ) )
end

mod.unzip_var_lib2 = function(storage)
  local book_data = storage:get_var_str("book_data", "")
  local data_list = {}
  if book_data ~= "" then
    for token in string.gmatch(book_data, "([^,]+)") do
      data_list[token] = true
    end
  end
  return data_list
end

---@type fun(storage: Item, str_to_add: string): void
mod.insert_lib2 = function(storage, str_to_add)
  local inserting_book_data = storage:get_var_str("book_data", "")
  if inserting_book_data ~= "" then
    inserting_book_data = inserting_book_data .. "," .. str_to_add
  else
    inserting_book_data = str_to_add
  end
  storage:set_var_str("book_data", inserting_book_data)
end

-- #lib doesn't work! :(
---@type fun(arr: table): integer
mod.arr_num = function(arr)
  local count = 0
  for _, _ in pairs(arr) do
    count = count + 1
  end
  return count
end

---@type fun(str: string): void
mod.poppin = function(str)
  local popup = QueryPopup.new()
  popup:message(str)
  popup:allow_any_key(true)
  popup:query()
end

---@type fun(str: string): string
mod.poppyn = function(str)
  local popup = QueryPopup.new()
  popup:message(str)
  return popup:query_yn()
end

--For colorful UI!
---@type fun(item: Item, ui: UiList): void
mod.ui_coloring = function(item, ui)
  --This is percentage of charges! Lua has no math.round, so I used math.floor with a trick.
  local charge_p = math.floor(item.charges / item:ammo_capacity(false) * 100 + 0.5)
  local color = 0
  ui:title(string.format("%s( %d%%): E-Book mod", item:tname(1, false, 0), charge_p))
  if charge_p == 100 then
    color = Color.c_green
  elseif charge_p >= 75 then
    color = Color.c_light_green
  elseif charge_p >= 50 then
    color = Color.c_yellow
  elseif charge_p >= 25 then
    color = Color.c_light_red
  elseif charge_p <= 0 then
    color = Color.c_red
  end
  ui:title_color(color)
  ui:border_color(color)
end

-- STOP BOTHER ME SAFE_REFERENCE ERROR!!!
mod.item_funeral = function()
  local YOU = gapi.get_avatar()
  local NOW = gapi.current_turn() - gapi.turn_zero()
  gdebug.log_info(string.format("[%d] Call the Requiem for the unmaterialized one.", NOW:to_turns()))
  if YOU:has_activity(ACT_READ) or YOU:has_activity(ACT_CRAFT) then
    -- For more precise, what you read is needed... but I don't know how to get.
    YOU:cancel_activity()
    mod.poppin(locale.gettext("Your e-copy is degrading!"))
    gapi.add_msg("You stop what you are doing and watch it vanishes.")
  end
  storage.hook_assuarance[NOW:to_turns()] = nil
  return false
end

-- Defence code of registering twice.
mod.__already_assured = mod.__already_assured or {}
-- This is for on_load_hook. But why is this doubled?
mod.assure_timer_hook = function()
  local NOW = gapi.current_turn() - gapi.turn_zero()
  if storage.hook_assuarance then
    -- gdebug.log_info(string.format("ebook_lua: Data loaded at %d turn", NOW:to_turns()))
    for the_when, val in pairs(storage.hook_assuarance) do
      if not mod.__already_assured[the_when] then
        mod.__already_assured[the_when] = true
        -- Main of assure timer hook.
        if val == "Requiem" then
          local booked_at = mod.from_turns(the_when)
          gdebug.log_info(string.format("Function [%s]: booked at [%d] turn after.", val, (booked_at - NOW):to_turns()))
          gapi.add_on_every_x_hook(booked_at, mod.item_funeral)
        end
      end
    end
  end
end

--[[
Scanning Process.
1. Check inventory
2. Find a book
3. Check tablet PC
4. Find data of the book
5. If not, store a book
]]
--
---@type fun(user: Character, device: Item): integer
mod.ebook_scan = function(user, device)
  --get all items in your inventory
  local your_items = user:all_items(false)
  local have_no_book = true
  local var_unzip = mod.unzip_var_lib2(device)
  local found = {}

  --let's find unscanned books!
  for i = 1, #your_items do
    local it = your_items[i]
    --Do the below for each book. But no virtual book!
    if it:is_book() and not it:has_var("aspect") then
      --Oh no. ItypeId is always different at everytime. I should work with string.
      local type_str = it:get_type():str()
      --One book for one type.
      if not var_unzip[type_str] then
        if have_no_book then
          have_no_book = false
        end
        --it:tname( int quantity, bool with_prefix, unsigned int truncate )
        --truncate limits the letters of the name.
        found[type_str] = it:tname(1, false, 0)
      end
    end
  end
  --so, do you have no book?
  if have_no_book then
    gapi.add_msg(locale.gettext("You have no book to be scanned."))
    return -1
  else
    local base_text =
      string.format(locale.gettext("%d books are found. Do you want to scan them all?\n"), mod.arr_num(found))
    local ui_limit = 20 -- Just assumption. I don't know its actual limit.
    local dev_limit = math.floor(device.charges / 10)
    local hit_end = (mod.arr_num(found) > dev_limit)

    local ui_scan = UiList.new()
    mod.ui_coloring(device, ui_scan)
    local count = 0
    for _, n_str in pairs(found) do
      if count == ui_limit then
        base_text = base_text .. string.format(locale.gettext("\n...And more! (%d)", found - count))
        break
      end
      base_text = base_text .. string.format("\n%s", n_str)
      count = count + 1
    end

    ui_scan:text(base_text)
    ui_scan:desc_enabled(true)
    ui_scan:add_w_desc(-1, locale.gettext("Scan all"), locale.gettext("Scan all the books until charges going off."))
    ui_scan:add_w_desc(-1, locale.gettext("Cancel"), locale.gettext("Discard the book list and go back."))
    while true do
      local sel_scan = ui_scan:query()
      if sel_scan == 0 then
        if hit_end then
          local YN = mod.poppyn(locale.gettext("WARNING: Not enough battery to scan all books.\nProceed anyway?"))
          if YN == "YES" then
            local count2 = 0
            for t_str, _ in pairs(found) do
              mod.insert_lib2(device, t_str)
              count2 = count2 + 1
              if count2 >= dev_limit then
                break
              end
            end
            gapi.add_msg(MsgType.good, string.format(locale.gettext("You scanned %d book(s)."), dev_limit))
            return dev_limit
          else --selected NO
            return -1
          end
        else
          for t_str, _ in pairs(found) do
            mod.insert_lib2(device, t_str)
          end
          gapi.add_msg(MsgType.good, string.format(locale.gettext("You scanned %d book(s)."), mod.arr_num(found)))
          return mod.arr_num(found)
        end
      else --selected Cancel or pressed ESC
        return -1
      end
    end
  end
end

--[[
Loading Process.
1. Check data of tablet PC
2. Make UiList of book data
3. Select one of them
4. ~~avatar::read runs!~~ <-- NO!
5. Spawn book but virtual.
]]
--
---@type fun(reader: Character, device: Item): integer
mod.ebook_load = function(reader, device)
  local ui_load = UiList.new()
  local tmp_arr = {}
  local lifetime = 60
  mod.ui_coloring(device, ui_load)
  ui_load:text(locale.gettext("Choose the book you want to get."))
  local unzip_var = mod.unzip_var_lib2(device)
  for type_str, _ in pairs(unzip_var) do
    local itype_id = ItypeId.new(type_str)
    local entry = {}
    entry.type = itype_id
    entry.name = gapi.create_item(itype_id, 1):tname(1, false, 32)
    tmp_arr[#tmp_arr + 1] = entry
    ui_load:add(-1, entry.name)
  end

  local minute = 0
  local sel = 0
  local u_input_too_big_minute = false
  local puip_str = PopupInputStr.new()
  puip_str:desc(locale.gettext("How long time do you need?(Digits only)"))
  puip_str:title(string.format(locale.gettext("Input the minute( up to %d ): "), device.charges - 10))
  -- Repeat when getting minute more than 0 AND less than charges-10.
  -- If you cancel to input number, minute is 0, and it will get back and show you data select menu.
  while true do
    if not u_input_too_big_minute then
      sel = ui_load:query()
    end
    u_input_too_big_minute = false
    if sel < 0 then
      --You canceled to select the book.
      return -1
    else
      minute = puip_str:query_int()
      if minute <= 0 then
        --You canceled to input minute.
        --Go back to select book menu.
      elseif minute > device.charges - 10 then
        --You input too big. Try again.
        mod.poppin(locale.gettext("Input value is over than available charges.\nPlease check your input."))
        u_input_too_big_minute = true
      else
        local selected = tmp_arr[sel + 1]
        --You got the book on the real world!
        reader:add_item_with_id(selected.type, 1)
        --Then lemme see it.
        local book_in_real = reader:get_item_with_id(selected.type, false)

        --The book should be labeled.
        book_in_real:set_var_str(
          "item_label",
          string.format(locale.gettext("[e-copy] %s"), string.gsub(selected.name, " %(unread%)", ""))
        ) -- no more "X (unread) (unread)"
        book_in_real:set_var_str("aspect", "virtual") --real, actual, virtual.
        --The book should have no weight nor volume.
        book_in_real:set_var_num("weight", 0)
        book_in_real:set_var_num("volume", 0)
        --The book should be gone.
        book_in_real:set_flag(flag_ETHEREAL_ITEM)
        lifetime = lifetime * minute
        book_in_real:set_var_num("ethereal", lifetime)
        --The book should be untradable. But it looks not effective.
        book_in_real:set_flag(flag_TRADER_AVOID)
        --The book should be no charges. Wait, what? Did it spawn with charges? It may be a bug from character::add_item_with_id.
        if book_in_real.charges ~= 0 then
          gdebug.log_info(
            string.format("%s has %d charge(s). Why does it happen?", selected.type:str(), book_in_real.charges)
          )
          book_in_real.charges = 0
        end

        mod.turntimer_hook(lifetime - 1, mod.item_funeral)

        local qp = QueryPopup.new()
        qp:message(
          string.format(
            locale.gettext(
              "You printed a physical copy of %s.\nIt’s virtually weightless and will degrade naturally in about \n[%d minutes.]\nYou can return it to the device to recover some energy."
            ),
            selected.name,
            mod.from_turns(lifetime):to_minutes()
          )
        )
        qp:allow_any_key(true)
        qp:query()
        return minute
      end
    end
  end
end

---@type fun(reader: Character, device: Item): integer
mod.ebook_return = function(reader, device)
  --let's find the books
  local your_items = reader:all_items(false)
  local virtual_books = {}
  for _, it in pairs(your_items) do
    if it:has_var("aspect") then
      virtual_books[#virtual_books + 1] = it
    end
  end
  if #virtual_books <= 0 then
    mod.poppin(locale.gettext("There is no book to return."))
    return -1
  end
  local letter_limit = 24
  local ui_return = UiList.new()
  mod.ui_coloring(device, ui_return)
  ui_return:text("Choose the book to return.")
  for key, it in pairs(virtual_books) do
    ui_return:add_w_col(
      key,
      it:tname(1, false, letter_limit),
      "NO_DESC",
      string.format("+%d%%", math.floor(it:get_var_num("ethereal", 0) / 60 / device:ammo_capacity(false) * 100))
    )
  end
  local sel_return = ui_return:query()
  if sel_return <= 0 then
    return -1
  else
    local it = virtual_books[sel_return]
    local ammo_type = device:ammo_current()
    device:ammo_set(ammo_type, device.charges + math.floor(it:get_var_num("ethereal", 0) / 60))
    it:set_var_num("ethereal", 0)
    --DON'T READ VANISHING ITEM EVER!!!
    reader:mod_moves(-250)
    return 0
  end
end

---@type fun(reader: Character, device: Item): integer
mod.check_lib = function(reader, device)
  local uid = mod.item_uid(device)
  local book_data = mod.unzip_var_lib2(device)
  local book_data_list = ""
  if mod.arr_num(book_data) < 1 then
    mod.poppin(locale.gettext("There is no book in the device."))
    return -1
  end
  for k_ity_str, _ in pairs(book_data) do
    book_data_list = book_data_list .. "\n" .. gapi.create_item(ItypeId.new(k_ity_str), 1):tname(1, false, 0)
  end
  mod.poppin(string.format(locale.gettext("UID:%s\n\nThis device has book data as below:\n%s"), uid, book_data_list))
  return -1
end

---@type fun(reader: Character, device: Item): integer
mod.mc_io = function(reader, device)
  local uid = device:get_var_str("tablet_uid", "nope")
  local your_mc = {}
  local your_items = reader:all_items(false)
  --Let's make your_mc!
  for _, it in pairs(your_items) do
    if it:has_flag(flag_MC_USED) then
      your_mc[#your_mc + 1] = it
    end
  end
  -- Escape when no mc.
  if #your_mc == 0 then
    mod.poppin(locale.gettext("You don't have any empty memory card."))
    return -1
  end
  -- Now we have your_mc table.
  local mc_sel_ui = UiList.new()
  mod.ui_coloring(device, mc_sel_ui)
  mc_sel_ui:text(string.format(locale.gettext("UID: %s\n\nSelect a memory card."), uid))
  for k_mc, v_mc in pairs(your_mc) do
    mc_sel_ui:add(k_mc, string.format("%s", v_mc:tname(1, false, 0)))
  end
  while true do
    for k_num, entry in pairs(mc_sel_ui.entries) do
      entry.txt = your_mc[k_num]:tname(1, false, 0)
    end
    local ans_mc_sel = mc_sel_ui:query()
    if ans_mc_sel <= 0 then
      return -1
    else
      local mc_menu_ui = UiList.new()
      mc_menu_ui:desc_enabled(true)
      mod.ui_coloring(device, mc_menu_ui)
      mc_menu_ui:add_w_desc(-1, "List book data", "Show the book data list from memory card.")
      mc_menu_ui:add_w_desc(-1, "Download book data", "Download book data from memory card.")
      mc_menu_ui:add_w_desc(-1, "Upload book data", "Upload book data to memory card.")
      mc_menu_ui:add_w_desc(-1, "Remove all book data", "Remove all book data of memory card.")
      while true do
        local that_mc = your_mc[ans_mc_sel]
        mc_menu_ui:text(
          string.format(
            locale.gettext("UID: %s\n\nMemory card data I/O online.\nCurrent card: %s"),
            uid,
            that_mc:tname(1, false, 0)
          )
        )
        local data_in_dev = mod.unzip_var_lib2(device)
        local data_in_card = mod.unzip_var_lib2(that_mc)
        local ans_mc_menu = mc_menu_ui:query()
        if ans_mc_menu < 0 then
          break
        elseif ans_mc_menu == 0 then --List
          local book_data_mc_text = ""
          for k_ity_str, _ in pairs(data_in_card) do
            book_data_mc_text = book_data_mc_text
              .. "\n"
              .. gapi.create_item(ItypeId.new(k_ity_str), 1):tname(1, false, 0)
          end
          mod.poppin(
            string.format(locale.gettext("Book data of %s:\n%s"), that_mc:tname(1, false, 0), book_data_mc_text)
          )
        elseif ans_mc_menu == 1 then --Download
          local dl_count = 0
          for type_str, _ in pairs(data_in_card) do
            if not data_in_dev[type_str] then
              mod.insert_lib2(device, type_str)
              dl_count = dl_count + 1
            end
          end
          if dl_count == 0 then
            mod.poppin(string.format(locale.gettext("%s has nothing new books."), that_mc:tname(1, false, 0)))
          else
            mod.poppin(
              string.format(locale.gettext("%d books downloaded from %s."), dl_count, that_mc:tname(1, false, 0))
            )
          end
        elseif ans_mc_menu == 2 then --Upload
          local ul_count = 0
          for key, _ in pairs(data_in_dev) do
            if not data_in_card[key] then
              mod.insert_lib2(that_mc, key)
              ul_count = ul_count + 1
            end
          end
          if ul_count == 0 then
            mod.poppin(string.format(locale.gettext("%s has the same books already."), that_mc:tname(1, false, 0)))
          else
            mod.poppin(
              string.format(
                locale.gettext("%d books uploaded on %s.\nNaming the card is recommended."),
                ul_count,
                that_mc:tname(1, false, 0)
              )
            )
            that_mc:set_var_str("name", locale.gettext("e-book saved SD-Memory Card"))
          end
        elseif ans_mc_menu == 3 then --Remove all
          if not that_mc:has_var("book_data") then
            mod.poppin(locale.gettext("It has no book data."))
          else
            local yn1 = mod.poppyn(
              string.format(
                locale.gettext("Do you want to delete all of book data of %s? It's irretrievable!"),
                that_mc:tname(1, false, 0)
              )
            )
            if yn1 == "YES" then
              local yn2 = mod.poppyn(locale.gettext("Are you sure to delete book data in the card?"))
              if yn2 == "YES" then
                that_mc:erase_var("name")
                that_mc:erase_var("book_data")
                mod.poppin(locale.gettext("The memory card is reset successfully."))
                break
              end
            end
          end
        end
      end
    end
  end
end

-- cloud sync. but that never works in this world.
---@type fun(reader: Character, device: Item): integer
mod.cloud_sync = function(reader, device)
  mod.poppin(
    locale.gettext(
      "ERROR: Network Connection Failed\n\nConnection denied by Protocol Zero. Server access is temporarily suspended.\nPlease wait for further updates."
    )
  )
  return -1
end

-- Reset library. Double caution.
---@type fun(reader: Character, device: Item): integer
mod.reset_lib = function(reader, device)
  local yn1 = mod.poppyn(
    string.format(
      locale.gettext("Do you want to delete all of book data of %s? It's irretrievable!"),
      device:tname(1, false, 0)
    )
  )
  if yn1 == "YES" then
    local yn2 = mod.poppyn(locale.gettext("Are you sure to delete book data in the device?"))
    if yn2 == "YES" then
      device:erase_var("book_data")
      mod.poppin(locale.gettext("The device is reset successfully."))
      return 0
    end
  end
  return -1
end

---@type fun(reader: Character, device: Item): integer
mod.ebook_info = function(reader, device)
  local uid = mod.item_uid(device)
  local ui_info = UiList.new()
  mod.ui_coloring(device, ui_info)
  ui_info:desc_enabled(true)
  ui_info:add_w_desc(-1, "Check library", "Check the book data the device has.")
  ui_info:add_w_desc(-1, "Memory card menu", "Operate book data with memory card.")
  ui_info:add_w_desc(-1, "Cloud sync", "Read / Write data from cloud server. It needs network connection.")
  ui_info:add_w_desc(-1, "Reset library", "Delete ALL book data in this device.")
  while true do
    local day = (gapi.current_turn() - gapi.turn_zero()):to_days() + 3
    ui_info:text(
      string.format(
        locale.gettext("UID: %s\n\nNetwork: Offline (Cloud unreachable)\n\nLast Sync: %d days ago"),
        uid,
        day
      )
    )
    local ans_info = ui_info:query()
    if ans_info < 0 then
      return -1
    elseif ans_info == 0 then
      local ch_lib = mod.check_lib(reader, device)
      if ch_lib ~= -1 then
        return ch_lib
      end
    elseif ans_info == 1 then
      local mc_io = mod.mc_io(reader, device)
      if mc_io ~= -1 then
        return mc_io
      end
    elseif ans_info == 2 then
      local clo_syn = mod.cloud_sync(reader, device)
      if clo_syn ~= -1 then
        return clo_syn
      end
    elseif ans_info == 3 then
      local res_lib = mod.reset_lib(reader, device)
      if res_lib ~= -1 then
        return res_lib
      end
    end
  end
end

-- The first action for this Lua
---@type fun(who: Character, item: Item, pos: integer): integer
mod.ebook_ui = function(who, item, pos)
  local unzip_var = mod.unzip_var_lib2(item)
  local var_count = mod.arr_num(unzip_var)
  local ui = UiList.new()
  ui:desc_enabled(true)
  ui:text(
    string.format(
      locale.gettext("Welcome to e-book library!\nThis device currently holds %d book(s).\nNetwork sync unavilable."),
      var_count
    )
  )
  ui:add_w_desc(
    -1,
    locale.gettext("Scan book(s)"),
    locale.gettext("Scans all the book you have. It will progress instantly.")
  )
  ui:add_w_desc(
    -1,
    locale.gettext("Load book(s)"),
    locale.gettext("Temporarily prints a physical copy from the device’s library.")
  )
  ui:add_w_desc(
    -1,
    locale.gettext("Return book(s)"),
    locale.gettext("Absorbs the copy back into the device and restores partial power.")
  )
  ui:add_w_desc(-1, locale.gettext("Info.."), locale.gettext("Shows UID, list of data, explanation for customers!"))

  if var_count == 0 then
    ui.entries[2].ctxt = locale.gettext("No book in device!")
    ui.entries[2].enable = false
  end

  while true do
    mod.ui_coloring(item, ui)
    local ans = ui:query()
    if ans < 0 then
      gapi.add_msg("Never mind.")
      return 0
    elseif ans == 0 then
      local scanned = mod.ebook_scan(who, item)
      if scanned ~= -1 then
        return scanned
      end
    elseif ans == 1 then
      local loaded = mod.ebook_load(who, item)
      if loaded ~= -1 then
        return loaded
      end
    elseif ans == 2 then
      local returned = mod.ebook_return(who, item)
      if returned ~= -1 then
        return returned
      end
    elseif ans == 3 then
      local info = mod.ebook_info(who, item)
      if info ~= -1 then
        return info
      end
    else
      gdebug.log_info("ebook_lua: Error occurred. What the heck did you select?")
      return 0
    end
  end
end
