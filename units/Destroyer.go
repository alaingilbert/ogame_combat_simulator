package units

type Destroyer struct {
	Ship
}

func NewDestroyer() *Destroyer {
	destroyer := new(Destroyer)
	destroyer.OgameID = 213
	destroyer.Name = "Destroyer"
	destroyer.Price = Price{60000, 50000, 15000}
	destroyer.ShieldPower = 500
	destroyer.WeaponPower = 2000
	destroyer.InitHullPlating()
	destroyer.RestoreShield()
	destroyer.CargoCapacity = 2000
	destroyer.BaseSpeed = 5000
	destroyer.FuelComsumption = 1000
	destroyer.RapidfireFrom = map[string]int{"Deathstar": 5}
	destroyer.RapidfireAgainst = map[string]int{"EspionageProbe": 5, "SolarSatellite": 5, "LightLaser": 10, "Battlecruiser": 2}
	return destroyer
}
