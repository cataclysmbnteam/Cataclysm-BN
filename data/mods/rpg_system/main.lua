gdebug.log_info("RPG System: Main")

local mod = game.mod_runtime[game.current_mod]
local storage = game.mod_storage[game.current_mod]
local function color_text(text, color) return string.format("<color_%s>%s</color>", color, text) end

local function color_good(text) return color_text(text, "light_green") end

local function color_bad(text) return color_text(text, "light_red") end

local function color_warning(text) return color_text(text, "yellow") end

local function color_info(text) return color_text(text, "light_cyan") end

local function color_highlight(text) return color_text(text, "white") end

local function create_progress_bar(current, max, width)
  width = width or 20
  local filled = math.floor((current / max) * width)
  local empty = width - filled
  local bar = color_good(string.rep("█", filled)) .. color_text(string.rep("░", empty), "dark_gray")
  local percent = math.floor((current / max) * 100)
  return string.format("[%s] %d%%", bar, percent)
end

local function wrap_text(text, width, indent)
  width = width or 60
  indent = indent or ""

  if #text <= width then return indent .. text end

  local lines = {}
  local current_line = ""

  for word in text:gmatch("%S+") do
    local test_line = current_line == "" and word or (current_line .. " " .. word)
    if #test_line <= width then
      current_line = test_line
    else
      if current_line ~= "" then table.insert(lines, indent .. current_line) end
      current_line = word
    end
  end

  if current_line ~= "" then table.insert(lines, indent .. current_line) end

  return table.concat(lines, "\n")
end

-- Experience Curve Functions for Levels 1-40
-- Formula: XP = 2.2387 * level^3.65
local XP_COEFFICIENT = 2.2387
local XP_EXPONENT = 3.65

local function get_rpg_level(exp)
  local level = math.floor((exp / XP_COEFFICIENT) ^ (1 / XP_EXPONENT))
  return math.min(level, 40)
end

local function rpg_xp_needed(level)
  if level >= 40 then return XP_COEFFICIENT * (40 ^ XP_EXPONENT) end
  return XP_COEFFICIENT * (level ^ XP_EXPONENT)
end

local function get_char_value(char, key, default)
  local val = char:get_value(key)
  if val == "" then return default end
  return tonumber(val) or default
end

local function set_char_value(char, key, value) char:set_value(key, tostring(value)) end

-- Ehhh, these aren't great and should be refactored if many more classes are added.
local function has_class(char)
  local classes = {
    MutationBranchId.new("RPG_WARRIOR"),
    MutationBranchId.new("RPG_BERSERKER"),
    MutationBranchId.new("RPG_GUARDIAN"),
    MutationBranchId.new("RPG_MAGE"),
    MutationBranchId.new("RPG_MYSTIC"),
    MutationBranchId.new("RPG_SCHOLAR"),
    MutationBranchId.new("RPG_SCOUT"),
    MutationBranchId.new("RPG_RANGER"),
    MutationBranchId.new("RPG_ASSASSIN"),
  }

  for _, class in ipairs(classes) do
    if char:has_trait(class) then return true end
  end
  return false
end

local function get_class_info(char)
  local class_ids = {
    MutationBranchId.new("RPG_WARRIOR"),
    MutationBranchId.new("RPG_BERSERKER"),
    MutationBranchId.new("RPG_GUARDIAN"),
    MutationBranchId.new("RPG_MAGE"),
    MutationBranchId.new("RPG_MYSTIC"),
    MutationBranchId.new("RPG_SCHOLAR"),
    MutationBranchId.new("RPG_SCOUT"),
    MutationBranchId.new("RPG_RANGER"),
    MutationBranchId.new("RPG_ASSASSIN"),
  }

  for _, class_id in ipairs(class_ids) do
    if char:has_trait(class_id) then
      local class_obj = class_id:obj()
      return class_obj:name(), class_obj:desc()
    end
  end
  return "[None]", nil
end

local function get_current_traits(char)
  local trait_ids = {
    MutationBranchId.new("RPG_TRAIT_BIONIC_SYMBIOTE"),
    MutationBranchId.new("RPG_TRAIT_VITAL_ESSENCE"),
    MutationBranchId.new("RPG_TRAIT_RADIOACTIVE_BLOOD"),
    MutationBranchId.new("RPG_TRAIT_IRON_HIDE"),
    MutationBranchId.new("RPG_TRAIT_LIGHTWEIGHT"),
    MutationBranchId.new("RPG_TRAIT_PACK_MULE"),
    MutationBranchId.new("RPG_TRAIT_TIRELESS"),
    MutationBranchId.new("RPG_TRAIT_MANA_FONT"),
    MutationBranchId.new("RPG_TRAIT_BLINK_STEP"),
    MutationBranchId.new("RPG_TRAIT_NATURAL_HEALER"),
    MutationBranchId.new("RPG_TRAIT_ADAPTIVE_BIOLOGY"),
    MutationBranchId.new("RPG_TRAIT_GLASS_CANNON"),
    MutationBranchId.new("RPG_TRAIT_JUGGERNAUT"),
    MutationBranchId.new("RPG_TRAIT_ARCANE_BATTERY"),
    MutationBranchId.new("RPG_TRAIT_IRON_FISTS"),
    MutationBranchId.new("RPG_TRAIT_EFFICIENT_METABOLISM"),
    MutationBranchId.new("RPG_TRAIT_COMBAT_REFLEXES"),
    MutationBranchId.new("RPG_TRAIT_ACROBAT"),
    MutationBranchId.new("RPG_TRAIT_TIRELESS_WORKER"),
    MutationBranchId.new("RPG_TRAIT_RAPID_METABOLISM"),
    MutationBranchId.new("RPG_TRAIT_SCENTLESS"),
    MutationBranchId.new("RPG_TRAIT_CLOTTING_FACTOR"),
    MutationBranchId.new("RPG_TRAIT_REGENERATOR"),
    MutationBranchId.new("RPG_TRAIT_MASTER_CRAFTSMAN"),
    MutationBranchId.new("RPG_TRAIT_PACK_RAT"),
    MutationBranchId.new("RPG_TRAIT_NATURAL_BUTCHER"),
  }

  local current = {}
  for _, trait_id in ipairs(trait_ids) do
    if char:has_trait(trait_id) then
      local trait_obj = trait_id:obj()
      table.insert(current, { name = trait_obj:name(), desc = trait_obj:desc() })
    end
  end
  return current
end

