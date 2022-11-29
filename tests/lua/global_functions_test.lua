
-- Get global avatar instance
local av = get_avatar()

-- Get avatar name.
-- Original C++ function accepts Character reference,
-- but we don't need to care about that, inheritance works.
local av_name = get_character_name( av )

-- We output single string as test result
test_data["out"] = av_name
