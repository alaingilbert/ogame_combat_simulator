package units

type Recycler struct {
	Ship
}

func NewRecycler() *Recycler {
	recycler := new(Recycler)
	recycler.OgameID = 209
	recycler.Name = "Recycler"
	recycler.Price = Price{10000, 6000, 2000}
	recycler.ShieldPower = 10
	recycler.WeaponPower = 1
	recycler.InitHullPlating()
	recycler.RestoreShield()
	recycler.CargoCapacity = 20000
	recycler.BaseSpeed = 2000
	recycler.FuelComsumption = 300
	recycler.RapidfireFrom = map[string]int{"Deathstar": 250}
	recycler.RapidfireAgainst = map[string]int{"EspionageProbe": 5, "SolarSatellite": 5}
	return recycler
}