mod.on_game_started = function()
  local player = gapi.get_avatar()

  set_char_value(player, "rpg_level", 0)
  set_char_value(player, "rpg_num_traits", 0)
  set_char_value(player, "rpg_max_traits", 1)
  set_char_value(player, "rpg_exp", 0)
  set_char_value(player, "rpg_xp_to_next_level", rpg_xp_needed(1))
  set_char_value(player, "rpg_stat_points", 0)
  set_char_value(player, "rpg_assigned_str", 0)
  set_char_value(player, "rpg_assigned_dex", 0)
  set_char_value(player, "rpg_assigned_int", 0)
  set_char_value(player, "rpg_assigned_per", 0)
  set_char_value(player, "rpg_level_scaling", 100)

  player:add_item_with_id(ItypeId.new("system_interface"), 1)

  gapi.add_msg(
    MsgType.mixed,
    color_info("[System]")
      .. " initialized. Welcome "
      .. color_highlight("[User]")
      .. " of world IB-73758-R. Use your "
      .. color_info("[System Interface]")
      .. " to access the "
      .. color_info("[System]")
      .. "."
  )
end

mod.on_game_load = function()
  local player = gapi.get_avatar()

  local level = get_char_value(player, "rpg_level", -1)
  if level < 0 then
    set_char_value(player, "rpg_level", 0)
    set_char_value(player, "rpg_num_traits", 0)
    set_char_value(player, "rpg_max_traits", 1)
    set_char_value(player, "rpg_exp", 0)
    set_char_value(player, "rpg_xp_to_next_level", rpg_xp_needed(1))
    set_char_value(player, "rpg_stat_points", 0)
    set_char_value(player, "rpg_assigned_str", 0)
    set_char_value(player, "rpg_assigned_dex", 0)
    set_char_value(player, "rpg_assigned_int", 0)
    set_char_value(player, "rpg_assigned_per", 0)
    set_char_value(player, "rpg_level_scaling", 100)

    player:add_item_with_id(ItypeId.new("system_interface"), 1)

    gapi.add_msg(
      MsgType.mixed,
      color_info("[System]")
        .. " initialized. Welcome "
        .. color_highlight("[User]")
        .. " of world IB-73758-R. Use your "
        .. color_info("[System Interface]")
        .. " to access the "
        .. color_info("[System]")
        .. "."
    )
    gdebug.log_info("RPG System: Initialized on game load")
    return
  end

  gdebug.log_info("RPG System: Loaded character at level " .. level)
end

mod.on_monster_killed = function(params)
  local killer = params.killer
  local monster = params.mon

  if not killer or not monster then return end
  if not killer:is_avatar() then return end

  local player = killer
  local exp = get_char_value(player, "rpg_exp", 0)
  local old_level = get_char_value(player, "rpg_level", 0)

  local monster_hp = monster:get_hp_max()
  local xp_gain = math.max(1, math.floor(monster_hp / 10))
  exp = exp + xp_gain
  set_char_value(player, "rpg_exp", exp)

  local new_level = get_rpg_level(exp)
  if new_level > old_level then
    set_char_value(player, "rpg_level", new_level)

    local level_msg = color_good("★ ")
      .. color_highlight("LEVEL UP!")
      .. color_good(" ★")
      .. " You are now "
      .. color_info("Level " .. new_level)
      .. "!"
    gapi.add_msg(MsgType.good, level_msg)

    local max_traits = 1 + math.floor(new_level / 7)
    set_char_value(player, "rpg_max_traits", max_traits)

    if new_level % 7 == 0 and new_level > 0 then
      gapi.add_msg(
        MsgType.good,
        color_good("New trait slot unlocked!") .. " You now have " .. color_highlight(max_traits) .. " trait slots."
      )
    end

    if new_level % 2 == 0 and new_level > 0 then
      local stat_points = get_char_value(player, "rpg_stat_points", 0)
      stat_points = stat_points + 1
      set_char_value(player, "rpg_stat_points", stat_points)
      gapi.add_msg(
        MsgType.good,
        color_good("✦ Stat point earned!")
          .. " You have "
          .. color_highlight(stat_points)
          .. " unassigned stat point"
          .. (stat_points > 1 and "s" or "")
          .. "."
      )
    end
  end

  local xp_needed = rpg_xp_needed(new_level + 1)
  local xp_to_next = xp_needed - exp
  set_char_value(player, "rpg_xp_to_next_level", xp_to_next)
end

