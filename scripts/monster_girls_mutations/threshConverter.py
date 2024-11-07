import json
from pathlib import Path

# This inputted file should be the one with all the "delete"s and "extend"s
preInput = input("Please input the file you want to base off of:\n")

preJson = json.loads(Path(preInput).read_text())

# List for holding the entries in the above file, and loop to extract all of them
preList = []
for entry in preJson:
    if "extend" in entry:
        preList.append(entry["id"])

# This one solely exists to get the mutations with threshreqs
userInput = input("Please input a valid filename/path below for processing:\n")

# Opens above file as JSON
userJson = json.loads(Path(userInput).read_text())

# Dictionary for converting vanilla threshreqs to monstergirl ones.
threshConvDict = {
    "THRESH_FELINE": "THRESH_NEKO",
    "THRESH_LUPINE": "THRESH_DOGGIRL",
    "THRESH_PLANT": "THRESH_DRYAD",
    "THRESH_BIRD": "THRESH_HARPY",
    "THRESH_URSINE": "THRESH_BEARGIRL",
    "THRESH_SPIDER": "THRESH_SPIDERGIRL",
    "THRESH_SLIME": "THRESH_SLIMEGIRL",
    "THRESH_MOUSE": "THRESH_MOUSEGIRL",
    "THRESH_CATTLE": "THRESH_COWGIRL"
}

# Storage for the results in the form of a dictionary
resultDict = {}

# For each entry in the vanilla file: if it is also in the monstergirl file
# and it has at least one threshreq that matches a monstergirl one,
#  use the conversion dictionary
for item in userJson:
    if item["id"] in preList:
        if "threshreq" in item:
            tempList = []
            for thresh in item["threshreq"]:
                if thresh in threshConvDict:
                    tempList.append(threshConvDict[thresh])
            resultDict[item["id"]] = tempList

# Go through each entry in the monstergirl json and if it's in the dictionary of results,
#  add in the relevant entry to the extend.
for entry in preJson:
    if entry["id"] in resultDict:
        entry["extend"]["threshreq"] = resultDict[entry["id"]]

# Dump the json into the original file
Path(preInput).write_text(json.dumps(preJson, indent=2))
