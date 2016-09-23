package units

type GaussCannon struct {
	CombatUnit
}

func NewGaussCannon() *GaussCannon {
	gaussCannon := new(GaussCannon)
	gaussCannon.OgameID = 404
	gaussCannon.Name = "GaussCannon"
	gaussCannon.Price = Price{20000, 15000, 2000}
	gaussCannon.ShieldPower = 200
	gaussCannon.WeaponPower = 1100
	gaussCannon.InitHullPlating()
	gaussCannon.RestoreShield()
	gaussCannon.RapidfireFrom = map[string]int{"Deathstar": 50}
	return gaussCannon
}
