package units

type Bomber struct {
	Ship
}

func NewBomber() *Bomber {
	bomber := new(Bomber)
	bomber.OgameID = 211
	bomber.Name = "Bomber"
	bomber.Price = Price{50000, 25000, 15000}
	bomber.ShieldPower = 500
	bomber.WeaponPower = 1000
	bomber.InitHullPlating()
	bomber.RestoreShield()
	bomber.CargoCapacity = 500
	bomber.BaseSpeed = 4000
	bomber.FuelComsumption = 1000
	bomber.RapidfireFrom = map[string]int{"Deathstar": 25}
	bomber.RapidfireAgainst = map[string]int{"EspionageProbe": 5, "IonCannon": 10,
		"SolarSatellite": 5, "RocketLauncher": 20, "LightLaser": 20, "HeavyLaser": 10}
	return bomber
}
