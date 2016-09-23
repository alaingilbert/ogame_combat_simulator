package units

type LargeShieldDome struct {
	CombatUnit
}

func NewLargeShieldDome() *LargeShieldDome {
	largeShieldDome := new(LargeShieldDome)
	largeShieldDome.Name = "LargeShieldDome"
	largeShieldDome.Price = Price{50000, 50000, 0}
	largeShieldDome.ShieldPower = 10000
	largeShieldDome.WeaponPower = 1
	largeShieldDome.InitHullPlating()
	largeShieldDome.RestoreShield()
	return largeShieldDome
}
