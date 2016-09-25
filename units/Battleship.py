package units

type Battleship struct {
	Ship
}

func NewBattleship() *Battleship {
	b := new(Battleship)
	b.OgameID = 207
	b.Name = "Battleship"
	b.Price = Price{45000, 15000, 0}
	b.ShieldPower = 200
	b.WeaponPower = 1000
	b.InitHullPlating()
	b.RestoreShield()
	b.CargoCapacity = 1500
	b.BaseSpeed = 10000
	b.FuelComsumption = 500
	b.RapidfireFrom = map[string]int{"Battlecruiser": 7, "Deathstar": 30}
	b.RapidfireAgainst = map[string]int{"EspionageProbe": 5, "SolarSatellite": 5}
	return b
}
