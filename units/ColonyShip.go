package units

type ColonyShip struct {
	Ship
}

func NewColonyShip() *ColonyShip {
	cs := new(ColonyShip)
	cs.OgameID = 208
	cs.Name = "ColonyShip"
	cs.Price = Price{10000, 20000, 10000}
	cs.ShieldPower = 100
	cs.WeaponPower = 50
	cs.InitHullPlating()
	cs.RestoreShield()
	cs.CargoCapacity = 7500
	cs.BaseSpeed = 2500
	cs.FuelComsumption = 1000
	cs.RapidfireFrom = map[string]int{"Deathstar": 250}
	cs.RapidfireAgainst = map[string]int{"EspionageProbe": 5, "SolarSatellite": 5}
	return cs
}
