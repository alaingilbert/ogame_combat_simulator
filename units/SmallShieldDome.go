package units

type SmallShieldDome struct {
	CombatUnit
}

func NewSmallShieldDome() *SmallShieldDome {
	smallShieldDome := new(SmallShieldDome)
	smallShieldDome.Name = "SmallShieldDome"
	smallShieldDome.Price = Price{10000, 10000, 0}
	smallShieldDome.ShieldPower = 2000
	smallShieldDome.WeaponPower = 1
	smallShieldDome.InitHullPlating()
	smallShieldDome.RestoreShield()
	return smallShieldDome
}
