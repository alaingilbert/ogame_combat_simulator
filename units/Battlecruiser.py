class Battlecruiser(object):
	bc := new(Battlecruiser)
	bc.OgameID = 215
	bc.Name = "Battlecruiser"
	bc.Price = Price{30000, 40000, 15000}
	bc.ShieldPower = 400
	bc.WeaponPower = 700
	bc.InitHullPlating()
	bc.RestoreShield()
	bc.CargoCapacity = 750
	bc.BaseSpeed = 1000
	bc.FuelComsumption = 250
	bc.RapidfireFrom = map[string]int{"Destroyer": 2, "Deathstar": 15}
	bc.RapidfireAgainst = map[string]int{"EspionageProbe": 5,
		"SolarSatellite": 5, "SmallCargo": 3, "LargeCargo": 3, "HeavyFighter": 4,
		"Cruiser": 4, "Battleship": 7}
	return bc
}
