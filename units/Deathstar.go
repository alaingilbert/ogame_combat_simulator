package units

type Deathstar struct {
	Ship
}

func NewDeathstar() *Deathstar {
	deathstar := new(Deathstar)
	deathstar.OgameID = 214
	deathstar.Name = "Deathstar"
	deathstar.Price = Price{5000000, 4000000, 1000000}
	deathstar.ShieldPower = 50000
	deathstar.WeaponPower = 200000
	deathstar.InitHullPlating()
	deathstar.RestoreShield()
	deathstar.CargoCapacity = 1000000
	deathstar.BaseSpeed = 100
	deathstar.FuelComsumption = 1
	deathstar.RapidfireAgainst = map[string]int{"SmallCargo": 250,
		"LargeCargo": 250, "LightFighter": 200, "HeavyFighter": 100, "Cruiser": 33,
		"Battleship": 30, "ColonyShip": 250, "Recycler": 250, "EspionageProbe": 1250,
		"SolarSatellite": 1250, "Bomber": 25, "Destroyer": 5, "RocketLauncher": 200,
		"LightLaser": 200, "HeavyLaser": 100, "GaussCannon": 50, "IonCannon": 100,
		"Battlecruiser": 15}
	return deathstar
}
