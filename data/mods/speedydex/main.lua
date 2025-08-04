gdebug.log_info("SpeedyDex: Lua loaded")

---@class ModSpeedyDex
local mod = game.mod_runtime[game.current_mod]

---CONFIGURATION
local MINIMUM_BONUS <const> = 10
local SPEED_MULTIPLIER <const> = 2

---Calculate speed bonus from dexterity value.
---@param dex number
---@return number
local function calc_speedydex_bonus(dex)
    return math.min((dex - 8) * SPEED_MULTIPLIER, MINIMUM_BONUS)
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