mod.on_character_reset_stats = function(params)
  local character = params.character
  if not character then return end
  if not character:is_avatar() then return end

  local level = get_char_value(character, "rpg_level", 0)
  local level_scaling = get_char_value(character, "rpg_level_scaling", 100) / 100.0

  local assigned_str = get_char_value(character, "rpg_assigned_str", 0)
  local assigned_dex = get_char_value(character, "rpg_assigned_dex", 0)
  local assigned_int = get_char_value(character, "rpg_assigned_int", 0)
  local assigned_per = get_char_value(character, "rpg_assigned_per", 0)

  character:mod_str_bonus(assigned_str)
  character:mod_dex_bonus(assigned_dex)
  character:mod_int_bonus(assigned_int)
  character:mod_per_bonus(assigned_per)

  -- These should also be refactored if more classes are added probably.
  if character:has_trait(MutationBranchId.new("RPG_WARRIOR")) then
    character:mod_str_bonus(math.floor(level * 0.55 * level_scaling))
    character:mod_dex_bonus(math.floor(level * 0.2 * level_scaling))
    character:mod_int_bonus(math.floor(level * 0.1 * level_scaling))
    character:mod_per_bonus(math.floor(level * 0.15 * level_scaling))
  end

  if character:has_trait(MutationBranchId.new("RPG_MAGE")) then
    character:mod_str_bonus(math.floor(level * 0.1 * level_scaling))
    character:mod_dex_bonus(math.floor(level * 0.05 * level_scaling))
    character:mod_int_bonus(math.floor(level * 0.55 * level_scaling))
    character:mod_per_bonus(math.floor(level * 0.3 * level_scaling))
  end

  if character:has_trait(MutationBranchId.new("RPG_SCOUT")) then
    character:mod_str_bonus(math.floor(level * 0.1 * level_scaling))
    character:mod_dex_bonus(math.floor(level * 0.55 * level_scaling))
    character:mod_int_bonus(math.floor(level * 0.05 * level_scaling))
    character:mod_per_bonus(math.floor(level * 0.3 * level_scaling))
  end

  if character:has_trait(MutationBranchId.new("RPG_BERSERKER")) then
    character:mod_str_bonus(math.floor(level * 0.75 * level_scaling))
    character:mod_dex_bonus(math.floor(level * 0.45 * level_scaling))
    character:mod_int_bonus(math.floor(level * 0.1 * level_scaling))
    character:mod_per_bonus(math.floor(level * 0.2 * level_scaling))
  end

  if character:has_trait(MutationBranchId.new("RPG_GUARDIAN")) then
    character:mod_str_bonus(math.floor(level * 0.6 * level_scaling))
    character:mod_dex_bonus(math.floor(level * 0.15 * level_scaling))
    character:mod_int_bonus(math.floor(level * 0.15 * level_scaling))
    character:mod_per_bonus(math.floor(level * 0.6 * level_scaling))
  end

  if character:has_trait(MutationBranchId.new("RPG_MYSTIC")) then
    character:mod_str_bonus(math.floor(level * 0.1 * level_scaling))
    character:mod_dex_bonus(math.floor(level * 0.2 * level_scaling))
    character:mod_int_bonus(math.floor(level * 0.75 * level_scaling))
    character:mod_per_bonus(math.floor(level * 0.45 * level_scaling))
  end

  if character:has_trait(MutationBranchId.new("RPG_SCHOLAR")) then
    character:mod_str_bonus(math.floor(level * 0.1 * level_scaling))
    character:mod_dex_bonus(math.floor(level * 0.3 * level_scaling))
    character:mod_int_bonus(math.floor(level * 0.8 * level_scaling))
    character:mod_per_bonus(math.floor(level * 0.3 * level_scaling))
  end

  if character:has_trait(MutationBranchId.new("RPG_RANGER")) then
    character:mod_str_bonus(math.floor(level * 0.15 * level_scaling))
    character:mod_dex_bonus(math.floor(level * 0.65 * level_scaling))
    character:mod_int_bonus(math.floor(level * 0.1 * level_scaling))
    character:mod_per_bonus(math.floor(level * 0.6 * level_scaling))
  end

  if character:has_trait(MutationBranchId.new("RPG_ASSASSIN")) then
    character:mod_str_bonus(math.floor(level * 0.1 * level_scaling))
    character:mod_dex_bonus(math.floor(level * 0.75 * level_scaling))
    character:mod_int_bonus(math.floor(level * 0.35 * level_scaling))
    character:mod_per_bonus(math.floor(level * 0.3 * level_scaling))
  end

  if
    character:has_trait(MutationBranchId.new("RPG_SCOUT"))
    or character:has_trait(MutationBranchId.new("RPG_RANGER"))
  then
    character:mod_speed_bonus(math.floor(level * 1 * level_scaling))
  end

  if character:has_trait(MutationBranchId.new("RPG_ASSASSIN")) then
    character:mod_speed_bonus(math.floor(level * 2 * level_scaling))
  end
end

mod.on_every_5_minutes = function()
  local player = gapi.get_avatar()
  if not player then return end

  local level = get_char_value(player, "rpg_level", 0)
  if level <= 0 then return end

  local level_scaling = get_char_value(player, "rpg_level_scaling", 100) / 100.0

  if
    player:has_trait(MutationBranchId.new("RPG_WARRIOR"))
    or player:has_trait(MutationBranchId.new("RPG_BERSERKER"))
    or player:has_trait(MutationBranchId.new("RPG_GUARDIAN"))
  then
    player:mod_fatigue(math.floor(-level * 0.5 * level_scaling))
    player:mod_stamina(math.floor(level * 20 * level_scaling))
  end

  if player:has_trait(MutationBranchId.new("RPG_RANGER")) then
    player:mod_thirst(math.floor(-level * 1.5 * level_scaling))
  end

  if player:has_trait(MutationBranchId.new("RPG_TRAIT_VITAL_ESSENCE")) then
    player:mod_healthy_mod(0.1 * level * level_scaling, 100)
  end

  if player:has_trait(MutationBranchId.new("RPG_TRAIT_RADIOACTIVE_BLOOD")) then
    player:mod_rad(math.floor(-level * 0.5 * level_scaling))
  end

  if player:has_trait(MutationBranchId.new("RPG_TRAIT_BIONIC_SYMBIOTE")) then
    local power_regen = Energy.from_joule(math.floor(level * 1 * 1000 * level_scaling))
    player:mod_power_level(power_regen)
  end
end

