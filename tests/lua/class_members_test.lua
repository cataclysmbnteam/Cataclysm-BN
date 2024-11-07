-- We receive Point as input
local val_in = test_data["in"]

-- We do some C++-defined operations on it
local p = val_in:abs() + Point.new(2, 3)

-- We use direct member access to turn it into a string
local val_out = "result is Point(" .. tostring(p.x) .. "," .. tostring(p.y) .. ")"

-- We output single string as test result
test_data["out"] = val_out
