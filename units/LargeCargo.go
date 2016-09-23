package units

type LargeCargo struct {
	Ship
}

func NewLargeCargo() *LargeCargo {
	lc := new(LargeCargo)
	lc.OgameID = 203
	lc.Name = "LargeCargo"
	lc.Price = Price{6000, 6000, 0}
	lc.ShieldPower = 25
	lc.WeaponPower = 5
	lc.InitHullPlating()
	lc.RestoreShield()
	lc.CargoCapacity = 25000
	lc.BaseSpeed = 7500
	lc.FuelComsumption = 50
	lc.RapidfireFrom = map[string]int{"Battlecruiser": 3, "Deathstar": 250}
	lc.RapidfireAgainst = map[string]int{"EspionageProbe": 5, "SolarSatellite": 5}
	return lc
}
