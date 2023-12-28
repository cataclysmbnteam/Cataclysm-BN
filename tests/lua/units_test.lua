-- Get initial test data
local angle_degrees = test_data["angle_degrees"]
local energy_kilojoules = test_data["energy_kilojoules"]
local mass_kilograms = test_data["mass_kilograms"]
local volume_liters = test_data["volume_liters"]

-- Set test results
test_data["angle_arcmins"] = UnitsAngle.from_degrees(angle_degrees):to_arcmin()
test_data["energy_joules"] = UnitsEnergy.from_kilojoule(energy_kilojoules):to_joule()
test_data["mass_grams"] = UnitsMass.from_kilogram(mass_kilograms):to_gram()
test_data["volume_milliliters"] = UnitsVolume.from_liter(volume_liters):to_milliliter()
