package units

type LightLaser struct {
	CombatUnit
}

func NewLightLaser() *LightLaser {
	lightLaser := new(LightLaser)
	lightLaser.OgameID = 402
	lightLaser.Name = "LightLaser"
	lightLaser.Price = Price{1500, 500, 0}
	lightLaser.ShieldPower = 25
	lightLaser.WeaponPower = 100
	lightLaser.InitHullPlating()
	lightLaser.RestoreShield()
	lightLaser.RapidfireFrom = map[string]int{"Destroyer": 10, "Bomber": 20, "Deathstar": 200}
	return lightLaser
}