mod.open_rpg_menu = function(who, item, pos)
  if not who:is_avatar() then return 0 end

  local player = who
  local keep_open = true

  while keep_open do
    local exp = get_char_value(player, "rpg_exp", 0)
    local level = get_char_value(player, "rpg_level", 0)
    local num_traits = get_char_value(player, "rpg_num_traits", 0)
    local max_traits = get_char_value(player, "rpg_max_traits", 1)
    local xp_to_next = get_char_value(player, "rpg_xp_to_next_level", 0)
    local class_name, class_desc = get_class_info(player)
    local current_traits = get_current_traits(player)

    local ui = UiList.new()
    ui:title("=== [SYSTEM] ===")

    local current_level_xp = rpg_xp_needed(level)
    local next_level_xp = rpg_xp_needed(level + 1)
    local xp_in_level = exp - current_level_xp
    local xp_for_level = next_level_xp - current_level_xp

    local str_val = player:get_str()
    local dex_val = player:get_dex()
    local int_val = player:get_int()
    local per_val = player:get_per()
    local player_name = player:get_name()

    local info_text = ""

    info_text = info_text .. color_highlight("Name: ") .. color_good(player_name) .. "\n"
    info_text = info_text .. color_highlight("Level: ") .. color_good(string.format("%d", level))
    if level < 40 then
      info_text = info_text .. color_text(string.format(" (Next: %.0f XP)", xp_to_next), "light_gray")
    else
      info_text = info_text .. color_warning(" [MAX]")
    end
    info_text = info_text .. "\n"

    if level < 40 then
      local progress = create_progress_bar(xp_in_level, xp_for_level, 30)
      info_text = info_text .. "XP: " .. progress .. "\n"
    end

    info_text = info_text .. color_highlight("Total XP: ") .. string.format("%.0f", exp) .. "\n"
    info_text = info_text
      .. color_text(
        "─────────────────────────────────────────",
        "light_gray"
      )
      .. "\n"

    local stat_points = get_char_value(player, "rpg_stat_points", 0)
    info_text = info_text .. color_highlight("Stats:") .. "\n"
    info_text = info_text
      .. string.format(
        "  %s %s  %s %s  %s %s  %s %s\n",
        color_text("STR:", "light_gray"),
        color_good(str_val),
        color_text("DEX:", "light_gray"),
        color_good(dex_val),
        color_text("INT:", "light_gray"),
        color_good(int_val),
        color_text("PER:", "light_gray"),
        color_good(per_val)
      )
    if stat_points > 0 then
      info_text = info_text
        .. color_good(
          "  ✦ " .. stat_points .. " unassigned stat point" .. (stat_points > 1 and "s" or "") .. " available!\n"
        )
    end

    local all_skills = {
      { name = "Dodge", skill = "dodge" },
      { name = "Melee", skill = "melee" },
      { name = "Unarmed", skill = "unarmed" },
      { name = "Bashing", skill = "bashing" },
      { name = "Cutting", skill = "cutting" },
      { name = "Piercing", skill = "stabbing" },
      { name = "Archery", skill = "archery" },
      { name = "Handguns", skill = "gun" },
      { name = "Rifles", skill = "rifle" },
      { name = "Shotguns", skill = "shotgun" },
      { name = "SMGs", skill = "smg" },
      { name = "Launchers", skill = "launcher" },
      { name = "Throwing", skill = "throw" },
      { name = "Cooking", skill = "cooking" },
      { name = "Tailoring", skill = "tailor" },
      { name = "Mechanics", skill = "mechanics" },
      { name = "Electronics", skill = "electronics" },
      { name = "Fabrication", skill = "fabrication" },
      { name = "First Aid", skill = "firstaid" },
      { name = "Computers", skill = "computer" },
      { name = "Survival", skill = "survival" },
      { name = "Trapping", skill = "traps" },
      { name = "Swimming", skill = "swimming" },
      { name = "Driving", skill = "driving" },
      { name = "Bartering", skill = "barter" },
      { name = "Speech", skill = "speech" },
      { name = "Spellcraft", skill = "spellcraft" },
    }

    info_text = info_text .. color_highlight("Skills:") .. "\n"

    local displayed_skills = {}
    for _, skill_info in ipairs(all_skills) do
      local skill_level = player:get_skill_level(SkillId.new(skill_info.skill))
      if skill_level > 0 then table.insert(displayed_skills, { name = skill_info.name, level = skill_level }) end
    end

    -- Display trained skills
    if #displayed_skills > 0 then
      local skill_count = 0
      for _, skill_data in ipairs(displayed_skills) do
        local skill_color = skill_data.level >= 5 and "light_green"
          or (skill_data.level >= 3 and "white" or "light_gray")
        info_text = info_text
          .. string.format(
            "  %s %s",
            color_text(skill_data.name .. ":", "light_gray"),
            color_text(skill_data.level, skill_color)
          )
        skill_count = skill_count + 1
        if skill_count % 3 == 0 then
          info_text = info_text .. "\n"
        else
          info_text = info_text .. "  "
        end
      end
      if skill_count % 3 ~= 0 then info_text = info_text .. "\n" end
    else
      info_text = info_text .. color_text("  No skills trained yet.", "dark_gray") .. "\n"
    end

    info_text = info_text
      .. color_text(
        "─────────────────────────────────────────",
        "light_gray"
      )
      .. "\n"

    -- Class info
    if class_name == "[None]" then
      info_text = info_text .. color_highlight("Class: ") .. color_warning(class_name) .. "\n"
    else
      info_text = info_text .. color_highlight("Class: ") .. color_good(class_name) .. "\n"
      if class_desc then
        local wrapped = wrap_text(class_desc, 60, "  ")
        info_text = info_text .. color_text(wrapped, "light_gray") .. "\n"
      end
    end

    info_text = info_text
      .. color_text(
        "─────────────────────────────────────────",
        "light_gray"
      )
      .. "\n"

    -- Trait section
    info_text = info_text
      .. color_highlight("Trait Slots: ")
      .. color_good(string.format("%d", num_traits))
      .. color_text("/", "light_gray")
      .. color_highlight(string.format("%d", max_traits))
      .. "\n"

    if #current_traits > 0 then
      info_text = info_text .. color_highlight("Active Traits:") .. "\n"
      for _, trait in ipairs(current_traits) do
        info_text = info_text .. color_good("  • " .. trait.name) .. "\n"
        local wrapped = wrap_text(trait.desc, 58, "    ")
        info_text = info_text .. color_text(wrapped, "light_gray") .. "\n"
      end
    else
      info_text = info_text .. color_text("  No traits selected yet.", "dark_gray") .. "\n"
    end

    info_text = info_text .. "\n"

    ui:text(info_text)

    local menu_items = {}
    table.insert(menu_items, { text = "Manage Class", action = "class" })

    if stat_points > 0 then
      table.insert(
        menu_items,
        { text = color_good("Assign Stats (" .. stat_points .. " available)"), action = "stats" }
      )
    end

    if num_traits < max_traits then table.insert(menu_items, { text = "Manage Traits", action = "traits" }) end

    table.insert(menu_items, { text = "Help", action = "help" })
    table.insert(menu_items, { text = "Close", action = "close" })

    for i, item_entry in ipairs(menu_items) do
      ui:add(i, item_entry.text)
    end

    local choice_index = ui:query()

    if choice_index > 0 and choice_index <= #menu_items then
      local chosen = menu_items[choice_index]

      if chosen.action == "class" then
        mod.manage_class_menu(player)
      elseif chosen.action == "stats" then
        mod.assign_stats_menu(player)
      elseif chosen.action == "traits" then
        mod.manage_traits_menu(player)
      elseif chosen.action == "help" then
        mod.show_help_menu(player)
      elseif chosen.action == "close" then
        keep_open = false
      end
    else
      keep_open = false
    end
  end

  return 0
end

local function format_requirement(label, current, required, show_current)
  local met = current >= required
  local color_fn = met and color_good or color_bad
  if show_current then
    return color_fn(string.format("%s %d/%d", label, current, required))
  else
    return color_fn(string.format("%s %d+", label, required))
  end
end

