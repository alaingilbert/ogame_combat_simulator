package units

import (
	"fmt"
)

type Price struct {
	Metal     int
	Crystal   int
	Deuterium int
}

func (p *Price) Add(price Price) {
	p.Metal += price.Metal
	p.Crystal += price.Crystal
	p.Deuterium += price.Deuterium
}

type CombatUnit struct {
	Price            Price
	OgameID          int
	Name             string
	HullPlating      float64
	Shield           float64
	ShieldPower      float64
	WeaponPower      float64
	ArmourTechnology int
	WeaponTechnology int
	ShieldTechnology int
	RapidfireFrom    map[string]int
	RapidfireAgainst map[string]int
}

func (ship *CombatUnit) IsDead() bool {
	return ship.HullPlating <= 0
}

func (ship *CombatUnit) IsAlive() bool {
	return ship.HullPlating > 0
}

func (ship *CombatUnit) GetPrice() Price {
	return ship.Price
}

func (ship *CombatUnit) GetRapidfireAgainst(name string) int {
	return ship.RapidfireAgainst[name]
}

func (ship *CombatUnit) InitHullPlating() {
	ship.HullPlating = (1 + (float64(ship.ArmourTechnology) / 10)) * ((float64(ship.Price.Metal) + float64(ship.Price.Crystal)) / 10)
}

func (ship *CombatUnit) GetInitialHullPlating() int {
	return (1 + (ship.ArmourTechnology / 10)) * ((ship.Price.Metal + ship.Price.Crystal) / 10)
}

func (ship *CombatUnit) GetHullPlating() float64 {
	return ship.HullPlating
}

func (ship *CombatUnit) SetHullPlating(hullPlating float64) {
	ship.HullPlating = hullPlating
}

func (ship *CombatUnit) GetName() string {
	return ship.Name
}

func (ship *CombatUnit) GetWeapon() float64 {
	return ship.WeaponPower
}

func (ship *CombatUnit) GetShield() float64 {
	return ship.Shield
}

func (ship *CombatUnit) SetShield(shield float64) {
	ship.Shield = shield
}

func (ship *CombatUnit) RestoreShield() {
	ship.Shield = ship.ShieldPower * (1 + 0.1*0)
}

func (ship *CombatUnit) String() string {
	return fmt.Sprintf("%s with %d:%d:%d", ship.GetName(), ship.GetHullPlating(), ship.GetShield(), ship.GetWeapon())
}
