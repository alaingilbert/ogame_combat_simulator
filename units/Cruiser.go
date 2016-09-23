package units

type Cruiser struct {
	Ship
}

func NewCruiser() *Cruiser {
	lf := new(Cruiser)
	lf.Name = "Cruiser"
	lf.Price = Price{20000, 7000, 2000}
	lf.ShieldPower = 50
	lf.WeaponPower = 400
	lf.InitHullPlating()
	lf.RestoreShield()
	lf.CargoCapacity = 800
	lf.BaseSpeed = 15000
	lf.FuelComsumption = 300
	lf.RapidfireFrom = map[string]int{"BattleCruiser": 4, "Deathstar": 33}
	lf.RapidfireAgainst = map[string]int{"EspionageProbe": 5, "SolarSatellite": 5, "LightFighter": 6, "RocketLauncher": 10}
	return lf
}
