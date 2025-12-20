gdebug.log_info("skyisland_lua_bindings_test: preload")

local mod = game.mod_runtime[game.current_mod]

-- Register item use functions (implementations in main.lua)
game.iuse_functions["test_item_storage"] = function(...) return mod.test_item_storage(...) end
game.iuse_functions["test_namespaced_storage"] = function(...) return mod.test_namespaced_storage(...) end
game.iuse_functions["test_item_teleport"] = function(...) return mod.test_item_teleport(...) end
game.iuse_functions["test_drop_all_items"] = function(...) return mod.test_drop_all_items(...) end
game.iuse_functions["test_overmap_search"] = function(...) return mod.test_overmap_search(...) end
