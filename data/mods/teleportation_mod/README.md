# Teleportation Tech Mod

## Quick guide

- Find "Teleporter network schematics", a book that spawns where schematics spawn (labs, bank
  vaults)
- Build a teleporter station. Use the item to place it - upon succesful placement, the station is
  registered in the teleporter network. Placement of this device is irrelevant if you don't intend
  to actively charge it. Otherwise place it in your base, and you can manually charge it with your
  electrical grid power to supplement the passive trickle charge.
- Build a teleporter remote. This tool takes batteries, and you use it to interact with the
  stations. You can use it to check the station power, manually charge the stations, and teleport.
- Build a teleporter anchor. Use the item to place it - upon succesful placement, the anchor is
  registered in the teleporter network. The anchor is the 'destination' you can teleport to.
  Teleportation is slightly inaccurate, but you will land on the same overmap square. This is
  intentional, so I suggest not placing it in tight underground locations (a mostly clear overmap
  lab tile is fine, a single tile hole in the ground very likely won't be).
- Deconstruction of anchors and stations is also possible, which removes them from the map, but does
  not yet return items. Debug them in if you want for now (working on this one).

## Items

- personal teleporter remote
- teleporter base (deployed, undeployed)
- teleporter target anchor (deployed, undeployed)

## Logic/design

- on use: teleporter anchor registers its tripoint in the teleporter network
- on use: teleporter station registers itself in the teleporter network (position, time of
  placement, and charge)
- teleporter network: array of stations with charge, timestamp and tripoint position, and anchor
  with tripoint position
- on use: teleporter remote:
  - updates charge on stations (if a minute has passed since the last use)

  - opens window with two options - teleport, and charge station
    - teleport gives a list of anchors with the distance to them, 1 distance = 1 charge
    - pick an anchor, pick station to use, player is teleported to anchor OMT, distance gets removed
      from station charge

  - charge station works if you are on the same grid with the station, it allows players to manually
    dump grid energy into the teleporter station -on use dumps energy into the station, removes it
    from grid

- teleporter stations also charge hourly, depending on the charge multiplier variable (easy edit by
  user at the top of the main.lua file)
- the amount of kj needed to get 1 unit of power is also a variable that's easily editable

- can build more stations for a larger "bank", though each teleporter has their own energy bank.
  These are currently unlimited, should not ever hit the ?double/float? limit either way.
- stations and anchors are removable, but debug is needed to spawn the item back. Quick fix once the
  item spawn function is exposed to LUA.

## TODO Feature frenzy (not done)

- give back the item upon deconstruction
- can draw vehicle power to charge stations(though you can probably connect vehicle to grid and do
  it that way?)
- teleporter stations and anchors as parts of vehicles. Would need hooks for something like
  on_build_veh_part or whatever.
