package units

type LightFighter struct {
	Ship
}

func NewLightFighter() *LightFighter {
	lf := new(LightFighter)
	lf.OgameID = 204
	lf.Name = "LightFighter"
	lf.Price = Price{3000, 1000, 0}
	lf.ShieldPower = 10
	lf.WeaponPower = 50
	lf.InitHullPlating()
	lf.RestoreShield()
	lf.CargoCapacity = 50
	lf.BaseSpeed = 12500
	lf.FuelComsumption = 20
	lf.RapidfireFrom = map[string]int{"Cruiser": 6, "Deathstar": 200}
	lf.RapidfireAgainst = map[string]int{"EspionageProbe": 5, "SolarSatellite": 5}
	return lf
}
