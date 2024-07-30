import json

userInput = input("Please input a valid filename/path below:\n")

# Creates the converted name
inputSep = userInput.split('.')
inputSep[0] += "converted"
convertName = '.'.join(inputSep)

# Opens user's file as JSON
userFile = open(userInput)
userJson = json.load(userFile)
# We don't need the user's file once it's been loaded in
userFile.close()

# List to hold the dictionaries that are the converted entries
tempList = []
for item in userJson:
    # Type and ID are assured, but not all mutations have categories
    if "category" in item:
        # Put together new entry using info from old entry
        tempDict = {
            "type": item["type"],
            "id": item["id"],
            "copy-from": item["id"],
            "delete": {"category": item["category"]}
        }
        # Add new entry to the list
        tempList.append(tempDict)

# Output the finished converted JSON
convertFile = open(convertName, "w")
json.dump(tempList, convertFile, indent = 2)
convertFile.close()