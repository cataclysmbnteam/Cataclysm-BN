#!/usr/bin/env python3

import sys
import glob
import json

assert(len(sys.argv) == 3)
blacklist_filename = sys.argv[1]
do_lua = sys.argv[2] == "1"

with open(blacklist_filename) as blacklist_file:
    blacklist = {s.rstrip('\n') for s in blacklist_file.readlines()}

mods_to_keep = []

def add_mods(mods):
    for mod in mods:
        if not mod in all_mod_dependencies:
            # Either an invalid mod id, or blacklisted.
            return False
    for mod in mods:
        if not mod in mods_to_keep:
            mods_to_keep.append(mod)
    return True

all_mod_dependencies = {}

for info in glob.glob('data/mods/*/modinfo.json'):
    mod_info = json.load(open(info))
    for e in mod_info:
        if e["type"] == "MOD_INFO":
            if not do_lua and "lua_api_version" in e:
                continue
            ident = e["id"]
            if not ident in blacklist:
                all_mod_dependencies[ident] = e.get("dependencies", [])

for mod in all_mod_dependencies:
    if not mod in mods_to_keep:
        if add_mods(all_mod_dependencies[mod]):
            mods_to_keep.append(mod)

print(','.join(mods_to_keep))
