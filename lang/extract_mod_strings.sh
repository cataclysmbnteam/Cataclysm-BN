# Extract strings from JSONs in this folder
mkdir -p lang
python3 extract_json_strings.py -i ./ -o lang/extracted_strings.pot
python3 dedup_pot_file.py lang/extracted_strings.pot
echo Done!
