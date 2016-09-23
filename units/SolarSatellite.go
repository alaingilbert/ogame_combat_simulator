package units

type SolarSatellite struct {
	Ship
}

func NewSolarSatellite() *SolarSatellite {
	ss := new(SolarSatellite)
	ss.OgameID = 212
	ss.Name = "SolarSatellite"
	ss.Price = Price{0, 2000, 500}
	ss.ShieldPower = 1
	ss.WeaponPower = 1
	ss.InitHullPlating()
	ss.RestoreShield()
	ss.CargoCapacity = 0
	ss.BaseSpeed = 0
	ss.FuelComsumption = 0
	ss.RapidfireFrom = map[string]int{"Battlecruiser": 5, "Destroyer": 5,
		"Bomber": 5, "Recycler": 5, "ColonyShip": 5, "Battleship": 5, "Cruiser": 5,
		"HeavyFighter": 5, "LightFighter": 5, "LargeCargo": 5, "Deathstar": 1250,
		"SmallCargo": 5}
	return ss
}
