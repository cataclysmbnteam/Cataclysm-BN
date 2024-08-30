local NIL_STRING = tostring(nil)

-- Get global avatar instance
local avatar = gapi.get_avatar()

-- Get avatar name
local avatar_name = avatar.name
local avatar_pos = avatar:get_pos_ms()

-- Run some global functions
local creature_avatar = gapi.get_creature_at(avatar_pos)
local creature_avatar_name = creature_avatar and creature_avatar:get_name() or NIL_STRING

local monster_avatar = gapi.get_monster_at(avatar_pos) -- Should be nil
local monster_avatar_name = monster_avatar and monster_avatar.name or NIL_STRING

local character_avatar = gapi.get_character_at(avatar_pos)
local character_avatar_name = character_avatar and character_avatar.name or NIL_STRING

local npc_avatar = gapi.get_npc_at(avatar_pos) -- Should be nil
local npc_avatar_name = npc_avatar and npc_avatar.name or NIL_STRING

-- Write to return table
test_data["avatar_name"] = avatar_name
test_data["creature_avatar_name"] = creature_avatar_name
test_data["monster_avatar_name"] = monster_avatar_name
test_data["character_avatar_name"] = character_avatar_name
test_data["npc_avatar_name"] = npc_avatar_name
