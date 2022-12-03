Game Objects

Many of the things that physically exist within the game world are game objects. Currently this only covers items, but creatures and vehicles are coming soon and then probably furniture at some point. GOs have a small common interface with methods like name and position that you can find in game_object.h. GOs use private constructors, this means your variables must always be references or pointers. Trying to assign a variable without a layer of indirection like that will cause a compile error. Likewise trying to copy one object over another will cause a similar error.

```
item& it_ref=...;//Good
item* it_pointer;//Good

item it=...; //Compile error
it_ref = other;//Compile error
*it_pointer = other;//Compile error
```

New GOs can be created via \_spawn methods that return a pointer to the newly created game object. 

```
item& it_ref=*item_spawn("water");
item* it_pointer=item_spawn("water");
```

When you create a new object like this it's in an unattached state. That is it doesn't exist anywhere within the game world. You must either put unattached objects somewhere or destroy them before the end of the turn. If it's a temporary object that you don't want to put in the world for whatever reason you can use `spawn_temporary` which will be destroyed for you at the end of the turn. You can remove a GO from the world by calling its `detach()` method but be aware that this will invalidate any iterators for the object's container. GO containers have methods that update both iterators and attachment correctly. 

If you'd like to add a new type of place that a game object can be within the world, you'll need to add a new location class. Look in locations.h for more info. 

Safe References

Game objects support safe references. Safe references will refuse access to an object that has been destroyed or is outside of the reality bubble without losing track of it and they can be saved and loaded. You must check them as a boolean (e.g. `if( ref )`) to see if they're valid. Trying to access them when they're not will cause debugmsgs and give you an error object instead. They have a small interface that lets you check if they're destroyed or unloaded etc. If they were destroyed or unloaded this turn, they have a method that will allow you to access the object in a const fashion, for instance to display an error message.

When choosing which reference to use the most important thing to consider is how long you want to hold onto the reference. If you're just using something temporarily, for instance most arguments and variables, you should use a normal reference or pointer. If you need to store it for longer you should use a safe reference and this means it can be easily stored in the save file. In the rare case that you do want to save it across turns but don't want to store it in the save file, which means caches, there's also a fast cache reference, which does last across turns but can't be saved. 

Game objects can sometimes be split into pieces or merged together. Item stacks are the main example of this but there are others like vehicles being split or dissoluted devourers merging. When a stack of items is split, the stack that stays in place is the one that safe references will follow. When they are merged safe references to either half of the merge will now point to the merge result. 

