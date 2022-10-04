#cs ----------------------------------------------------------------------------

 AutoIt Version: 3.3.14.5
 Author:         leoCottret

 Script Function:
	Testing the balance of martial arts in Cataclysm Bright Night

#ce ----------------------------------------------------------------------------

#include <Date.au3>
#include <EditConstants.au3>
#include <GUIConstantsEx.au3>
#include <StaticConstants.au3>
#include <WindowsConstants.au3>
#include <Misc.au3>
#include <Array.au3>
#include <String.au3>
#include <File.au3>
#include <Json.au3>


; INSTRUCTIONS
; This tool will help you test the effectiveness of a martial art (mostly DPS) in a specific context (Cf options below)
; This tool will send keys inside the game at a fast pace to stimulate a fight between the player and a monster
; It does require a particular set up to work properly (everything must be predictable), so please follow those instructions carefully
; 1) Download autoit https://www.autoitscript.com/site/autoit/downloads/
; 2) Download a "library" for autoit, from the offical website https://www.autoitscript.com/forum/applications/core/interface/file/attachment.php?id=69571
; Extract the zip somewhere and move the 2 files BinaryCall.au3 and Json.au3 into your AutoIt Include directory (eg "C:\Program Files (x86)\AutoIt3\Include")
; 3) Create a world with size of cities 1(or low), city spacing 8, all spawn scaling to 0 (important!), eternal season true, no npc of any kind
; 4) For each monsters that will be used in the tests, you need to set their health to 99999. Find them in JSON files then change their hp field to this value.
; You might want to change some special attacks too (eg to no being smashed against a wall during the entire test)
; 5) Set your keys and your options in the script
; Eg for Tiger Kung Fu, you want to keep hitting fast, so remove the cSend($KEY_WAIT) line. For a martial art that needs a onWait/Dodge/Block buff that is removed on hit, you'll prefer to wait one turn before hitting, so leave it as it is (etc.)
; 6) Open your world
; 7) Find a nice open space in the wild
; 8) Disable autosave and safe mode (in options -> General)
; 9) Learn all martial arts with Debug Key -> p -> l
; 10) Then start the script, switch to your game and don't press a key or move the mouse during the tests (you'll see the tests begin after about 20 seconds).
; After the tests you can throw the items on the ground through the lava with "/" -> 5 -> Tab -> 8 -> Hold "Enter" on the items
;
; The tests can be quite long, so your best bet is to start the tests and let them run while doing something else
; You can count about 5-6 minutes for 1 weapon, 80 iterations and 3 fight cycles
; So if you test 2 Martial Arts with 20 weapons each, with 2 stats/skills couples, against 2 monsters you can count 2 * 20 * 2 * 2 * 6 = 960 minutes = 16 hours (you've been warned)
; If you don't want to "block" your computer for a (very) long time, your best bet is to use the $WEAPONS_ID_RESTRICTION variable to only test a few compatible weapons of your choice

; IMPORTANT: If there's a problem during the script, or you just need to stop it for some reason, hold the Escape key.

; 11) (OPTIONAL) To add content from a mod you must just be careful of 2 things
; - 1 - Add the martial art file(s) in the right order, which means every vanilla files first, then the mod(s) file(s) in alphabetical order (folder name is prioritized, then file name)
; Eg, if I want to add the MA file of Aftershock, I'll add -> , $MA_F&"..\mods\Aftershock\martialarts.json" to the $MA_FILES variable
; - 2 - Add all the weapon files for the weapon used by the martial arts. The script will throw errors when it doesn't find some weapon data, so you can add them little by little.
; Eg, if I then want to add the corresponding weapons (and given we only need one file here), I'll add -> , $ITEM_F&"..\..\mods\Aftershock\items\weapons.json" to the $WEAPON_FILES variable
; Also it's better to add them after vanilla files

; INSTRUCTIONS EXPLANATIONS (OPTIONAL)
; 2) This script needs it to work with JSON files
; 7)
; The script will build:
; - A resin wall to stop knockback effects from monster or player
; - A resin roof to protect you from the rain
; - Lava in front of you to destroy the monster items. After every ~12 iterations, it will remove the lava and make you wait 5 minutes to reset your body temperature (lava only warm you in CBN, no direct damage)
; 8)
; The autosave or the safe mode will prevent keys executions, and the script won't detect it