mod.manage_class_menu = function(player)
  local level = get_char_value(player, "rpg_level", 0)
  local str_val = player:get_str()
  local dex_val = player:get_dex()
  local int_val = player:get_int()
  local per_val = player:get_per()

  local ui = UiList.new()
  ui:title("=== Select Class ===")
  ui:desc_enabled(true)

  local options = {}
  local index = 1

  if not has_class(player) then
    local warrior_id = MutationBranchId.new("RPG_WARRIOR")
    local warrior_meets = str_val >= 8
    -- Honestly, these should also definitely be refactored if more classes are added to be
    -- a generic format requirements function. That takes in class/trait requirements and returns
    -- a formatted string.
    local warrior_text = warrior_meets and color_good("⚔ [Warrior]")
      or color_text("⚔ [Warrior]", "dark_gray") .. " - " .. format_requirement("STR", str_val, 8, true)
    table.insert(options, {
      text = warrior_text,
      desc = warrior_id:obj():desc(),
      id = warrior_id,
      can_select = warrior_meets,
      index = index,
    })
    index = index + 1

    local mage_id = MutationBranchId.new("RPG_MAGE")
    local mage_meets = int_val >= 8
    local mage_text = mage_meets and color_good("✦ [Mage]")
      or color_text("✦ [Mage]", "dark_gray") .. " - " .. format_requirement("INT", int_val, 8, true)
    table.insert(options, {
      text = mage_text,
      desc = mage_id:obj():desc(),
      id = mage_id,
      can_select = mage_meets,
      index = index,
    })
    index = index + 1

    local scout_id = MutationBranchId.new("RPG_SCOUT")
    local scout_meets = dex_val >= 8
    local scout_text = scout_meets and color_good("➳ [Scout]")
      or color_text("➳ [Scout]", "dark_gray") .. " - " .. format_requirement("DEX", dex_val, 8, true)
    table.insert(options, {
      text = scout_text,
      desc = scout_id:obj():desc(),
      id = scout_id,
      can_select = scout_meets,
      index = index,
    })
    index = index + 1
  end

  -- Prestige classes (show if level 10+ and have appropriate base class)
  if player:has_trait(MutationBranchId.new("RPG_WARRIOR")) then
    local berserker_id = MutationBranchId.new("RPG_BERSERKER")
    local melee_skill = player:get_skill_level(SkillId.new("melee"))
    local unarmed_skill = player:get_skill_level(SkillId.new("unarmed"))
    local berserker_meets = level >= 10 and str_val >= 12 and dex_val >= 8 and melee_skill >= 6 and unarmed_skill >= 5

    local berserker_text
    if berserker_meets then
      berserker_text = color_good("★ [Berserker]") .. color_text(" (Prestige)", "yellow")
    else
      local reqs = format_requirement("Lv", level, 10, true)
        .. ", "
        .. format_requirement("STR", str_val, 12, true)
        .. ", "
        .. format_requirement("DEX", dex_val, 8, true)
        .. ", "
        .. format_requirement("Melee", melee_skill, 6, true)
        .. ", "
        .. format_requirement("Unarmed", unarmed_skill, 5, true)
      berserker_text = color_text("★ [Berserker]", "dark_gray") .. " - " .. reqs
    end

    table.insert(options, {
      text = berserker_text,
      desc = berserker_id:obj():desc(),
      id = berserker_id,
      remove_first = MutationBranchId.new("RPG_WARRIOR"),
      can_select = berserker_meets,
      index = index,
    })
    index = index + 1

    local guardian_id = MutationBranchId.new("RPG_GUARDIAN")
    local dodge_skill = player:get_skill_level(SkillId.new("dodge"))
    local firstaid_skill = player:get_skill_level(SkillId.new("firstaid"))
    local guardian_meets = level >= 10 and str_val >= 12 and per_val >= 8 and dodge_skill >= 6 and firstaid_skill >= 4

    local guardian_text
    if guardian_meets then
      guardian_text = color_good("★ [Guardian]") .. color_text(" (Prestige)", "yellow")
    else
      local reqs = format_requirement("Lv", level, 10, true)
        .. ", "
        .. format_requirement("STR", str_val, 12, true)
        .. ", "
        .. format_requirement("PER", per_val, 8, true)
        .. ", "
        .. format_requirement("Dodge", dodge_skill, 6, true)
        .. ", "
        .. format_requirement("FirstAid", firstaid_skill, 4, true)
      guardian_text = color_text("★ [Guardian]", "dark_gray") .. " - " .. reqs
    end

    table.insert(options, {
      text = guardian_text,
      desc = guardian_id:obj():desc(),
      id = guardian_id,
      remove_first = MutationBranchId.new("RPG_WARRIOR"),
      can_select = guardian_meets,
      index = index,
    })
    index = index + 1
  end

  if player:has_trait(MutationBranchId.new("RPG_MAGE")) then
    local mystic_id = MutationBranchId.new("RPG_MYSTIC")
    local spellcraft_skill = player:get_skill_level(SkillId.new("spellcraft"))
    local mystic_meets = level >= 10 and int_val >= 12 and per_val >= 8 and spellcraft_skill >= 6

    local mystic_text
    if mystic_meets then
      mystic_text = color_good("★ [Mystic]") .. color_text(" (Prestige)", "yellow")
    else
      local reqs = format_requirement("Lv", level, 10, true)
        .. ", "
        .. format_requirement("INT", int_val, 12, true)
        .. ", "
        .. format_requirement("PER", per_val, 8, true)
        .. ", "
        .. format_requirement("Spellcraft", spellcraft_skill, 6, true)
      mystic_text = color_text("★ [Mystic]", "dark_gray") .. " - " .. reqs
    end

    table.insert(options, {
      text = mystic_text,
      desc = mystic_id:obj():desc(),
      id = mystic_id,
      remove_first = MutationBranchId.new("RPG_MAGE"),
      can_select = mystic_meets,
      index = index,
    })
    index = index + 1

    local scholar_id = MutationBranchId.new("RPG_SCHOLAR")
    local fabrication_skill = player:get_skill_level(SkillId.new("fabrication"))
    local electronics_skill = player:get_skill_level(SkillId.new("electronics"))
    local scholar_meets = level >= 10
      and int_val >= 12
      and dex_val >= 8
      and fabrication_skill >= 5
      and electronics_skill >= 4

    local scholar_text
    if scholar_meets then
      scholar_text = color_good("★ [Scholar]") .. color_text(" (Prestige)", "yellow")
    else
      local reqs = format_requirement("Lv", level, 10, true)
        .. ", "
        .. format_requirement("INT", int_val, 12, true)
        .. ", "
        .. format_requirement("DEX", dex_val, 8, true)
        .. ", "
        .. format_requirement("Fabrication", fabrication_skill, 5, true)
        .. ", "
        .. format_requirement("Electronics", electronics_skill, 4, true)
      scholar_text = color_text("★ [Scholar]", "dark_gray") .. " - " .. reqs
    end

    table.insert(options, {
      text = scholar_text,
      desc = scholar_id:obj():desc(),
      id = scholar_id,
      remove_first = MutationBranchId.new("RPG_MAGE"),
      can_select = scholar_meets,
      index = index,
    })
    index = index + 1
  end

  if player:has_trait(MutationBranchId.new("RPG_SCOUT")) then
    local ranger_id = MutationBranchId.new("RPG_RANGER")
    local archery_skill = player:get_skill_level(SkillId.new("archery"))
    local survival_skill = player:get_skill_level(SkillId.new("survival"))
    local ranger_meets = level >= 10 and dex_val >= 12 and per_val >= 10 and archery_skill >= 6 and survival_skill >= 5

    local ranger_text
    if ranger_meets then
      ranger_text = color_good("★ [Ranger]") .. color_text(" (Prestige)", "yellow")
    else
      local reqs = format_requirement("Lv", level, 10, true)
        .. ", "
        .. format_requirement("DEX", dex_val, 12, true)
        .. ", "
        .. format_requirement("PER", per_val, 10, true)
        .. ", "
        .. format_requirement("Archery", archery_skill, 6, true)
        .. ", "
        .. format_requirement("Survival", survival_skill, 5, true)
      ranger_text = color_text("★ [Ranger]", "dark_gray") .. " - " .. reqs
    end

    table.insert(options, {
      text = ranger_text,
      desc = ranger_id:obj():desc(),
      id = ranger_id,
      remove_first = MutationBranchId.new("RPG_SCOUT"),
      can_select = ranger_meets,
      index = index,
    })
    index = index + 1

    local assassin_id = MutationBranchId.new("RPG_ASSASSIN")
    local throw_skill = player:get_skill_level(SkillId.new("throw"))
    local cooking_skill = player:get_skill_level(SkillId.new("cooking"))
    local assassin_meets = level >= 10 and dex_val >= 12 and int_val >= 8 and throw_skill >= 5 and cooking_skill >= 4

    local assassin_text
    if assassin_meets then
      assassin_text = color_good("★ [Assassin]") .. color_text(" (Prestige)", "yellow")
    else
      local reqs = format_requirement("Lv", level, 10, true)
        .. ", "
        .. format_requirement("DEX", dex_val, 12, true)
        .. ", "
        .. format_requirement("INT", int_val, 8, true)
        .. ", "
        .. format_requirement("Throwing", throw_skill, 5, true)
        .. ", "
        .. format_requirement("Cooking", cooking_skill, 4, true)
      assassin_text = color_text("★ [Assassin]", "dark_gray") .. " - " .. reqs
    end

    table.insert(options, {
      text = assassin_text,
      desc = assassin_id:obj():desc(),
      id = assassin_id,
      remove_first = MutationBranchId.new("RPG_SCOUT"),
      can_select = assassin_meets,
      index = index,
    })
    index = index + 1
  end

  if has_class(player) then
    table.insert(options, {
      text = color_bad("✖ Abandon class"),
      desc = color_warning("WARNING: ")
        .. "Damages your soul for 3 days, greatly reducing all your stats and movement speed.",
      action = "abandon",
      can_select = true,
      index = index,
    })
    index = index + 1
  end

  table.insert(options, {
    text = color_text("← Back", "light_gray"),
    desc = "Return to main menu",
    action = "back",
    can_select = true,
    index = index,
  })

  for i, opt in ipairs(options) do
    ui:add_w_desc(opt.index, opt.text, opt.desc or "")
    if ui.entries and ui.entries[i] then ui.entries[i].enable = opt.can_select end
  end

  local choice_index = ui:query()

  if choice_index > 0 and choice_index <= #options then
    local chosen = options[choice_index]

    if not chosen.can_select then
      gapi.add_msg(MsgType.warning, color_warning("You don't meet the requirements for this class."))
      return
    end

    if chosen.action == "back" then
      return
    elseif chosen.action == "abandon" then
      local all_classes = {
        MutationBranchId.new("RPG_WARRIOR"),
        MutationBranchId.new("RPG_BERSERKER"),
        MutationBranchId.new("RPG_GUARDIAN"),
        MutationBranchId.new("RPG_MAGE"),
        MutationBranchId.new("RPG_MYSTIC"),
        MutationBranchId.new("RPG_SCHOLAR"),
        MutationBranchId.new("RPG_SCOUT"),
        MutationBranchId.new("RPG_RANGER"),
        MutationBranchId.new("RPG_ASSASSIN"),
      }

      for _, class_id in ipairs(all_classes) do
        if player:has_trait(class_id) then player:remove_mutation(class_id, true) end
      end

      player:add_effect(EffectTypeId.new("damaged_soul"), TimeDuration.from_hours(72))
      gapi.add_msg(
        MsgType.bad,
        color_bad("✖ You have abandoned your class!") .. " Your soul is " .. color_bad("damaged") .. " for 3 days."
      )
    elseif chosen.id then
      if chosen.remove_first then player:remove_mutation(chosen.remove_first, true) end
      player:set_mutation(chosen.id)
      local class_name = chosen.id:obj():name()
      gapi.add_msg(
        MsgType.good,
        color_good("✓ You have chosen your path as ") .. color_highlight(class_name) .. color_good("!")
      )
    end
  end
