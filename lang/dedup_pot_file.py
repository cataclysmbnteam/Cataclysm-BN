#!/usr/bin/env python3
"Resolve duplicates and conflicts within POT file."

import os
from optparse import OptionParser
from typing import Dict, List, Tuple

import polib
from polib import POEntry

##
##  FUNCTIONS
##

def first_occurence(entry: POEntry) -> str:
    if len(entry.occurrences) > 0:
        return entry.occurrences[0][0]
    else:
        return 'unknown_file'


def merged_flags(entry1: POEntry, entry2: POEntry) -> List[str]:
    flags1 = set(entry1.flags)
    flags2 = set(entry2.flags)
    return entry1.flags + list(flags2 - flags1)


# Assumes 2nd entry has only 1 comment.
def merged_comments(entry1: POEntry, entry2: POEntry) -> str:
    if entry2.comment[0] in entry1.comment:
        return entry1.comment
    else:
        return entry1.comment + entry2.comment


def merged_occurrences(entry1: POEntry, entry2: POEntry) -> List[Tuple[str, int]]:
    res = entry1.occurrences
    for x in entry2.occurrences:
        if x not in res:
            res.append(x)
    return res


def merged_msgid_pl(entry1: POEntry, entry2: POEntry) -> str:
    pl1 = entry1.msgid_plural
    pl2 = entry2.msgid_plural
    if not pl1:
        return pl2
    elif not pl2:
        return pl1
    if pl1 != pl2:
        print(f"WARNING: plural form mismatch for msgid='{entry1.msgid}' msgctxt='{entry1.msgctxt}' in {first_occurence(entry1)} and {first_occurence(entry2)}: '{pl1}' vs '{pl2}'")
    return pl1


def resolve_duplicates(entries: List[POEntry]) -> List[POEntry]:
    nodup: List[POEntry] = []
    nodup_keys: Dict[Tuple[str, str], int] = {}
    for entry in entries:
        # Strip line numbers, remove repeating occurrences
        entry.occurrences = list(dict.fromkeys([(x[0], None) for x in entry.occurrences]))

        # The only flag we use is c-format
        if entry.flags != [] and entry.flags != ['c-format']:
            print(f"WARNING: unexpected flags {entry.flags} for msgid='{entry.msgid}' in {first_occurence(entry)}")

        # Convert comment string into list of strings (simplifies merging of comments)
        entry.comment = [entry.comment]

        key = (entry.msgid, entry.msgctxt)
        if key not in nodup_keys:
            # Add new entry
            nodup_keys[key] = len(nodup)
            nodup.append(entry)
        else:
            # Merge with existing entry
            x = nodup[nodup_keys[key]]

            new_comment = merged_comments(x, entry)
            new_flags = merged_flags(x, entry)
            new_occurrences = merged_occurrences(x, entry)
            new_msgid_plural = merged_msgid_pl(x, entry)

            # There also exists trcomment field, but we ignore it as
            # translator comments are used only in PO, not POT.

            x.comment = new_comment
            x.flags = new_flags
            x.occurrences = new_occurrences
            x.msgid_plural = new_msgid_plural

    for entry in nodup:
        # Convert comments from lists of strings back into strings
        entry.comment = '\n'.join(entry.comment)
    return nodup

##
##  MAIN
##

cmd_usage = 'usage: %prog [options] filename'
parser = OptionParser(usage=cmd_usage)
(options, args) = parser.parse_args()

if len(args) != 1:
    print("Expected 1 argument")
    exit(1)

filename = args[0]

if not os.path.isfile(filename):
    print("Error: File not found.")
    exit(1)

pot = polib.pofile(filename)
entries = [e for e in pot]
entries_nodup = resolve_duplicates(entries)

res = polib.POFile()
res.metadata = pot.metadata
for e in entries_nodup:
    # Entries with plural forms use 1+ `msgstr[x]` fields instead of single `msgstr`.
    # We have to explicitly specify which one we want,
    # as polib by default always writes `msgstr`.
    if e.msgid_plural:
        e.msgstr = None
        e.msgstr_plural = {0:'', 1:''}
    else:
        e.msgstr = ''
        e.msgstr_plural = None
    res.append(e)
res.save(filename, newline='\n')
