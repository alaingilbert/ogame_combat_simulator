package units

type HeavyLaser struct {
	CombatUnit
}

func NewHeavyLaser() *HeavyLaser {
	heavyLaser := new(HeavyLaser)
	heavyLaser.OgameID = 403
	heavyLaser.Name = "HeavyLaser"
	heavyLaser.Price = Price{6000, 2000, 0}
	heavyLaser.ShieldPower = 100
	heavyLaser.WeaponPower = 250
	heavyLaser.InitHullPlating()
	heavyLaser.RestoreShield()
	heavyLaser.RapidfireFrom = map[string]int{"Bomber": 10, "Deathstar": 100}
	return heavyLaser
}
