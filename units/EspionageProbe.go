package units

type EspionageProbe struct {
	Ship
}

func NewEspionageProbe() *EspionageProbe {
	ep := new(EspionageProbe)
	ep.OgameID = 210
	ep.Name = "EspionageProbe"
	ep.Price = Price{0, 1000, 0}
	ep.ShieldPower = 0.01
	ep.WeaponPower = 0.01
	ep.InitHullPlating()
	ep.RestoreShield()
	ep.CargoCapacity = 5
	ep.BaseSpeed = 100000000
	ep.FuelComsumption = 1
	ep.RapidfireFrom = map[string]int{"Battlecruiser": 5, "Destroyer": 5,
		"Bomber": 5, "Recycler": 5, "ColonyShip": 5, "Battleship": 5, "Cruiser": 5,
		"HeavyFighter": 5, "LightFighter": 5, "LargeCargo": 5, "Deathstar": 1250,
		"SmallCargo": 5}
	return ep
}