end

local function check_trait_requirements(player, trait_id)
  -- Maybe this is inefficient, but it's not run often enough for that to matter.
  local str_val = player:get_str()
  local dex_val = player:get_dex()
  local int_val = player:get_int()
  local per_val = player:get_per()

  local requirements = {
    RPG_TRAIT_BIONIC_SYMBIOTE = { stats = { INT = 10 } },
    RPG_TRAIT_VITAL_ESSENCE = { stats = { PER = 12 } },
    RPG_TRAIT_RADIOACTIVE_BLOOD = {},
    RPG_TRAIT_IRON_HIDE = { stats = { STR = 12 } },
    RPG_TRAIT_LIGHTWEIGHT = { stats = { DEX = 12 } },
    RPG_TRAIT_PACK_MULE = { stats = { STR = 14 } },
    RPG_TRAIT_TIRELESS = { stats = { STR = 10 } },
    RPG_TRAIT_MANA_FONT = { stats = { INT = 14 } },
    RPG_TRAIT_BLINK_STEP = { stats = { DEX = 14 } },
    RPG_TRAIT_NATURAL_HEALER = { stats = { PER = 10 } },
    RPG_TRAIT_ADAPTIVE_BIOLOGY = {},
    RPG_TRAIT_GLASS_CANNON = { stats = { DEX = 16 } },
    RPG_TRAIT_JUGGERNAUT = { stats = { STR = 18 } },
    RPG_TRAIT_ARCANE_BATTERY = { stats = { INT = 16 } },
    RPG_TRAIT_IRON_FISTS = { stats = { STR = 12 } },
    RPG_TRAIT_COMBAT_REFLEXES = { stats = { DEX = 12 } },
    RPG_TRAIT_ACROBAT = { stats = { DEX = 14 } },
    RPG_TRAIT_EFFICIENT_METABOLISM = {},
    RPG_TRAIT_TIRELESS_WORKER = { stats = { STR = 10 } },
    RPG_TRAIT_RAPID_METABOLISM = { stats = { STR = 12 } },
    RPG_TRAIT_SCENTLESS = { stats = { PER = 10 } },
    RPG_TRAIT_CLOTTING_FACTOR = { stats = { STR = 10 } },
    RPG_TRAIT_REGENERATOR = { stats = { STR = 14, PER = 10 } },
    RPG_TRAIT_MASTER_CRAFTSMAN = { stats = { INT = 12, DEX = 12 } },
    RPG_TRAIT_PACK_RAT = { stats = { STR = 10 } },
    RPG_TRAIT_NATURAL_BUTCHER = { stats = { DEX = 10, PER = 10 }, skills = { survival = 2 } },
  }

  local reqs = requirements[trait_id]
  if not reqs then return true, "" end

  local unmet = {}

  if reqs.stats then
    if reqs.stats.STR and str_val < reqs.stats.STR then table.insert(unmet, string.format("STR %d", reqs.stats.STR)) end
    if reqs.stats.DEX and dex_val < reqs.stats.DEX then table.insert(unmet, string.format("DEX %d", reqs.stats.DEX)) end
    if reqs.stats.INT and int_val < reqs.stats.INT then table.insert(unmet, string.format("INT %d", reqs.stats.INT)) end
    if reqs.stats.PER and per_val < reqs.stats.PER then table.insert(unmet, string.format("PER %d", reqs.stats.PER)) end
  end

  if reqs.skills then
    for skill_name, required_level in pairs(reqs.skills) do
      local skill_level = player:get_skill_level(SkillId.new(skill_name))
      if skill_level < required_level then
        local display_name = skill_name:gsub("^%l", string.upper)
        table.insert(unmet, string.format("%s %d", display_name, required_level))
      end
    end
  end

  if #unmet > 0 then return false, "Requires: " .. table.concat(unmet, ", ") end

  return true, ""
