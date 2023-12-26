-- Get global avatar instance
local av = gapi.get_avatar()

-- Get avatar name.
-- Original C++ function accepts Character reference,
-- but we don't need to care about that, inheritance works.
local av_name = gapi.get_character_name(av)

-- We output single string as test result
test_data["out"] = av_name
