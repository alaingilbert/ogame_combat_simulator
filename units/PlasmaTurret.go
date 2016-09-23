package units

type PlasmaTurret struct {
	CombatUnit
}

func NewPlasmaTurret() *PlasmaTurret {
	plasmaTurret := new(PlasmaTurret)
	plasmaTurret.OgameID = 406
	plasmaTurret.Name = "PlasmaTurret"
	plasmaTurret.Price = Price{50000, 50000, 30000}
	plasmaTurret.ShieldPower = 300
	plasmaTurret.WeaponPower = 3000
	plasmaTurret.InitHullPlating()
	plasmaTurret.RestoreShield()
	return plasmaTurret
}
