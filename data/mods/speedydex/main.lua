gdebug.log_info("SpeedyDex: Lua loaded")

---@class ModSpeedyDex
local mod = game.mod_runtime[game.current_mod]

---CONFIGURATION
local SPEEDYDEX_MIN_DEX = 0    -- The minimum dex required for bonus to kick in
local SPEEDYDEX_DEX_SPEED = 0   -- The amount of moves gained per dex above the minimum

---Calculate speed bonus from dexterity value.
---@param dex number
---@return number
local function calc_speedydex_bonus(dex)
    local modified_dex = math.max(dex - SPEEDYDEX_MIN_DEX, 0)
    return modified_dex * SPEEDYDEX_DEX_SPEED
end

---Applies SpeedyDex speed bonus to the player.
function mod.speedydex()
    ---@type Avatar
    local player = gapi.get_avatar()

    local dex = player:get_dex()
    local bonus = calc_speedydex_bonus(dex)

    player:mod_speed_bonus(bonus)
end

-- Register to run every game turn
gapi.add_on_every_x_hook(TimeDuration.new():from_turns(1), mod.speedydex)