local ui = {}

---@type fun(str: string): void
ui.query_any_key = function(str)
  local popup = QueryPopup.new()
  popup:message(str)
  popup:allow_any_key(true)
  popup:query()
end

---@type fun(str: string): string
ui.query_yn = function(str)
  local popup = QueryPopup.new()
  popup:message(str)
  return popup:query_yn()
end

return ui
