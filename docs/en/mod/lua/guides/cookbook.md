# Lua scripting cookbook

Here are some snippets to help you get familiar with Lua APIs and learn how to use them.
To test these examples, paste the code into the in-game Lua console by pressing the `` ` `` (backtick) key.

## Items

### Getting list of all wielded and worn items in your inventory

```lua
local you = gapi.get_avatar()
local items = you:all_items(false)

for _, item in pairs(items) do
  local status = ""
  if you:is_wielding(item) then
    status = "wielded: "
  elseif you:is_wearing(item) then
    status = "worn: "
  end
  print(status .. item:tname(1, false, 0))
end
```

<details>
<summary>Example output</summary>

```
wielded: smartphone
worn: bra
worn: panties
worn: pair of socks
worn: jeans
worn: long-sleeved shirt
worn: pair of sneakers
worn: messenger bag
worn: wrist watch
pocket knife
matchbook
clean water (plastic bottle)
clean water
```

</details>