end

mod.manage_traits_menu = function(player)
  local num_traits = get_char_value(player, "rpg_num_traits", 0)
  local max_traits = get_char_value(player, "rpg_max_traits", 1)

  if num_traits >= max_traits then
    gapi.add_msg(MsgType.warning, color_warning("You have no trait slots available."))
    return
  end

  local ui = UiList.new()
  ui:title(string.format("=== Select Trait (%d/%d) ===", num_traits, max_traits))
  ui:desc_enabled(true)

  local all_traits = {
    MutationBranchId.new("RPG_TRAIT_BIONIC_SYMBIOTE"),
    MutationBranchId.new("RPG_TRAIT_VITAL_ESSENCE"),
    MutationBranchId.new("RPG_TRAIT_RADIOACTIVE_BLOOD"),
    MutationBranchId.new("RPG_TRAIT_IRON_HIDE"),
    MutationBranchId.new("RPG_TRAIT_LIGHTWEIGHT"),
    MutationBranchId.new("RPG_TRAIT_PACK_MULE"),
    MutationBranchId.new("RPG_TRAIT_TIRELESS"),
    MutationBranchId.new("RPG_TRAIT_MANA_FONT"),
    MutationBranchId.new("RPG_TRAIT_BLINK_STEP"),
    MutationBranchId.new("RPG_TRAIT_NATURAL_HEALER"),
    MutationBranchId.new("RPG_TRAIT_ADAPTIVE_BIOLOGY"),
    MutationBranchId.new("RPG_TRAIT_GLASS_CANNON"),
    MutationBranchId.new("RPG_TRAIT_JUGGERNAUT"),
    MutationBranchId.new("RPG_TRAIT_ARCANE_BATTERY"),
    MutationBranchId.new("RPG_TRAIT_IRON_FISTS"),
    MutationBranchId.new("RPG_TRAIT_EFFICIENT_METABOLISM"),
    MutationBranchId.new("RPG_TRAIT_COMBAT_REFLEXES"),
    MutationBranchId.new("RPG_TRAIT_ACROBAT"),
    MutationBranchId.new("RPG_TRAIT_TIRELESS_WORKER"),
    MutationBranchId.new("RPG_TRAIT_RAPID_METABOLISM"),
    MutationBranchId.new("RPG_TRAIT_SCENTLESS"),
    MutationBranchId.new("RPG_TRAIT_CLOTTING_FACTOR"),
    MutationBranchId.new("RPG_TRAIT_REGENERATOR"),
    MutationBranchId.new("RPG_TRAIT_MASTER_CRAFTSMAN"),
    MutationBranchId.new("RPG_TRAIT_PACK_RAT"),
    MutationBranchId.new("RPG_TRAIT_NATURAL_BUTCHER"),
  }

  local traits = {}
  for i, trait_id in ipairs(all_traits) do
    local already_has = player:has_trait(trait_id)
    local trait_obj = trait_id:obj()
    local trait_name = trait_obj:name()
    local trait_desc = trait_obj:desc()

    local meets_reqs, req_text = check_trait_requirements(player, trait_id:str())

    local display_name
    local can_select = not already_has and meets_reqs

    if already_has then
      display_name = color_text("✓ " .. trait_name, "dark_gray") .. color_text(" (Owned)", "dark_gray")
    elseif not meets_reqs then
      display_name = color_text("◆ " .. trait_name, "dark_gray") .. " - " .. color_bad(req_text)
    else
      display_name = color_good("◆ " .. trait_name)
    end

    table.insert(traits, {
      id = trait_id,
      name = display_name,
      desc = trait_desc,
      can_select = can_select,
      index = i,
    })
  end

  table.insert(traits, {
    name = color_text("← Back", "light_gray"),
    desc = "Return to main menu",
    action = "back",
    can_select = true,
    index = #all_traits + 1,
  })

  for i, trait in ipairs(traits) do
    ui:add_w_desc(trait.index, trait.name, trait.desc or "")
    if ui.entries and ui.entries[i] then ui.entries[i].enable = trait.can_select end
  end

  local choice_index = ui:query()

  if choice_index > 0 and choice_index <= #traits then
    local chosen = traits[choice_index]

    if not chosen.can_select then
      if player:has_trait(chosen.id) then
        gapi.add_msg(MsgType.warning, color_warning("You already have this trait."))
      else
        gapi.add_msg(MsgType.warning, color_warning("You don't meet the requirements for this trait."))
      end
      return
    end

    if chosen.action == "back" then
      return
    elseif chosen.id then
      player:set_mutation(chosen.id)
      set_char_value(player, "rpg_num_traits", num_traits + 1)
      local trait_name = chosen.id:obj():name()
      gapi.add_msg(MsgType.good, color_good("✓ You have gained ") .. color_highlight(trait_name) .. color_good("!"))
    end
  end
end

