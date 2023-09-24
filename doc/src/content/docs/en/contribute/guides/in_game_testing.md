---
title: Debug menu
---

Whether you are implementing a new feature or whether you are fixing a bug, it is always a good
practice to test your changes in-game. It can be a hard task to create the exact conditions by
playing a normal game to be able to test your changes, which is why there is a debug menu. There is
no default key to bring up the menu so you will need to assign one first.

Bring up the keybindings menu (press `Escape` then `1`), scroll down almost to the bottom and press
`+` to add a new key binding. Press the letter that corresponds to the _Debug menu_ item, then press
the key you want to use to bring up the debug menu. To test your changes, create a new world with a
new character. Once you are in that world, press the key you just assigned for the debug menu and
you should see something like this:

```
┌─────────────────────────────────────────────────────┐
│ Debug Functions - Manipulate the fabric of reality! │
├─────────────────────────────────────────────────────┤
│ i Info                                              │
│ Q Quit to main menu                                 │
│ s Spawning...                                       │
│ p Player...                                         │
│ t Teleport...                                       │
│ m Map...                                            │
└─────────────────────────────────────────────────────┘
```

With these commands, you should be able to recreate the proper conditions to test your changes. The
[DDA wiki](http://cddawiki.chezzo.com/cdda_wiki/index.php) may have useful informations regarding
debug menu.
