:: Extract strings from JSONs in this folder
@echo off
if not exist lang md lang
python extract_json_strings.py -i .\ -o lang\extracted_strings.pot
python dedup_pot_file.py lang\extracted_strings.pot
echo Done!
pause
@echo on 
