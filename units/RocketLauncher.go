package units

type RocketLauncher struct {
	CombatUnit
}

func NewRocketLauncher() *RocketLauncher {
	rocketLauncher := new(RocketLauncher)
	rocketLauncher.OgameID = 401
	rocketLauncher.Name = "RocketLauncher"
	rocketLauncher.Price = Price{2000, 0, 0}
	rocketLauncher.ShieldPower = 20
	rocketLauncher.WeaponPower = 80
	rocketLauncher.InitHullPlating()
	rocketLauncher.RestoreShield()
	rocketLauncher.RapidfireFrom = map[string]int{"Bomber": 20, "Cruiser": 10, "Deathstar": 200}
	return rocketLauncher
}
