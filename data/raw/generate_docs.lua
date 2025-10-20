--[[
    Receives a table and returns sorted array where each value is table of type { k=k, v=v }.

    Sort by key by default, but may also use provided sort function f(a, b)
    where a and b - tables of type {k=k, v=v}
    (the function must return whether a is less than b).
]]
--
local sorted_by = function(t, f)
  if not f then f = function(a, b) return (a.k < b.k) end end
  local sorted = {}
  for k, v in pairs(t) do
    table.insert(sorted, { k = k, v = v })
  end
  table.sort(sorted, f)
  return sorted
end

local remove_hidden_args = function(arg_list)
  local ret = {}
  for _, arg in pairs(arg_list) do
    if arg == "<this_state>" then
      -- sol::this_state is only visible to C++ side
    else
      table.insert(ret, arg)
    end
  end
  return ret
end

local slug_for = function(typename, membername)
  local typename = string.gsub(tostring(typename), "%s", "")
  if membername == nil then
    return ("#sol::%s"):format(typename)
  else
    return ("#sol::%s::%s"):format(typename, membername)
  end
end

local table_contains = function(tbl, key)
  for k, v in pairs(tbl) do
    if tostring(k):upper() == tostring(key):upper() then return true end
  end
  return false
end

--- func desc
---@param str string
---@param open string
---@param close string
---@param i integer
---@return string before, string match, string after
local extract_token = function(str, open, close, i)
  local j = i
  local depth = 0
  while true do
    local openBracket = str:find(open, j, true) or #str
    local closeBracket = str:find(close, j, true) or #str

    if openBracket == nil and closeBracket == nil then break end

    if openBracket < closeBracket then
      depth = depth + 1
      j = openBracket + 1
    else
      depth = depth - 1
      j = closeBracket + 1
      if depth == 0 then break end
    end
  end
  local left = str:sub(1, i - 1)
  local mid = str:sub(i, j - 1)
  local right = str:sub(j, #str)

  return left, mid, right
end

---comment
---@param str string
---@return table
local extract_cpp_types = function(str)
  local tbl = {}
  local tmp = str
  while true do
    local match = tmp:find("CppVal", 1, true)
    if match ~= nil then
      local before, token, after = extract_token(tmp, "<", ">", match)
      if before ~= "" then table.insert(tbl, { before, false }) end
      if token ~= "" then table.insert(tbl, { token, true }) end
      tmp = after
    else
      if tmp ~= "" then table.insert(tbl, { tmp, false }) end
      break
    end
  end
  return tbl
end

local linkify_types = function(str_)
  local dt = catadoc
  local types_table = dt["#types"]

  local tokens = extract_cpp_types(tostring(str_))

  local ret = ""

  for _, token in pairs(tokens) do
    local str, is_cppval = table.unpack(token)
    if is_cppval then
      str = string.gsub(str, "<", "&lt;")
      str = string.gsub(str, ">", "&gt;")
    else
      str = string.gsub(str, "[%a%d]+", function(k)
        if table_contains(types_table, k) then return ("[%s](#sol::%s)"):format(k, k) end
        return k
      end)
    end
    ret = ret .. str
  end

  return ret
end

local fmt_arg_list = function(arg_list)
  local ret = " "
  local arg_list = remove_hidden_args(arg_list)
  if #arg_list == 0 then return ret end
  ret = ret .. table.concat(arg_list, ", ")
  return ret .. " "
end

local fmt_one_constructor =
  function(typename, ctor) return typename .. ".new(" .. linkify_types(fmt_arg_list(ctor)) .. ")" end

local fmt_constructors = function(typename, ctors)
  if #ctors == 0 then
    return "  No constructors.\n"
  else
    local ret = ""
    for k, v in pairs(ctors) do
      ret = ret .. "* " .. fmt_one_constructor(typename, v) .. "  \n"
    end
    return ret
  end
end

local fmt_one_member = function(typename, member)
  local ret = ("#### %s {%s}\n"):format(member.name, slug_for(typename, member.name))
  if member.type == "var" then
    if member.hasval then
      ret = ret .. (" 🇨 Constant --> <code>%s</code>"):format(linkify_types(member.vartype))
      ret = ret .. " = `" .. tostring(member.varval) .. "`"
    else
      ret = ret .. (" 🇻 Variable --> <code>%s</code>"):format(linkify_types(member.vartype))
    end
    ret = ret .. "  \n"
  elseif member.type == "func" then
    for _, overload in pairs(member.overloads) do
      if typename ~= nil and overload.args[1] == typename then
        local args = { table.unpack(overload.args, 2) }
        local sigFmt = "(%s)"
        if overload.retval ~= "nil" then sigFmt = sigFmt .. " -> %s" end
        local sigStr = sigFmt:format(fmt_arg_list(args), overload.retval)
        ret = ret .. (" 🇲 Method --> <code>%s</code>  \n"):format(linkify_types(sigStr))
      else
        local args = overload.args
        local sigFmt = "(%s)"
        if overload.retval ~= "nil" then sigFmt = sigFmt .. " -> %s" end
        local sigStr = sigFmt:format(fmt_arg_list(args), overload.retval)
        ret = ret .. (" 🇫 Function --> <code>%s</code>  \n"):format(linkify_types(sigStr))
      end
    end
  else
    error("  Unknown member type " .. tostring(member.type))
  end

  if member.comment then
    for s in member.comment:gmatch("[^\r\n]+") do
      ret = ret .. " > " .. s .. "  \n"
    end
  end

  return ret
end

local fmt_members = function(typename, members)
  if #members == 0 then
    return "  No members.\n"
  else
    local ret = ""

    local ss = function(a, b)
      local aName = a.v.name:upper()
      local bName = b.v.name:upper()

      if aName:find("^__") and not bName:find("^__") then return false end
      if not aName:find("^__") and bName:find("^__") then return true end

      return aName < bName
    end

    local members_sorted = sorted_by(members, ss)

    -- Hide operators and serialization methods
    local is_hidden = function(member)
      if member.comment and member.comment:find("DEPRECATED") then return true end
      if member.name:find("^__") then return true end
      if member.name == "serialize" then return true end
      if member.name == "deserialize" then return true end
      return false
    end

    for _, it in pairs(members_sorted) do
      if not is_hidden(it.v) then ret = ret .. fmt_one_member(typename, it.v) .. "\n" end
    end
    return ret
  end
end

local fmt_bases = function(typename, bases)
  if #bases == 0 then
    return "  No base classes.\n"
  else
    local ret = ""
    for k, v in pairs(bases) do
      ret = ret .. "- `" .. v .. "`\n"
    end
    return ret
  end
end

local fmt_enum_entries = function(typename, entries)
  if next(entries) == nil then
    return "  No entries.\n"
  else
    local ret = ""

    local entries_filtered = {}
    for k, v in pairs(entries) do
      -- TODO: this should not be needed
      if type(v) ~= "table" and type(v) ~= "function" then entries_filtered[k] = v end
    end

    local entries_sorted = sorted_by(entries_filtered, function(a, b) return a.v < b.v end)
    for _, it in pairs(entries_sorted) do
      ret = ret .. "- `" .. tostring(it.k) .. "` = `" .. tostring(it.v) .. "`\n"
    end
    return ret
  end
end

doc_gen_func.impl = function()
  local ret = [[---
edit: false
---

# Lua API reference

> [!NOTE]
>
> This page is auto-generated from [`data/raw/generate_docs.lua`][generate_docs]
and should not be edited directly.

> [!WARNING]
>
> In Lua, functions can be called with a `:` and pass the object itself as the first argument, eg:
>
> Members where this behaviour is intended to be used are marked as 🇲 Methods<br/>
> Their signature documentation hides the first argument to reflect that
>
> * Call 🇫 Function members with a `.`
> * Call 🇲 Method members with a `:`
>
> Alternatively, you can still call 🇲 Methods with a `.`, from the class type or the variable itself
> but a value of the given type must be passed as the first parameter (that is hidden)
>
> All of these do the same thing:
> * ```
>   print(Angle.from_radians(3):to_degrees(a))
>   ```
> * ```
>   print(Angle.to_degrees(Angle.from_radians(3)))
>   ```
> * ```
>   local a = Angle.from_radians(3)
>   print(a:to_degrees())
>   ```
> * ```
>   local a = Angle.from_radians(3)
>   print(a.to_degrees(a))
>   ```
> * ```
>   local a = Angle.from_radians(3)
>   print(Angle.to_degrees(a))
>   ```

[generate_docs]: https://github.com/cataclysmbnteam/Cataclysm-BN/blob/main/data/raw/generate_docs.lua
]]

  local dt = catadoc

  local types_table = dt["#types"]

  local types_sorted = sorted_by(types_table)
  for _, it in pairs(types_sorted) do
    local typename = it.k
    local dt_type = it.v
    local type_comment = dt_type.type_comment
    ret = ret .. ("## %s {%s}\n"):format(typename, slug_for(typename))

    if type_comment then ret = ret .. type_comment .. "\n" end

    local bases = dt_type["#bases"]
    local ctors = dt_type["#construct"]
    local members = dt_type["#member"]

    ret = ret
      .. ("### Bases {#sol::%s::@bases}\n"):format(typename)
      .. fmt_bases(typename, bases)
      .. "\n"
      .. ("### Constructors {#sol::%s::@ctors}\n"):format(typename)
      .. fmt_constructors(typename, ctors)
      .. "\n"
      .. ("### Members {#sol::%s::@members}\n"):format(typename)
      .. fmt_members(typename, members)
      .. "\n"
  end

  ret = ret .. "# Enums\n\n"

  local enums_table = dt["#enums"]

  local enums_sorted = sorted_by(enums_table)
  for _, it in pairs(enums_sorted) do
    local typename = it.k
    local dt_type = it.v
    ret = ret .. "## " .. typename .. "\n"

    local entries = dt_type["entries"]

    ret = ret .. "### Entries\n" .. fmt_enum_entries(typename, entries) .. "\n"
  end

  ret = ret .. "# Libraries\n\n"

  local libs_table = dt["#libs"]

  local libs_sorted = sorted_by(libs_table)
  for _, it in pairs(libs_sorted) do
    local typename = it.k
    local dt_lib = it.v
    local lib_comment = dt_lib.lib_comment
    ret = ret .. "## " .. typename .. "\n"

    if lib_comment then ret = ret .. lib_comment .. "\n" end

    local members = dt_lib["#member"]

    ret = ret .. "### Members\n" .. fmt_members(nil, members) .. "\n"
  end

  return ret
end