; The "You should probably not touch" section. You should still give it a look though.
Global $ARMOR_STACK_LEFT = 0; Will store the number of armor stack left
; Armor piece names. Enter the EXACT same text, as it's written in the debug spawn list. You need a bit of storage for special weapons eg monomolecular blade. And you need some armors to avoid bleeding effects and such messing with results
; Also avoid too much piece of armors and/or long names, this is one of the slowest part of this script
Const $ARMOR_NAMES = [ "ANBC suit (poor fit)", "survivor mask (poor fit)", "backpack" ]; Edit: for now DO NOT TOUCH, this will mess up the tests
Const $ARMOR_STACK_NUMBER = 20; The number of armor stacks you want to spawn. The sum of the armor times this number must not go above 1000L (one tile space). If any doubt do not touch, and for now do not touch
Global $COMPLETE_GAME_WINDOW_NAME = ""; Once we get this, we can try to refocus the window on the game
Global $CURRENT_TURN = 0; Store the last time where we got the damages done to a monster
Global $FIRST_LINE_SAVE_FILE = ""; The first line in the save file. Is filled in the init function
Global $FIRST_CYCLE_EXPLANATIONS = ["Wait then hit", "Hit only"]; Will just display the fight cycle method used in the tests in the result file
Const $FIGHT_CYCLES = 3; Number of fight cycles. Impact the precison of the result, the time, and the impact on pain, stamina etc. on tests
Const $GAME_WINDOW_NAME = "Cataclysm: Bright Nights -"; This text must be in the game window title (up left). This is how we get the $COMPLETE_GAME_WINDOW_NAME value. Should not be touched
Global $hDLL = DllOpen("user32.dll"); Load ddl for _isPressed function
Const $ITERATIONS = 80; Number of fight cycle repeat. Impact the precision of the result, and the time. To get a precise enough result, try to keep $FIGHT_CYCLES * $ITERATIONS >= 240. If any doubt do not touch
; LOADED FILES CONTENT
Global $LOADED_MARTIAL_ARTS[0]; Store the martial art files content, as JSON
Global $LOADED_WEAPONS[0]; Store the weapon files content, as JSON
Const $MAX_MONSTERS_HEALTH = 99999; "IMPORTANT: To speed up the tests, every tested monsters must have 99999 health. If for some reasons you set an other value in JSON files (eg you wanted to test the DPS with a 50 STR character), change it here too
; MA DATA MAIN
Global $MA_DATA_MAIN[0]; The most important variable, will store the data of the martial arts and compatible weapons as shown below, so we can then get those informations much faster during the tests
#cs ------
[
	[
		"ma_id_1",
		"ma_name_1",
		[
			["weapon_id_1", "weapon_name_1", "weapon_attackcost_1"],
			["weapon_id_2", "weapon_name_2", "weapon_attackcost_2"],
			[etc.=
		]
	],
	[etc.]
]
#ce ------

; OPTIONS (avoid modifying variables above)
; System options (should probably be left as default, except if your computer is slow)
Const $DELAY_KEYS = 50; Delay between each sent key, in milliseconds. Should not be below 50, can be increase to debug (or if your computer is too slow and the keys are sent too fast)
Const $QUICKSAVE_TIMER = 1500; How long a quicksave last on your computer, in milliseconds. Can't be detected easily. 1700 should be more than enough, depending on computer speed

; Tests options (you can change most of them)
Const $BUILD_STRUCTURES = 1; If set to 1, will build the resin walls and roof at the start. Can be set to 0 after to speed up the start (a bit)
Const $FIGHT_CYCLES_METHOD = 0; 0 = wait then hit, 1 = hit only. Could improved in the future
Const $MONSTER_NAMES = [ "Kevlar hulk", "zombie hulk" ]; The names of the monsters to test the martial art(s) against. Case sensitive
Const $MONSTER_IDS = [ "mon_zombie_kevlar_2", "mon_zombie_hulk" ]; Must match monster name (same as player stats/skills)
Const $PLAYER_STATS = [ 12, 18 ]; Player Stats
Const $PLAYER_SKILLS = [ 5, 9 ]; Player Skills. Both variable will be tested together (eg Player with 12 stats and 5 skills, then 15-7, etc.)
Const $GAME_FILES_PATH = "C:\Games\CDDA_MODDING_BN\cdda\"; "PATH_TO_GAME\cdda\"
Const $MA_F = $GAME_FILES_PATH&"data\json\"; Path to json, Should not be touched
; Martial arts files or folders. Keep this order: from left to right, vanilla files, mod files in alphabetical order (folder order is prioritized, then file names)
Const $MA_FILES = [ $MA_F&"martialarts.json", $MA_F&"martialarts_fictional.json"]
Const $MA_IDS = [ "style_barbaran" ]; the martial arts to test!
Const $MONSTER_FILES = [ $MA_F&"monsters\"]
Const $ITEM_F = $GAME_FILES_PATH&"data\json\items\"; Items folder. Should not be touched
Const $SAVE_FILE_PATH = $GAME_FILES_PATH&"save\TestMA\#VEVTVA==.sav"; "save\WORLD_NAME\#BASE64_ENCODED_CHARACTER_NAME.sav" The path to your character save file
; If set to True, the script will try to load the "$STORED_MA_DATA_MAIN_PATH" file created during a precedent test (speed up start from about 23 seconds to 3)
; But, if you add/remove/change files from the script or the game, you need to delete the file so that the script can recreate an updated version
Const $SPEED_UP_TESTS_START = False
Const $TESTS_RESULT_PATH = "C:\Users\"&@UserName&"\Desktop\results_tests_martial_arts.md"; Results will be stored in a markdown file here. You might want to replace the username variable if you have a UAC prompt
; Will contain all the important data used in the test, so the script don't have to extract them each time
; This will contain a "computed" version of the important data this script uses. If you modified the files or updated the game, delete the file so it can be recreated
Const $STORED_MA_DATA_MAIN_PATH = "C:\Users\"&@UserName&"\Desktop\results_tests_important_ma_data.json"
Const $UNARMED_WEAPONS_ID = [ "bagh_nakha", "bio_blade_weapon", "knuckle_katar", "punch_dagger" ]; Styles with 0 compatible weapons. If you want to test unarmed weapons for those style, change this
; If you want to test weapons from mod, add the folder/file here. Add them in load order, (same as for martial arts)
Const $WEAPON_FILES = [ $ITEM_F&"melee\", $ITEM_F&"gun\", $ITEM_F&"ranged\", $ITEM_F&"tool\", $ITEM_F&"resources\", $ITEM_F&"classes\", $ITEM_F&"generic.json" ]
; If you want to only test some weapons. They need to be compatible with the MA.
Global $WEAPONS_ID_RESTRICTION[0]; To disable this feature, add "[0];" (it is by default). To use it, remove the "[0]" and add " = [ "weapon_id_1", "weapon_id_2" etc. ]"

; KEYS
; Press F1 (here, not in game) -> Type "Send", -> Enter, then scroll to see the different keys. Most are just the key between quotes, but some needs a special code (eg default $KEY_UP)
Const $KEY_ADVANCED_INV_MNG = "/"
Const $KEY_DEBUG = "²"; Is not set by default!
Const $KEY_DROP_ADJACENT = "D"
Const $KEY_DOWN = "{NUMPAD2}"
Const $KEY_FILTER = "/"
Const $KEY_MAP_UP = "<"; Go up in map editor
Const $KEY_MAP_DOWN = ">"; Go down in map editor
Const $KEY_SELECT_MA = "_"; Select Martial Art
Const $KEY_UP = "{NUMPAD8}"
Const $KEY_LEFT = "{NUMPAD4}"
Const $KEY_RIGHT = "{NUMPAD6}"
Const $KEY_WAIT = "{NUMPAD5}"
Const $KEY_WAIT_MENU = "|"
Const $KEY_WEAR = "W"
Const $KEY_WIELD = "w"









; HELPER FUNCTIONS (mostly unrelated to CBN)
; Just add a delay on keys to avoid sending them too fast
Func cSend($keysToSend)
	; Hold escape if something goes wrong! Even out of the game
	If _IsPressed("1B", $hDLL) Then
		Exit
	EndIf

	; Check if the game window is focused before sending a key. Still probably not 100% reliable, but should help continue tests if an other app pop up during them
	If not (StringInStr(WinGetTitle("[ACTIVE]"), $GAME_WINDOW_NAME)) Then
		MsgBox(1, "Tests paused!", "The script will try to switch back to the game. DO NOT PRESS ANYTHING! Or hold escape to quit", 7)
		cWaitForActiveWindow($GAME_WINDOW_NAME)
	EndIf

	Sleep($DELAY_KEYS)
	Send($keysToSend)
EndFunc

; For text fields, we always want to send enter at the end to confirm
Func cSendTextField($text)
	cSend($text)
	cSend("{ENTER}")
EndFunc

; Check to see if the game window is the focused one, and try to focus on it if possible.
Func cWaitForActiveWindow($windowsName)
	; If we didn't get the full window name yet, wait for the game window to be activated
	If $COMPLETE_GAME_WINDOW_NAME = "" Then
		While not (StringInStr(WinGetTitle("[ACTIVE]"), $windowsName))
			Sleep(1000)
		WEnd
		; We got it, we can then use it for later
		$COMPLETE_GAME_WINDOW_NAME = WinGetTitle("[ACTIVE]")
	Else
		; Otherwise just reactivate the window
		WinActivate($COMPLETE_GAME_WINDOW_NAME)
	EndIf
	Sleep(3000)
EndFunc

; Load a file in a variable, as json
Func loadFileFromPath($path, ByRef $loadedContentVariable)
	$fileOpener = FileOpen($path, 0)
	If $fileOpener = -1 Then
        MsgBox($MB_SYSTEMMODAL, "", "An error occurred when reading/writing the file:"&$path)
        Exit
    EndIf
	; Decode file json content
	$fileContentJson = Json_Decode(FileRead($fileOpener))
	FileClose($fileOpener)
	Dim $loadedContentRow = [$path, $fileContentJson]
	ReDim $loadedContentVariable[UBound($loadedContentVariable)+1]
	$loadedContentVariable[UBound($loadedContentVariable)-1] = $loadedContentRow
EndFunc


; UI FUNCTIONS THAT DOES NOT END OUT OF A MENU (they usually open or close it)
; Eg open player menu
Func openSubMenu($menu_letter)
	cSend($KEY_DEBUG)
	cSend($menu_letter)
EndFunc

; Send Escape key "$level" times, to quit a menu until all are closed
Func closeMenuLevel($level)
	For $i = 0 To $level-1 Step 1
		cSend("{ESC}")
	Next
EndFunc

; UI FUNCTIONS THAT DOES END OUT OF A MENU
; Set player stats
Func setStats($playerStats)
	openSubMenu("p")
	; Set Stats
	cSend("t")
	cSend("S")
	cSendTextField($playerStats)
	cSend("t")
	cSend("D")
	cSendTextField($playerStats)
	cSend("t")
	cSend("I")
	cSendTextField($playerStats)
	cSend("t")
	cSend("P")
	cSendTextField($playerStats)
	closeMenuLevel(2)
EndFunc

; Set player skills. Has been tested with skills level from 0 to 17
Func setSkills($playerSkillsLevel)
	openSubMenu("p")
	; Set Skills at 0
	cSend("s")
	cSend("1")
	cSend("5")
	Local $curSkillsLevel = 0
	While $curSkillsLevel < $playerSkillsLevel
		; If desired level >= 10, set it to 10
		If $playerSkillsLevel >= 10 and $curSkillsLevel < 10 Then
			cSend("1")
			cSend("7")
			$curSkillsLevel = 10
		EndIf
		; If desired level >= 5, set it to 5
		If $playerSkillsLevel >= 5 and $curSkillsLevel < 5 Then
			cSend("1")
			cSend("6")
			$curSkillsLevel = 5
		EndIf
		; If we're at least 3 level from the desired level, add 3
		If $playerSkillsLevel-$curSkillsLevel >= 3 Then
			cSend("1")
			cSend("1")
			$curSkillsLevel += 3
		EndIf
		; If we're at least 1 level from the desired level, add 1
		If $playerSkillsLevel-$curSkillsLevel >= 1 And $playerSkillsLevel-$curSkillsLevel < 3  Then
			cSend("1")
			cSend("2")
			$curSkillsLevel += 1
		EndIf
	WEnd
	closeMenuLevel(3)
EndFunc

; Light reset (each time we kill a monster after a few hits)
Func resetPlayer()
	openSubMenu("p")
	; Reset hit points
	cSend("h")
	cSend("e")
	cSendTextField("999")
	; Reset pain
	cSend("P")
	cSendTextField("-1000")
	; Reset stamina
	cSend("S")
	cSendTextField("10000")

	closeMenuLevel(2)
EndFunc

; Quicksave the game
Func quickSave()
	cSend("{ESC}")
	cSend("{NUMPAD9}")
	Sleep($QUICKSAVE_TIMER)
EndFunc

; Delete everything on the player
Func deleteInventory()
	openSubMenu("p")
	cSend("d")
	cSend("Y")
	closeMenuLevel(2)
EndFunc

; Spawn and equip player with weapon
Func setWeapon($weaponName)
	openSubMenu("s")
	cSend("w")
	cSend("/")
	cSendTextField('"' & $weaponName & '"'); Needs my modification to allow exact match request with "text". And yes in AutoIt strings are concatenated with &
	Sleep(StringLen($weaponName) * 40)
	cSend("{ENTER}")
	Sleep(50)
	cSend("{ENTER}")
	Sleep(50)
	closeMenuLevel(1)
	cSend($KEY_WIELD)
	cSend($KEY_FILTER)
	cSendTextField($weaponName); Needs my modification to allow exact match request with "text". And yes in AutoIt strings are concatenated with &
	Sleep(StringLen($weaponName) * 40)
	cSend("{ENTER}")
EndFunc

; Spawn and equip player with armor. For now, armor is the same for the entire test
Func setArmor()
	For $armorName In $ARMOR_NAMES
		openSubMenu("s")
		cSend("w")
		cSend("/")
		cSendTextField('"' & $armorName & '"'); Needs my modification to allow exact match request with "text"
		Sleep(StringLen($armorName) * 40)
		cSend("{ENTER}")
		Sleep(50)
		cSend("{ENTER}")
		Sleep(50)
		closeMenuLevel(1)
		cSend($KEY_WEAR)
		cSend("{ENTER}")
	Next
EndFunc

; Spawn stacks of armors set on the ground, to then just have to equip a set for each fullPlayerReset()
Func spawnArmorStacks()
	For $armorName In $ARMOR_NAMES
		openSubMenu("s")
		cSend("w")
		cSend("/")
		cSendTextField('"' & $armorName & '"'); Needs my modification to allow exact match request with "text"
		Sleep(StringLen($armorName) * 40)
		cSend("{ENTER}")
		Sleep(50)
		cSend("{BACKSPACE}")
		Sleep(50)
		cSend(String($ARMOR_STACK_NUMBER))
		cSend("{ENTER}")
		Sleep(50)
		closeMenuLevel(1)
	Next
	$ARMOR_STACK_LEFT = $ARMOR_STACK_NUMBER
EndFunc


Func equipArmor()
	If $ARMOR_STACK_LEFT > 1 Then
		; Ultra hacky, but for now we get them in this order to avoid ANBC changing place randomly because you can't equip 2 set of them
		For $i = UBound($ARMOR_NAMES-1) to 0 Step -1
			cSend($KEY_WEAR)
			cSend(String($i))
		Next
	Else
		For $i = 0 to UBound($ARMOR_NAMES-1) Step 1
			cSend($KEY_WEAR)
			cSend("0")
		Next
	EndIf
	$ARMOR_STACK_LEFT -= 1
EndFunc

; Full reset (player comes out fresh as new)
Func resetPlayerFull($weaponName, $weaponId, $playerSkillsLevel)
	deleteInventory()
	resetPlayerWarmth(); delete lava
	resetPlayer()
	If $ARMOR_STACK_LEFT = 0 Then
		spawnArmorStacks()
	EndIf
	equipArmor()
	setWeapon($weaponName)
	manageWieldedWeapon($weaponId)
	openSubMenu("p")
	; Reset needs
	cSend("n")
	cSend("a")
	closeMenuLevel(2)
	; reset skills (they level up from time to time)
	setSkills($playerSkillsLevel)
	; Spawn lava up, in last
	placeLava()
EndFunc

; Place lava up so that droped items from monsters (including their corpses) are instantly deleted
Func placeLava()
	openSubMenu("m")
	cSend("M")
	cSend($KEY_UP)
	cSend("e")
	cSend($KEY_FILTER)
	cSendTextField("lava")
	cSend("{ENTER}")
	cSend("{ENTER}")
	closeMenuLevel(2)
EndFunc

; Save, get total damage to monster from player save, then kill and respawn monster
Func getMonsterGroupeIterationDamageAndKillThem($monsterIndex)

	; Save
	quickSave()

	$fileOpener = FileOpen($SAVE_FILE_PATH, 0)
	$groupeIterationDamage = 0
	; Remove first line of save file to get only the JSON part, then decode it. PS: go see Json_Test.au3 file to see how json functions are used
	$fileContentJson = Json_Decode(StringTrimLeft(FileRead($fileOpener), StringLen($FIRST_LINE_SAVE_FILE)))
	FileClose($fileOpener)
	If Json_IsObject($fileContentJson) Then
		If Json_ObjExists($fileContentJson, 'player_messages.messages') Then
			; Got through messages logs
			For $i = 0 to 255 Step 1
				$turn = Json_Get($fileContentJson, '["player_messages"]["messages"][' & $i & ']["turn"]')
				; Only the messages since the beginning of the iteration are relevent
				If $turn > $CURRENT_TURN Then
					$message = Json_Get($fileContentJson, '["player_messages"]["messages"][' & $i & ']["message"]')
					; Message must begin by "You ", contains the monster name and "damage." but not " no damage." to be a valid damage monster message.
					If StringInStr($message, "You ") = 1 And StringInStr($message, $MONSTER_NAMES[$monsterIndex]) <> 0 And StringInStr($message, " damage.") <> 0 And StringInStr($message, " no damage.") = 0 Then
						; Extract damage number from message
						$groupeIterationDamage += _StringBetween($message, " for ", " damage.")[0]
						; DEBUG MsgBox(1,$message,$totalIterationDamage & " " &  _StringBetween($message, " for ", " damage.")[0])
					EndIf
				EndIf
			Next
			; Update current turn
			$CURRENT_TURN = Json_Get($fileContentJson, '["turn"]')
		Else
			MsgBox(1, "Error", "Could not get player_messages.messages object (message log content). The script needs to stop. Please report this.")
		EndIf
	Else
		MsgBox(1, "Error", "$fileContentJson is not a JSON object, something went wrong. The script needs to stop. Please report this.")
	EndIf
	killAllMonsters()
	return $groupeIterationDamage
EndFunc



; Spawn a monster above the player
Func spawnMonster($monsterName)
	openSubMenu("s")
	cSend("m")
	cSend($KEY_FILTER)
	cSendTextField($monsterName)
	Sleep(StringLen($monsterName) * 40)
	cSend("{Enter}")
	Sleep(50)
	cSend($KEY_UP)
	Sleep(50)
	cSend("{Enter}")
	closeMenuLevel(1)
EndFunc

; Spawn dirt and wait 15 minutes to reset warmth completly, then spawn lava again
Func resetPlayerWarmth()
	; Spawn dirt
	openSubMenu("m")
	cSend("M")
	cSend($KEY_UP)
	cSend("e")
	cSend($KEY_FILTER)
	cSendTextField("dirt")
	cSend("{ENTER}")
	cSend("{ENTER}")
	closeMenuLevel(2)

	; Wait 15 mins
	cSend($KEY_WAIT_MENU)
	cSend("c")
	cSendTextField("15")
	Sleep(2000)
EndFunc

; Verify that all compatible weapons have been correctly loaded
Func validateCompatibleWeaponsData()
	For $i=0 to UBound($MA_DATA_MAIN)-1 Step 1
		For $j=0 to UBound($MA_DATA_MAIN[$i][2])-1 Step 1
			; If we didn't find the name of a weapon, it's probably because the file didn't get included
			If ($MA_DATA_MAIN[$i][2])[$j][1] = "" Then
				$wpId = ($MA_DATA_MAIN[$i][2])[$j][0]
				MsgBox(1, "Failed to load data for: "&$wpId, 'Please verify that the data of this weapon is in one of the loaded folders in the $ITEM_F variable (search in game files for: "id": "'&$wpId&'")')
				Exit
			EndIf
		Next
	Next
EndFunc


Func getWeightValue($weight, $weaponId)
	; Also work as a typo detector, you're welcome
	If StringInStr($weight," g") Then
		$weight = StringReplace($weight," g","")
	ElseIf StringInStr($weight," kg") Then
		$weight = StringReplace($weight," kg","")
		$weight = $weight * 1000
	; If it's just a number, assume "g" like the game does
	ElseIf $weight <> "" And stringregexp($weight, "(-?\d+\.?\d+)" , 1)[0] = $weight Then
		;
	ElseIf $weight <> "" Then
		MsgBox(1,"Error", "Could not handle this weight unit: "&$weight&" for: "&$weaponId)
	EndIf
	return $weight
EndFunc

Func getVolumeValue($volume, $weaponId)
	If StringInStr($volume," mL") Then
		$volume = StringReplace($volume," mL","")
	ElseIf StringInStr($volume," L") Then
		$volume = StringReplace($volume," L","")
		$volume = $volume * 1000
	; If it's just a number, assume "mL" like the game does
	ElseIf $volume <> "" And stringregexp($volume, "(-?\d+\.?\d+)" , 1)[0] = $volume Then
		;
	ElseIf $volume <> "" Then
		MsgBox(1,"Error", "Could not handle this volume unit: "&$volume&" for: "&$weaponId)
	EndIf
	return $volume
EndFunc

Func getAttackCost($volume, $weight)
	return 65 + ($volume / 62.5 + $weight / 60)
EndFunc

; Load name and attackcost of MA compatible weapons
Func loadMACompatibleWeaponsData()
	; For each weapon file (I hope you like for loops)
	For $loadedContent in $LOADED_WEAPONS
		For $i = 0 To 1000 Step 1
			$weaponId = Json_Get($loadedContent[1], '['&$i&']["id"]')
			If $weaponId <> "" Then
				; Go through all MA data
				For $j = 0 to UBound($MA_DATA_MAIN)-1 Step 1
					; Go through all MA compatible weapons
					For $k = 0 to UBound($MA_DATA_MAIN[$j][2])-1 Step 1
						; If the $k compatible weapon has the same id, store its name and its attackcost for later
						If ($MA_DATA_MAIN[$j][2])[$k][0] = $weaponId Then
							$weaponsData = $MA_DATA_MAIN[$j][2]

							; Set name. Some names have only a plural version
							$weaponsData[$k][1] = Json_Get($loadedContent[1], '['&$i&']["name"]["str"]')
							If $weaponsData[$k][1] = "" Then
								$weaponsData[$k][1] = Json_Get($loadedContent[1], '['&$i&']["name"]["str_sp"]')
							EndIf
							If $weaponsData[$k][1] = "" Then
								$weaponsData[$k][1] = Json_Get($loadedContent[1], '['&$i&']["name"]')
							EndIf
							$weight = getWeightValue(Json_Get($loadedContent[1], '['&$i&']["weight"]'), $weaponId)
							$volume = getVolumeValue(Json_Get($loadedContent[1], '['&$i&']["volume"]'), $weaponId)

							; Calculate attack cost (cf formula from attack_cost (cpp))
							$attackCost = getAttackCost($volume, $weight)
							; If $weight or $volume is empty, it probably uses a copy-from. Skip this weapon for now
							If $weight = "" Or $volume = "" Then
								; If it uses some kind of copy-from, we'll assume it will be set in the copy-from, in that case we'll handle them later
								If Json_Get($loadedContent[1], '['&$i&']["copy-from"]') <> "" Then
									$attackCost = 0
									;TODOREMOVE MsgBox(1,"Copy-from and atk 0", $weaponId)
								EndIf
								; Otherwise, do nothing. Some very specific weapons like the bionic claws don't have a weight
							EndIf

							; Set attack cost
							$weaponsData[$k][2] = $attackCost
							$MA_DATA_MAIN[$j][2] = $weaponsData
						EndIf
					Next
				Next
			Else
				ExitLoop
			EndIf
		Next
	Next
EndFunc

; Load name of MA and id of compatible weapons
Func loadMartialArtData()
	For $loadedContent in $LOADED_MARTIAL_ARTS

		; I couldn't get the size of the "array of objects", so here it is. Trying to get an index out of bound returns "" and doesn't throw an error
		For $i=0 To 1000 Step 1
			; Check if the MA uses a copy-from itself. If that's the case skip it (we handle them later)
			If Json_Get($loadedContent[1], '['&$i&']["copy-from"]') Then
				ContinueLoop
			EndIf
			Dim $martialArtRow[1]
			$maRowId = Json_Get($loadedContent[1], '['&$i&']["id"]')

			; If there's still some ma left in the file
			If $maRowId <> "" And Json_Get($loadedContent[1], '['&$i&']["type"]') = "martial_art" Then
				$maName = Json_Get($loadedContent[1], '['&$i&']["name"]["str"]')
				Dim $compatibleWeaponsId[0]
				For $j=0 To 1000 Step 1
					$weaponId = Json_Get($loadedContent[1], '['&$i&']["weapons"]['&$j&']')
					; if there are still weapons left
					If $weaponId <> "" Then
						$weaponId = Json_Get($loadedContent[1], '['&$i&']["weapons"]['&$j&']')

						ReDim $compatibleWeaponsId[UBound($compatibleWeaponsId)+1][3]
						$compatibleWeaponsId[UBound($compatibleWeaponsId)-1][0] = $weaponId
						$compatibleWeaponsId[UBound($compatibleWeaponsId)-1][1] = ""
						$compatibleWeaponsId[UBound($compatibleWeaponsId)-1][2] = 0
					; If the MA has 0 weapon, it's probably because it's an unarmed weapon MA only. In that case, we should use a list of hardcoded weapon ids, for now
					ElseIf $j = 0 Then
						For $k = 0 to UBound($UNARMED_WEAPONS_ID)-1
							ReDim $compatibleWeaponsId[UBound($compatibleWeaponsId)+1][3]
							$compatibleWeaponsId[UBound($compatibleWeaponsId)-1][0] = $UNARMED_WEAPONS_ID[$k]
							$compatibleWeaponsId[UBound($compatibleWeaponsId)-1][1] = ""
							$compatibleWeaponsId[UBound($compatibleWeaponsId)-1][2] = 0
						Next
						ExitLoop
					Else
						ExitLoop
					EndIf
				Next
				ReDim $MA_DATA_MAIN[UBound($MA_DATA_MAIN)+1][3]
				$MA_DATA_MAIN[UBound($MA_DATA_MAIN)-1][0] = $maRowId
				$MA_DATA_MAIN[UBound($MA_DATA_MAIN)-1][1] = $maName
				$MA_DATA_MAIN[UBound($MA_DATA_MAIN)-1][2] = $compatibleWeaponsId
			Elseif  Json_Get($loadedContent[1], '['&$i&']["type"]') = "martial_art" Then
				ExitLoop
			EndIf
		Next
	Next
EndFunc

; Select martial art depending on martial art loading position
Func selectMartialArt($maId)
	; Select the first MA to be in the right position
	cSend($KEY_SELECT_MA)
	cSend("1")
	; Then select the MA based on its loading order
	cSend($KEY_SELECT_MA)
	; Could have been generated with ASCII codes, but between the different ASCII ranges and the "h" skip, it's simpler that way. Has been tested with digits, lower and upper case letters
	Local $maKeys = ["1","2","3","4","5","6","7","8","9","0","a","b","c","d","e","f","g","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z","A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z"]
	For $i = 0 To UBound($MA_DATA_MAIN)-1 Step 1
		If $MA_DATA_MAIN[$i][0] = $maId Then
			cSend($maKeys[$i])
		EndIf
	Next

	; Verify that the right MA is selected
	quickSave()
	$fileOpener = FileOpen($SAVE_FILE_PATH, 0)
	; Remove first line of save file to get only the JSON part, then decode it.
	$fileContentJson = Json_Decode(StringTrimLeft(FileRead($fileOpener), StringLen($FIRST_LINE_SAVE_FILE)))
	FileClose($fileOpener)
	If Json_Get($fileContentJson, '["player"]["martial_arts_data"]["style_selected"]') <> $maId Then
		MsgBox(1, "The selected martial art isn't the right one!", "Please verify the order of martial art files in $MA_FILES (must be alphabetical), and if your modlist doesn't add extra MA. In that case add the file to the $MA_FILES variable")
		Exit
	EndIf
EndFunc

; Returns the index in the $MA_DATA_MAIN variable of the selected martial art
Func getMaDataIndex($maId)
	For $i = 0 to UBound($MA_DATA_MAIN)-1
		If $MA_DATA_MAIN[$i][0] = $maId Then
			return $i
		EndIf
	Next
EndFunc

; Build a wall and a roof around the player
Func buildTestStructures()
	; Build walls
	openSubMenu("m")
	cSend("M")
	cSend($KEY_LEFT)
	cSend($KEY_DOWN)
	cSend("e")
	cSend($KEY_FILTER)
	cSendTextField("resin wall")
	cSend("{ENTER}")
	cSend("{TAB}")
	cSend($KEY_UP)
	cSend($KEY_UP)
	cSend($KEY_UP)
	cSend($KEY_RIGHT)
	cSend($KEY_RIGHT)
	cSend("{ENTER}")
	cSend("{ENTER}")
	closeMenuLevel(2)
	; Build roof
	openSubMenu("m")
	cSend("M")
	cSend($KEY_LEFT)
	cSend($KEY_DOWN)
	cSend($KEY_MAP_UP)
	cSend("e")
	cSend($KEY_FILTER)
	cSendTextField("resin roof")
	cSend("{ENTER}")
	cSend("{TAB}")
	cSend($KEY_UP)
	cSend($KEY_UP)
	cSend($KEY_UP)
	cSend("{ENTER}")
	cSend("{ENTER}")
	cSend("{TAB}")
	cSend($KEY_RIGHT)
	cSend("{ENTER}")
	cSend("{ENTER}")
	cSend("{TAB}")
	cSend($KEY_RIGHT)
	cSend("{ENTER}")
	cSend("{ENTER}")
	closeMenuLevel(2)
EndFunc

; For now will probably be only hit, but could handle things like move, wait and hit etc.
; For a MA like Tiger kung fu, leave only one attack, while with a MA like Barbaran Montante you'll prefer to set a wait key before
Func fightCycle($fCMethod)
	; Wait and hit
	If $fCMethod = 0 Then
		cSend($KEY_WAIT)
		cSend($KEY_UP)
	; Hit only
	ElseIf $fCMethod = 1 Then
		cSend($KEY_UP)
	Else
		MsgBox(1,"Error", "Unkown fight cycle method, please verify the $FIGHT_CYCLES_METHOD value")
		Exit
	EndIf
EndFunc

; Even if very rare, it is possible that the player wield a cloth instead of an item
; In that case, we "brute force" all the items until we wield the right one
; If we had a way to search an exact match in the inventory (like "this"), this function could be removed
Func manageWieldedWeapon($weaponId)
	quicksave()
	$fileOpener = FileOpen($SAVE_FILE_PATH, 0)
	$fileContentJson = Json_Decode(StringTrimLeft(FileRead($fileOpener), StringLen($FIRST_LINE_SAVE_FILE)))
	FileClose($fileOpener)
	If Json_IsObject($fileContentJson) Then
		$currentWeaponId = Json_Get($fileContentJson, '["player"]["weapon"]["typeid"]')
		; We wielded a cloth on the ground! Drop it and go through everything until we get the weapons
		If $currentWeaponId <> $weaponId Then
			For $i = 0 to UBound($ARMOR_NAMES) Step 1
				;Drop current item
				cSend($KEY_DROP_ADJACENT)
				cSend("{NUMPAD5}")
				cSend("{RIGHT}")
				cSend("{ENTER}")
				cSend($KEY_WIELD)
				For $j = $i to UBound($ARMOR_NAMES)-1 Step 1
					cSend("{DOWN}")
				Next
				cSend("{ENTER}")
				quicksave()
				$fileOpener = FileOpen($SAVE_FILE_PATH, 0)
				$fileContentJson = Json_Decode(StringTrimLeft(FileRead($fileOpener), StringLen($FIRST_LINE_SAVE_FILE)))
				FileClose($fileOpener)
				$currentWeaponId = Json_Get($fileContentJson, '["player"]["weapon"]["typeid"]')
				; We found it!
				If $currentWeaponId = $weaponId Then
					ExitLoop
				EndIf
			Next
			; If the weapon still cannot be found, it's a worse bug (probably my fault), stop the tests
			If $currentWeaponId <> $weaponId Then
				MsgBox(1, "Error", "Could not equip the tested weapon: "&$weaponId&". Please report this.")
				Exit
			EndIf
		EndIf
	Else
		MsgBox(1, "Error", "$fileContentJson is not a JSON object, something went wrong. The script needs to stop. Please report this.")
	EndIf
EndFunc



Func killAllMonsters()
	openSubMenu("m")
	cSend("K")
EndFunc

; Store all the important martial arts and compatible weapons data so we don't have to calculate them every time
Func createStoredMaDataMainFile()
	Local $storedMaDataMainJson
	For $i=0 to UBound($MA_DATA_MAIN)-1 Step 1
		Json_Put($storedMaDataMainJson, "["&$i&"].id", $MA_DATA_MAIN[$i][0])
		Json_Put($storedMaDataMainJson, "["&$i&"].name", $MA_DATA_MAIN[$i][1])
		For $j = 0 to UBound($MA_DATA_MAIN[$i][2])-1 Step 1
			Json_Put($storedMaDataMainJson, "["&$i&"].weapons["&$j&"].id", ($MA_DATA_MAIN[$i][2])[$j][0])
			Json_Put($storedMaDataMainJson, "["&$i&"].weapons["&$j&"].name", ($MA_DATA_MAIN[$i][2])[$j][1])
			Json_Put($storedMaDataMainJson, "["&$i&"].weapons["&$j&"].attack_cost", ($MA_DATA_MAIN[$i][2])[$j][2])
		Next
	Next

	Local $Json = Json_Encode($storedMaDataMainJson)
	Local $hFileIDataOpen = FileOpen($STORED_MA_DATA_MAIN_PATH, $FO_OVERWRITE)
	FileWrite($hFileIDataOpen, $Json)
	FileClose($hFileIDataOpen)
EndFunc

; If the file exist, load all the important data from the JSON file
Func loadStoredMaDataMainFile()
	Local $loadedContents[0]
	loadFileFromPath($STORED_MA_DATA_MAIN_PATH, $loadedContents)
	; Will always be size one, but use this for consistency
	For $loadedContent in $loadedContents
		For $i = 0 To 1000 Step 1
			$maId = Json_Get($loadedContent[1], '['&$i&']["id"]')
			If $maId <> "" Then
				ReDim $MA_DATA_MAIN[UBound($MA_DATA_MAIN)+1][3]
				$MA_DATA_MAIN[UBound($MA_DATA_MAIN)-1][0] = Json_Get($loadedContent[1], '['&$i&']["id"]')
				$MA_DATA_MAIN[UBound($MA_DATA_MAIN)-1][1] = Json_Get($loadedContent[1], '['&$i&']["name"]')
				Dim $compatibleWeapons[0]
				For $j = 0 to 1000 Step 1
					$wpId = Json_Get($loadedContent[1], '['&$i&']["weapons"]['&$j&']["id"]')
					If $wpId <> "" Then
						ReDim $compatibleWeapons[UBound($compatibleWeapons)+1][3]
						$compatibleWeapons[UBound($compatibleWeapons)-1][0] = Json_Get($loadedContent[1], '['&$i&']["weapons"]['&$j&']["id"]')
						$compatibleWeapons[UBound($compatibleWeapons)-1][1] = Json_Get($loadedContent[1], '['&$i&']["weapons"]['&$j&']["name"]')
						$compatibleWeapons[UBound($compatibleWeapons)-1][2] = Json_Get($loadedContent[1], '['&$i&']["weapons"]['&$j&']["attack_cost"]')
					Else
						ExitLoop
					EndIf
				Next
				$MA_DATA_MAIN[UBound($MA_DATA_MAIN)-1][2] = $compatibleWeapons
			Else
				ExitLoop
			EndIf
		Next
	Next
EndFunc

; Set attack cost of weapons of weapons that have their weight and/or volume in a parent object
Func handleCopyFromCompatibleWeapons()
	; Go through all compatible weapons
	For $i=0 to UBound($MA_DATA_MAIN)-1 Step 1
		For $j=0 to UBound($MA_DATA_MAIN[$i][2])-1 Step 1
			; If a weapon has 0 attack cost, we have a copy-from to take care of
			If ($MA_DATA_MAIN[$i][2])[$j][2] = 0 Then
				$wpId = ($MA_DATA_MAIN[$i][2])[$j][0]
				; First we store the inheritance tree of this object (just the ids)
				Dim $weaponsInheritanceTree[1][7]
				$weaponsInheritanceTree[UBound($weaponsInheritanceTree)-1][0] = $wpId
				$hasNextParent = True
				$nextParent = ""
				$currentWeaponTreeId = $wpId
				$lastWeaponsInheritanceTreeSize = 1
				$weaponFound = True
				While $hasNextParent
					; This means we went through all the files without finding an ID. This can happen if the copy-from ID file is not included while the weapon file using the copy-from is
					; Without this check, we get an infinite loop
					If not $weaponFound Then
						MsgBox(1,"Error", "Could not find the data for this id:"&$wpId&" Parent:"&$nextParent&". Please add the Folder/File containing this id in the corresponding options.")
						;Exit
					EndIf
					$weaponFound = False
					; Go through every weapon files until we find the parent
					For $loadedContent in $LOADED_WEAPONS
						For $k = 0 To 1000 Step 1
							$weaponId = Json_Get($loadedContent[1], '['&$k&']["id"]')
							If $weaponId = "" Then
								$weaponId = Json_Get($loadedContent[1], '['&$k&']["abstract"]')
							EndIf
							If $weaponId <> "" Then
								; If we found our weapon
								If $currentWeaponTreeId = $weaponId Then
									$weaponFound = True
									; Store every important informations to calculate the attack cost now, so we don't have to go through the files again (this is the more performance costly)
									$weaponsInheritanceTree[UBound($weaponsInheritanceTree)-1][1] = Json_Get($loadedContent[1], '['&$k&']["weight"]'); weight
									$weaponsInheritanceTree[UBound($weaponsInheritanceTree)-1][2] = Json_Get($loadedContent[1], '['&$k&']["relative"]["weight"]'); relative weight
									$weaponsInheritanceTree[UBound($weaponsInheritanceTree)-1][3] = Json_Get($loadedContent[1], '['&$k&']["proportional"]["weight"]'); proportional weight
									$weaponsInheritanceTree[UBound($weaponsInheritanceTree)-1][4] = Json_Get($loadedContent[1], '['&$k&']["volume"]'); volume
									$weaponsInheritanceTree[UBound($weaponsInheritanceTree)-1][5] = Json_Get($loadedContent[1], '['&$k&']["relative"]["volume"]'); relative volume
									$weaponsInheritanceTree[UBound($weaponsInheritanceTree)-1][6] = Json_Get($loadedContent[1], '['&$k&']["proportional"]["volume"]'); proportional volume
									$nextParent = Json_Get($loadedContent[1], '['&$k&']["copy-from"]')
									; This will either be an error or a weapon that inheritate from "itself" (same id) further in the loop
									; To avoid a infinite while loop, there's is a check to know if we went through all files without finding the weapon
									If $nextParent = "" and UBound($weaponsInheritanceTree) = 1  Then
										; In some cases a weapon copy-from the same id as itself
										$nextParent = Json_Get($loadedContent[1], '['&$k&']["id"]')
										; Also means the weapon isn't found yet
										$weaponFound = False
										; And we skip this one, because it's the parent. We'll get it later
										ContinueLoop
									EndIf

									; If we found at least one parent, it's the last, we can go out the while loop
									If UBound($weaponsInheritanceTree) > 1 And $nextParent = "" Then
										$hasNextParent = False
										ExitLoop
									EndIf
									; Otherwise, add the parent and stay in the while loop
									$currentWeaponTreeId = $nextParent
									ReDim $weaponsInheritanceTree[UBound($weaponsInheritanceTree)+1][7]
									$weaponsInheritanceTree[UBound($weaponsInheritanceTree)-1][0] = $nextParent
									ExitLoop
								EndIf
							Else
								If $loadedContent[0] = ($LOADED_WEAPONS[UBound($LOADED_WEAPONS)-1])[0] Then
									MsgBox(1,"Error","The script went through all the loaded weapons and didn't find the weaponId (or the parent) for: "&$wpId&". Please report this.")
									Exit
								EndIf
								ExitLoop
							EndIf
						Next
						If $weaponFound Then
							ExitLoop
						EndIf
					Next
				WEnd

				; Then we rebuild the weight and volume from the tree
				$weight = 0
				$volume = 0
				For $k = UBound($weaponsInheritanceTree)-1 to 0 Step -1
					$weight = ($weaponsInheritanceTree[$k][1] <> "") ? getWeightValue($weaponsInheritanceTree[$k][1], $wpId) : $weight
					$weight = ($weaponsInheritanceTree[$k][2] <> "") ? $weight + getWeightValue($weaponsInheritanceTree[$k][2], $wpId) : $weight
					$weight = ($weaponsInheritanceTree[$k][3] <> "") ? $weight * getWeightValue($weaponsInheritanceTree[$k][3], $wpId) : $weight
					$volume = ($weaponsInheritanceTree[$k][4] <> "") ? getVolumeValue($weaponsInheritanceTree[$k][4], $wpId) : $volume
					$volume = ($weaponsInheritanceTree[$k][5] <> "") ? $volume + getVolumeValue($weaponsInheritanceTree[$k][5], $wpId) : $volume
					$volume = ($weaponsInheritanceTree[$k][6] <> "") ? $volume * getVolumeValue($weaponsInheritanceTree[$k][6], $wpId) : $volume
				Next
				$attackCost = getAttackCost($weight, $volume)
				; If the attack cost is still = 0, something went seriously wrong
				If $attackCost = 0 Then
					MsgBox(1,"Error", $wpId&" Something went wrong, attackCost=0, Please report this.")
				EndIf
				; DEBUG to see weapon attack cost from this function
				Local $testResultsFileOpener = FileOpen($TESTS_RESULT_PATH , $FO_APPEND); Contains a reference to the result file, to write the results in it
				;FileWriteLine($testResultsFileOpener, $wpId & ", N:" & ($MA_DATA_MAIN[$i][2])[$j][1]&", V:"& $volume&", W:"&$weight/0.453&", ATKC:"&$attackCost)
				Local $testResultsFileOpener = FileOpen($TESTS_RESULT_PATH , $FO_APPEND); Contains a reference to the result file, to write the results in it
				$weaponsData = $MA_DATA_MAIN[$i][2]
				$weaponsData[$j][2] = $attackCost
				$MA_DATA_MAIN[$i][2] = $weaponsData
			EndIf
		Next
	Next
EndFunc

; Set the copy-from martial arts values (it only adds extra weapons)
Func handleCopyFromMartialArts()
	For $loadedContent in $LOADED_MARTIAL_ARTS
		; I couldn't get the size of the "array of objects", so here it is. Trying to get an index out of bound returns "" and doesn't throw an error
		For $i=0 To 1000 Step 1
			; Check if the MA uses a copy-from itself. If that's the case skip it (we handle them later)
			$maId = Json_Get($loadedContent[1], '['&$i&']["id"]')
			$maCopyFrom = Json_Get($loadedContent[1], '['&$i&']["copy-from"]')
			If $maId <> "" And $maCopyFrom = $maId And Json_Get($loadedContent[1], '['&$i&']["type"]') = "martial_art" Then
				; Go through existing MA
				For $j=0 to UBound($MA_DATA_MAIN)-1 Step 1
					; If we found the right MA
					If $maId = $MA_DATA_MAIN[$j][0] Then
						$weaponsData = $MA_DATA_MAIN[$j][2]
						; Add weapons
						For $k=0 to 1000 Step 1
							Dim $martialArtRow[1]
							$weaponId = Json_Get($loadedContent[1], '['&$i&']["extend"]["weapons"]['&$k&']')
							If $weaponId <> "" Then
								ReDim $weaponsData[UBound($weaponsData)+1][3]
								$weaponsData[UBound($weaponsData)-1][0] = $weaponId
								$weaponsData[UBound($weaponsData)-1][1] = ""
								$weaponsData[UBound($weaponsData)-1][2] = 0
							Else
								ExitLoop
							EndIf
						Next
						$MA_DATA_MAIN[$j][2] = $weaponsData
						ExitLoop
					EndIf
				Next
			ElseIf $maCopyFrom <> "" And Json_Get($loadedContent[1], '['&$i&']["type"]') = "martial_art" Then
				MsgBox(1,"Error","This script doesn't handle copy-from different martial arts ("&$maId&"), as it seems it doesn't exist in the game for now")
				Exit
			ElseIf $maId = "" Then
				ExitLoop
			EndIf
		Next
	Next

EndFunc

; Load all MA files content in a variable, as JSON
Func loadMartialArtFiles()
	For $folderPath in $MA_FILES
		; If it's a folder
		If StringRight($folderPath, 1) = "\" Then
			$files = _FileListToArray($folderPath, "*.*", 1)
			For $fileName in $files
				; Do not remove, first row contains length of array
				If not StringInStr($fileName, ".json") Then
					ContinueLoop
				Else
					loadFileFromPath($folderPath&$fileName, $LOADED_MARTIAL_ARTS)
				EndIf
			Next
		; If it's a single file
		Else
			loadFileFromPath($folderPath, $LOADED_MARTIAL_ARTS)
		EndIf
	Next
EndFunc

; Load all weapon files content in a variable, as JSON
Func loadWeaponFiles()
	For $folderPath in $WEAPON_FILES
		; If it's a folder
		If StringRight($folderPath, 1) = "\" Then
			$files = _FileListToArray($folderPath, "*.*", 1)
			For $fileName in $files
				; Do not remove, first row contains length of array
				If not StringInStr($fileName, ".json") Then
					ContinueLoop
				Else
					loadFileFromPath($folderPath&$fileName, $LOADED_WEAPONS)
				EndIf
			Next
		; If it's a single file
		Else
			loadFileFromPath($folderPath, $LOADED_WEAPONS)
		EndIf
	Next
EndFunc

; Verify that the monsters have their life set to $MAX_MONSTERS_HEALTH
Func validateMonstersHealthModificationSubPart($fileOpener, $filePath)
	If $fileOpener = -1 Then
		MsgBox($MB_SYSTEMMODAL, "", "An error occurred when reading/writing the file:"&$filePath)
		Exit
	EndIf
	; Decode file json content
	$fileContentJson = Json_Decode(FileRead($fileOpener))
	For $i=0 to 1000 Step 1
		$monId = Json_Get($fileContentJson, '['&$i&']["id"]')
		If $monId <> "" Then
			For $monsterId in $MONSTER_IDS
				If $monId = $monsterId Then
					If Not (Json_Get($fileContentJson, '['&$i&']["hp"]') = $MAX_MONSTERS_HEALTH) Then
						MsgBox(1,"Error", "The monster: "&$monId&" doesn't have his -hp- field set to "&$MAX_MONSTERS_HEALTH)
						Exit
					EndIf
				EndIf
			Next
		Else
			ExitLoop
		EndIf
	Next
	FileClose($fileOpener)

EndFunc

; Go through monster files
Func validateMonstersHealthModification()
	For $folderPath in $MONSTER_FILES
		; If it's a folder
		If StringRight($folderPath, 1) = "\" Then
			$files = _FileListToArray($folderPath, "*.*", 1)
			For $fileName in $files
				; Do not remove, first row contains length of array
				If not StringInStr($fileName, ".json") Then
					ContinueLoop
				Else
					$fileOpener = FileOpen($folderPath&$fileName, 0)
					validateMonstersHealthModificationSubPart(FileOpen($folderPath&$fileName, 0), $folderPath&$fileName)
				EndIf
			Next
		; If it's a single file
		Else
			validateMonstersHealthModificationSubPart(FileOpen($folderPath, 0), $folderPath&$fileName)
		EndIf
	Next
EndFunc

; Init game
Func Init()

	; Get first line in the save file once. Then we can remove it and treat what's left in the file as JSON
	$fileOpener = FileOpen($SAVE_FILE_PATH, 0)
	If $fileOpener = -1 Then
		MsgBox(1,"Error", "Could not load the save")
		Exit
	EndIf
	$FIRST_LINE_SAVE_FILE = FileReadLine($SAVE_FILE_PATH, 1)
	; Get the current turn to know when the test began
	$CURRENT_TURN = _StringBetween(FileReadLine($SAVE_FILE_PATH, 3), '"turn": ', ",")[0]
	FileClose($fileOpener)
	; If the file containing "computed" important data exists and the SPEED_UP_TESTS_START option is set to true
	If FileExists($STORED_MA_DATA_MAIN_PATH) and $SPEED_UP_TESTS_START Then
		loadStoredMaDataMainFile()
	Else
		; Load all files and format $MA_DATA_MAIN with interesting data (ma id, ma name, compatible weapons with id, name and attack cost)
		loadMartialArtFiles()
		loadWeaponFiles()
		loadMartialArtData()
		handleCopyFromMartialArts()
		loadMACompatibleWeaponsData()
		validateCompatibleWeaponsData()
		handleCopyFromCompatibleWeapons()
		validateMonstersHealthModification()
		; Finally, store all those computed informations in a Json file, to save some time next time
		createStoredMaDataMainFile()
	EndIf

EndFunc

; MAIN
Init()


; Start sending keys only when the game is the active window
cWaitForActiveWindow($GAME_WINDOW_NAME)
killAllMonsters()

If $BUILD_STRUCTURES = 1 Then
	buildTestStructures()
EndIf




Local $testResultsFileOpener = FileOpen($TESTS_RESULT_PATH , $FO_APPEND); Contains a reference to the result file, to write the results in it
FileWriteLine($testResultsFileOpener,  @CRLF&@CRLF & "# NEW TESTS STARTING AT - " & _NowDate() & " " & _NowTime()&@CRLF)
FileWriteLine($testResultsFileOpener, "Fight cycle method: "&$FIRST_CYCLE_EXPLANATIONS[$FIGHT_CYCLES_METHOD]&", Iterations: "&$ITERATIONS&", Fight cycles: "&$FIGHT_CYCLES)

; 1st Loop monsters
For $d = 0 to UBound($MONSTER_NAMES)-1 Step 1
	; 2nd Stats/Skills
	For $e = 0 to UBound($PLAYER_STATS)-1 Step 1
		FileWriteLine($testResultsFileOpener, "## STATS: "&$PLAYER_STATS[$e]& ", SKILLS: "&$PLAYER_SKILLS[$e]& ", M: "&$MONSTER_NAMES[$d])
		FileWriteLine($testResultsFileOpener, "| Martial Art Name                | Weapon                          | Avg dmg |")
		FileWriteLine($testResultsFileOpener, "|---------------------------------|---------------------------------|---------|")

		setStats($PLAYER_STATS[$e])

		Dim $TESTS_RESULT[0]; each row = martial art name, weapon name, average damage
		; 3nd Martial Arts
		For $f = 0 To UBound($MA_IDS)-1 Step 1
			; SELECT MA
			selectMartialArt($MA_IDS[$f])

			; 4st Weapons
			; Get martial art data index (at this index is the martial art id, name, and compatible weapons with id, name and attack cost)
			$maDataIndex = getMaDataIndex($MA_IDS[$f])
			For $g = 0 to UBound($MA_DATA_MAIN[$maDataIndex][2])-1 Step 1
				; If we want to only test some weapons and this weapon isn't one of them, skip it
				If UBound($WEAPONS_ID_RESTRICTION) > 0 And _ArraySearch($WEAPONS_ID_RESTRICTION, ($MA_DATA_MAIN[$maDataIndex][2])[$g][0], 0, 0, 0, 2, 1, 0) = -1 Then
					ContinueLoop
				EndIf
				; 5th Iterations
				$totalDamage = 0
				For $i = 0 To $ITERATIONS-1 Step 1
					; Every X iterations we want to fully reset the player. The more fight cycles for each iteration, the more often we want to fully reset the player.
					; This is also where we'll save and calculate damages done to monster
					If Mod($i, Round(36/$FIGHT_CYCLES)) == 0 Then
						; If it's not the first monster spawn, get total damage
						If $i <> 0 Then
							; Get damage done to monster from the save
							$totalDamage += getMonsterGroupeIterationDamageAndKillThem($d)
						EndIf
						resetPlayerFull(($MA_DATA_MAIN[$maDataIndex][2])[$g][1], ($MA_DATA_MAIN[$maDataIndex][2])[$g][0], $PLAYER_SKILLS[$e])
						; Spawn the monster
						spawnMonster($MONSTER_NAMES[$d])
					Else
						resetPlayer()
					EndIf

					; Hit him a few times
					; 6th Fight cycles (may be only hitting the monster, or wait then hit etc.)
					For $j = 0 To $FIGHT_CYCLES-1 Step 1
						fightCycle($FIGHT_CYCLES_METHOD)
					Next
				Next
				; Get damage done to monster from the save
				$totalDamage += getMonsterGroupeIterationDamageAndKillThem($d)

				; Compute average damage
				$avgDmg = $totalDamage/($ITERATIONS * $FIGHT_CYCLES)
				$correctedAvgDmg = $avgDmg * 100 / ($MA_DATA_MAIN[$maDataIndex][2])[$g][2]; Correct average damage with attack cost of weapon

				ReDim $TESTS_RESULT[UBound($TESTS_RESULT)+1][3]; each row = martial art name, weapon name, average damage
				$TESTS_RESULT[UBound($TESTS_RESULT)-1][0] = $MA_DATA_MAIN[$maDataIndex][1]
				$TESTS_RESULT[UBound($TESTS_RESULT)-1][1] = ($MA_DATA_MAIN[$maDataIndex][2])[$g][1]
				$TESTS_RESULT[UBound($TESTS_RESULT)-1][2] = $correctedAvgDmg

				;Uncomment to test performances improvement FileWriteLine($testResultsFileOpener, "# WEAPON FINISHED AT " & _NowDate() & " " & _NowTime())
			Next
		Next

		; Sort results by DPS
		_ArraySort($TESTS_RESULT, 1, 0, 0, 2)
		; Display results

		For $i=0 to UBound($TESTS_RESULT)-1 Step 1
			; Add horizontal alignment
			For $j = 0 to 1 Step 1
				For $k = 0 to 30-StringLen($TESTS_RESULT[$i][$j]) Step 1
					$TESTS_RESULT[$i][$j]&= " "
				Next
			Next
			FileWriteLine($testResultsFileOpener, "| "&$TESTS_RESULT[$i][0]&" | "&$TESTS_RESULT[$i][1]&" | "&Round($TESTS_RESULT[$i][2], 2)&" |")
		Next
		FileWriteLine($testResultsFileOpener, "|                                 |                                 |       |")
	Next
Next

FileClose($testResultsFileOpener)
MsgBox(1, ":)", "The tests are finished")