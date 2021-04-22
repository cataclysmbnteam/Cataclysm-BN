#!/usr/bin/env python3
"Concatenate 2 POT files. The resulting file may contain duplicate or conflicting entries."

import polib
import os
from optparse import OptionParser

cmd_usage = 'usage: %prog [options] input1 input2 output'
parser = OptionParser(usage=cmd_usage)
(options, args) = parser.parse_args()

if len(args) != 3:
    print("Expected 3 arguments")
    exit(1)

source_file_1 = args[0]
source_file_2 = args[1]
destination_file = args[2]

if not os.path.isfile(source_file_1):
    print("Error: Couldn't find file '{}'.".format(source_file_1))
    exit(1)
if not os.path.isfile(source_file_2):
    print("Error: Couldn't find file '{}'.".format(source_file_2))
    exit(1)

print("==> Merging '{}' and '{}' into '{}".format(source_file_1, source_file_2, destination_file))

pot1 = polib.pofile(source_file_1)
pot2 = polib.pofile(source_file_2)

res = polib.POFile()
res.metadata = pot1.metadata

for e in pot1:
    res.append(e)

for e in pot2:
    res.append(e)

res.save(destination_file, newline='\n')
