local raw_ptr = test_data.raw_ptr
local str_id = test_data.str_id
local int_id = test_data.int_id

local func_raw = test_data.func_raw
local func_str_id = test_data.func_str_id
local func_int_id = test_data.func_int_id

assert(str_id:implements_int_id())

-- Functions directly accept their expected result
func_raw(raw_ptr)
func_str_id(str_id)
func_int_id(int_id)

-- Converting raw -> id
func_raw(str_id:obj())
func_raw(int_id:obj())

-- Converting id -> raw
func_int_id(raw_ptr:int_id())
func_str_id(raw_ptr:str_id())

-- Converting str_id <-> int_id
func_int_id(str_id:int_id())
func_str_id(int_id:str_id())
