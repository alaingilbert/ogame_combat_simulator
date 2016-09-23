package units

import (
	"fmt"
)

type Price struct {
	Metal     int
	Crystal   int
	Deuterium int
}

type CombatUnit struct {
	Price            Price
	OgameID          int
	Name             string
	HullPlating      int
	Shield           int
	ShieldPower      int
	WeaponPower      int
	ArmourTechnology int
	WeaponTechnology int
	ShieldTechnology int
	RapidfireFrom    map[string]int
	RapidfireAgainst map[string]int
}

func (ship *CombatUnit) GetPrice() Price {
	return ship.Price
}

func (ship *CombatUnit) GetRapidfireAgainst(name string) int {
	return ship.RapidfireAgainst[name]
}

func (ship *CombatUnit) InitHullPlating() {
	ship.HullPlating = (1 + (ship.ArmourTechnology / 10)) * ((ship.Price.Metal + ship.Price.Crystal) / 10)
}

func (ship *CombatUnit) GetInitialHullPlating() int {
	return (1 + (ship.ArmourTechnology / 10)) * ((ship.Price.Metal + ship.Price.Crystal) / 10)
}

func (ship *CombatUnit) GetHullPlating() int {
	return ship.HullPlating
}

func (ship *CombatUnit) SetHullPlating(hullPlating int) {
	ship.HullPlating = hullPlating
}

func (ship *CombatUnit) GetName() string {
	return ship.Name
}

func (ship *CombatUnit) GetWeapon() int {
	return ship.WeaponPower
}

func (ship *CombatUnit) GetShield() int {
	return ship.Shield
}

func (ship *CombatUnit) SetShield(shield int) {
	ship.Shield = shield
}

func (ship *CombatUnit) RestoreShield() {
	ship.Shield = ship.ShieldPower * (1 + 0.1*0)
}

func (ship *CombatUnit) String() string {
	return fmt.Sprintf("%s with %d:%d:%d", ship.GetName(), ship.GetHullPlating(), ship.GetShield(), ship.GetWeapon())
}
