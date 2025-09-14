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


## Monsters

### Spawning a dog near the player

```lua
local avatar = gapi.get_avatar()
local coords = avatar:get_pos_ms()
local dog_mtype = MtypeId.new("mon_dog_bcollie")
local doggy = gapi.place_monster_around(dog_mtype, coords, 5)
if doggy == nil then
    gdebug.log_info("Could not spawn doggy :(")
else
    gdebug.log_info(string.format("Spawned Doggy at %s", doggy:get_pos_ms()))
end
```


## Combat

### Printing details about a combat technique when it is used
First, define the function.
```lua
on_creature_performed_technique = function(params)
  local char = params.char
  local technique = params.technique
  local target = params.target
  local damage_instance = params.damage_instance
  local move_cost = params.move_cost
  gdebug.log_info(
          string.format(
                  "%s performed %s on %s (DI: %s , MC: %s)",
                  char:get_name(),
                  technique.name,
                  target:get_name(),
                  damage_instance:total_damage(),
                  move_cost
          )
  )
end
```
Then connect the hook to the function ONLY ONCE.
```lua
table.insert(game.hooks.on_creature_performed_technique, function(...) return on_creature_performed_technique(...) end)
```


<details>
<summary>Example output</summary>

```
Ramiro Waters performed Power Hit on zombie (DI: 27.96 , MC: 58)
```

</details>