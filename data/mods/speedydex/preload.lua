gdebug.log_info("SpeedyDex: Lua loaded")

---@class ModSpeedyDex
local mod = game.mod_runtime[game.current_mod]

---CONFIGURATION
---Default: for every level above 10 dexterity, the player gains 2 moves. Negative bonuses are not allowed.
local SPEEDYDEX_MIN_DEX = 10 -- The minimum dex required for bonus to kick in
local SPEEDYDEX_DEX_SPEED = 2 -- The amount of moves gained per dex above the minimum
local allow_negative_bonus = false

---Calculate speed bonus from dexterity value.
---@param dex number
---@return number
local function calc_speedydex_bonus(dex)
  -- do negative bonus if allowed
  if allow_negative_bonus and dex < SPEEDYDEX_MIN_DEX then
    return (SPEEDYDEX_MIN_DEX - dex) * SPEEDYDEX_DEX_SPEED
  else
    local modified_dex = math.max(dex - SPEEDYDEX_MIN_DEX, 0)
    return modified_dex * SPEEDYDEX_DEX_SPEED
  end
end

---Applies SpeedyDex speed bonus to the player every turn.
function mod.speedydex()
  ---@type Avatar
  local player = gapi.get_avatar()

  local dex = player:get_dex()
  local bonus = calc_speedydex_bonus(dex)

  player:mod_speed_bonus(bonus)
end

-- Register to run every game turn.
gapi.add_on_every_x_hook(TimeDuration.new().from_turns(1), mod.speedydex)
