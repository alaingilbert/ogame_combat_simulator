package units

type SmallCargo struct {
	Ship
}

func NewSmallCargo() *SmallCargo {
	sc := new(SmallCargo)
	sc.OgameID = 202
	sc.Name = "SmallCargo"
	sc.Price = Price{2000, 2000, 0}
	sc.ShieldPower = 10
	sc.WeaponPower = 5
	sc.InitHullPlating()
	sc.RestoreShield()
	sc.CargoCapacity = 5000
	sc.BaseSpeed = 5000
	sc.FuelComsumption = 10
	sc.RapidfireFrom = map[string]int{"Battlecruiser": 3, "HeavyFighter": 3, "Deathstar": 250}
	sc.RapidfireAgainst = map[string]int{"EspionageProbe": 5, "SolarSatellite": 5}
	return sc
}
