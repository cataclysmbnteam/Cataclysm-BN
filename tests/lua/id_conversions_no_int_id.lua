local raw_ptr = test_data.raw_ptr
local str_id = test_data.str_id

local func_raw = test_data.func_raw
local func_str_id = test_data.func_str_id

assert(not str_id:implements_int_id())

-- Functions directly accept their expected result
func_raw(raw_ptr)
func_str_id(str_id)

-- Converting raw -> id
func_raw(str_id:obj())

-- Converting id -> raw
func_str_id(raw_ptr:str_id())
