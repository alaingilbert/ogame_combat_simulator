package units

type IonCannon struct {
	CombatUnit
}

func NewIonCannon() *IonCannon {
	ionCannon := new(IonCannon)
	ionCannon.OgameID = 405
	ionCannon.Name = "IonCannon"
	ionCannon.Price = Price{2000, 6000, 0}
	ionCannon.ShieldPower = 500
	ionCannon.WeaponPower = 150
	ionCannon.InitHullPlating()
	ionCannon.RestoreShield()
	ionCannon.RapidfireFrom = map[string]int{"Bomber": 10, "Deathstar": 100}
	return ionCannon
}