mod.assign_stats_menu = function(player)
  local stat_points = get_char_value(player, "rpg_stat_points", 0)

  if stat_points <= 0 then
    gapi.add_msg(MsgType.warning, color_warning("You have no stat points to assign."))
    return
  end

  local ui = UiList.new()
  ui:title(string.format("=== Assign Stat Points (%d available) ===", stat_points))
  ui:desc_enabled(true)

  local str_val = player:get_str()
  local dex_val = player:get_dex()
  local int_val = player:get_int()
  local per_val = player:get_per()

  local options = {
    {
      name = string.format("Strength (Current: %d)", str_val),
      desc = "Strength affects your melee damage, the amount of weight you can carry, your total HP, your resistance to many diseases, and the effectiveness of actions which require brute force.",
      stat = "STR",
    },
    {
      name = string.format("Dexterity (Current: %d)", dex_val),
      desc = "Dexterity affects your chance to hit in melee combat, helps you steady your gun for ranged combat, and enhances many actions that require finesse.",
      stat = "DEX",
    },
    {
      name = string.format("Intelligence (Current: %d)", int_val),
      desc = "Intelligence is less important in most situations, but it is vital for more complex tasks like electronics crafting.  It also affects how much skill you can pick up from reading a book.",
      stat = "INT",
    },
    {
      name = string.format("Perception (Current: %d)", per_val),
      desc = "Perception is the most important stat for ranged combat.  It's also used for detecting traps and other things of interest.",
      stat = "PER",
    },
    {
      name = color_text("← Back", "light_gray"),
      desc = "Return to main menu",
      stat = "BACK",
    },
  }

  for i, opt in ipairs(options) do
    ui:add_w_desc(i, opt.name, opt.desc)
  end

  local choice_index = ui:query()

  if choice_index > 0 and choice_index <= #options then
    local chosen = options[choice_index]

    if chosen.stat == "BACK" then
      return
    elseif chosen.stat then
      local key = "rpg_assigned_" .. chosen.stat:lower()
      local current = get_char_value(player, key, 0)
      set_char_value(player, key, current + 1)

      set_char_value(player, "rpg_stat_points", stat_points - 1)

      gapi.add_msg(MsgType.good, color_good("✓ +1 ") .. color_highlight(chosen.stat) .. color_good(" assigned!"))
    end
  end
end

mod.show_help_menu = function(player)
  local ui = UiList.new()
  ui:title("=== [System] Information ===")
  ui:desc_enabled(true)

  local options = {
    {
      name = "About the System",
      desc = "[System] Query: Basic information",
      action = "about",
    },
    {
      name = "Adjust Level Scaling",
      desc = "Fine-tune power scaling for balance",
      action = "scaling",
    },
    {
      name = color_text("← Back", "light_gray"),
      desc = "Return to main menu",
      action = "back",
    },
  }

  for i, opt in ipairs(options) do
    ui:add_w_desc(i, opt.name, opt.desc)
  end

  local choice_index = ui:query()

  if choice_index > 0 and choice_index <= #options then
    local chosen = options[choice_index]

    if chosen.action == "back" then
      return
    elseif chosen.action == "about" then
      mod.show_about_screen(player)
    elseif chosen.action == "scaling" then
      mod.adjust_level_scaling(player)
    end
  end
end

mod.show_about_screen = function(player)
  local ui = UiList.new()
  ui:title("=== [System] Database Entry ===")

  local help_text = ""
  help_text = help_text .. color_info("[System]") .. " Protocol IB-73758-R\n"
  help_text = help_text
    .. color_text(
      "─────────────────────────────────────────",
      "light_gray"
    )
    .. "\n\n"

  help_text = help_text .. "Welcome to the " .. color_info("[System]") .. "! You, among\n"
  help_text = help_text .. "all inhabitants of world IB-73758-R, have\n"
  help_text = help_text .. "been blessed with a chance at greatness\n"
  help_text = help_text .. "for your great deeds doing " .. color_warning("[REASON NOT FOUND]") .. ".\n"
  help_text = help_text .. "Admire below the power made available:\n\n"

  help_text = help_text .. color_highlight("PROGRESSION") .. "\n"
  help_text = help_text .. "• Level 10: Prestige Class unlocks\n"
  help_text = help_text .. "• +1 stat point per 2 levels\n"
  help_text = help_text .. "• +1 trait slot per 7 levels\n\n"

  help_text = help_text .. color_highlight("BALANCE") .. "\n"
  help_text = help_text .. "~25 point value (weak early, strong late)\n"
  help_text = help_text .. color_warning("⚠") .. " Not meant to be used with Stats through Kills\n\n"

  help_text = help_text .. color_highlight("PRESTIGE PATHS") .. "\n"
  help_text = help_text .. "Warrior → Berserker / Guardian\n"
  help_text = help_text .. "Mage → Mystic / Scholar\n"
  help_text = help_text .. "Scout → Ranger / Assassin\n"

  ui:text(help_text)
  ui:add(1, color_text("← Back", "light_gray"))

  ui:query()
end

mod.adjust_level_scaling = function(player)
  local current_scaling = get_char_value(player, "rpg_level_scaling", 100)

  local ui = UiList.new()
  ui:title("=== Adjust Level Scaling ===")
  ui:desc_enabled(true)

  local info_text = ""
  info_text = info_text .. "Current setting: " .. color_highlight(current_scaling .. "%") .. "\n"
  info_text = info_text
    .. color_text(
      "Affects class bonuses and periodic effects. Things that don't scale with level are unaffected.",
      "light_gray"
    )
    .. "\n"
  info_text = info_text .. color_text("Does not affect assigned stats.", "light_gray") .. "\n\n"

  ui:text(info_text)

  local options = {
    {
      name = "0% (Disable scaling)",
      desc = "Disables all level-based class bonuses and periodic effects.",
      value = 0,
    },
    {
      name = "50% (Half power)",
      desc = "Half effectiveness for balanced gameplay.",
      value = 50,
    },
    {
      name = "100% (Full power)",
      desc = "Default intended [System] experience.",
      value = 100,
    },
    {
      name = color_text("← Back", "light_gray"),
      desc = "Return to help menu",
      value = nil,
    },
  }

  for i, opt in ipairs(options) do
    local display_name = opt.name
    if opt.value == current_scaling then display_name = color_good("✓ " .. opt.name) end
    ui:add_w_desc(i, display_name, opt.desc)
  end

  local choice_index = ui:query()

  if choice_index > 0 and choice_index <= #options then
    local chosen = options[choice_index]

    if chosen.value ~= nil then
      set_char_value(player, "rpg_level_scaling", chosen.value)
      gapi.add_msg(MsgType.good, color_good("Level scaling set to ") .. color_highlight(chosen.value .. "%"))
    end
  end
end
