package units

type HeavyFighter struct {
	Ship
}

func NewHeavyFighter() *HeavyFighter {
	hf := new(HeavyFighter)
	hf.OgameID = 205
	hf.Name = "HeavyFighter"
	hf.Price = Price{6000, 4000, 0}
	hf.ShieldPower = 25
	hf.WeaponPower = 150
	hf.InitHullPlating()
	hf.RestoreShield()
	hf.CargoCapacity = 100
	hf.BaseSpeed = 10000
	hf.FuelComsumption = 75
	hf.RapidfireFrom = map[string]int{"Battlecruiser": 4, "Deathstar": 100}
	hf.RapidfireAgainst = map[string]int{"EspionageProbe": 5, "SolarSatellite": 5, "SmallCargo": 3}
	return hf
}
