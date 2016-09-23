package main

import (
	"combat_simulator/config"
	"combat_simulator/units"
	"encoding/json"
	"fmt"
	"github.com/BurntSushi/toml"
	"github.com/urfave/cli"
	"io/ioutil"
	"math"
	"math/rand"
	"os"
	"strings"
	"time"
)

func Round(val float64) int {
	if val < 0 {
		return int(val - 0.5)
	}
	return int(val + 0.5)
}

type ICombatUnit interface {
	GetPrice() units.Price
	GetName() string
	GetInitialHullPlating() int
	GetHullPlating() int
	SetHullPlating(int)
	GetShield() int
	SetShield(int)
	GetWeapon() int
	GetRapidfireAgainst(string) int
	RestoreShield()
}

type Entity struct {
	Weapon         int
	Shield         int
	Armour         int
	SmallCargo     int
	LargeCargo     int
	LightFighter   int
	HeavyFighter   int
	Cruiser        int
	Battleship     int
	ColonyShip     int
	Recycler       int
	EspionageProbe int
	Bomber         int
	SolarSatellite int
	Destroyer      int
	Deathstar      int
	Battlecruiser  int
	Units          []ICombatUnit
}

func (e *Entity) Init() {
	for i := 0; i < e.SmallCargo; i++ {
		e.Units = append(e.Units, units.NewSmallCargo())
	}
	for i := 0; i < e.LargeCargo; i++ {
		e.Units = append(e.Units, units.NewLargeCargo())
	}
	for i := 0; i < e.LightFighter; i++ {
		e.Units = append(e.Units, units.NewLightFighter())
	}
	for i := 0; i < e.HeavyFighter; i++ {
		e.Units = append(e.Units, units.NewHeavyFighter())
	}
	for i := 0; i < e.Cruiser; i++ {
		e.Units = append(e.Units, units.NewCruiser())
	}
	for i := 0; i < e.Battleship; i++ {
		e.Units = append(e.Units, units.NewBattleship())
	}
	for i := 0; i < e.ColonyShip; i++ {
		e.Units = append(e.Units, units.NewColonyShip())
	}
	for i := 0; i < e.Recycler; i++ {
		e.Units = append(e.Units, units.NewRecycler())
	}
	for i := 0; i < e.EspionageProbe; i++ {
		e.Units = append(e.Units, units.NewEspionageProbe())
	}
	for i := 0; i < e.Bomber; i++ {
		e.Units = append(e.Units, units.NewBomber())
	}
	for i := 0; i < e.SolarSatellite; i++ {
		e.Units = append(e.Units, units.NewSolarSatellite())
	}
	for i := 0; i < e.Destroyer; i++ {
		e.Units = append(e.Units, units.NewDestroyer())
	}
	for i := 0; i < e.Deathstar; i++ {
		e.Units = append(e.Units, units.NewDeathstar())
	}
	for i := 0; i < e.Battlecruiser; i++ {
		e.Units = append(e.Units, units.NewBattlecruiser())
	}
}

type Attacker struct {
	Entity
	Combustion int
	Impulse    int
	Hyperspace int
}

func NewAttacker() *Attacker {
	attacker := new(Attacker)
	return attacker
}

type Defender struct {
	Entity
	RocketLauncher  int
	LightLaser      int
	HeavyLaser      int
	GaussCannon     int
	IonCannon       int
	PlasmaTurret    int
	SmallShieldDome int
	LargeShieldDome int
}

func (e *Defender) Init() {
	e.Entity.Init()
	for i := 0; i < e.RocketLauncher; i++ {
		e.Units = append(e.Units, units.NewRocketLauncher())
	}
	for i := 0; i < e.LightLaser; i++ {
		e.Units = append(e.Units, units.NewLightLaser())
	}
	for i := 0; i < e.HeavyLaser; i++ {
		e.Units = append(e.Units, units.NewHeavyLaser())
	}
	for i := 0; i < e.GaussCannon; i++ {
		e.Units = append(e.Units, units.NewGaussCannon())
	}
	for i := 0; i < e.IonCannon; i++ {
		e.Units = append(e.Units, units.NewIonCannon())
	}
	for i := 0; i < e.PlasmaTurret; i++ {
		e.Units = append(e.Units, units.NewPlasmaTurret())
	}
	for i := 0; i < e.SmallShieldDome; i++ {
		e.Units = append(e.Units, units.NewSmallShieldDome())
	}
	for i := 0; i < e.LargeShieldDome; i++ {
		e.Units = append(e.Units, units.NewLargeShieldDome())
	}
}

func NewDefender() *Defender {
	defender := new(Defender)
	return defender
}

type CombatSimulator struct {
	Attacker       Attacker
	Defender       Defender
	MaxRounds      int
	Rounds         int
	FleetToDebris  float64
	Winner         string
	IsLogging      bool
	AttackerLosses units.Price
	DefenderLosses units.Price
	Debris         units.Price
}

func (simulator *CombatSimulator) attack(attackingUnit, defendingUnit ICombatUnit) {
	msg := ""
	if simulator.IsLogging {
		msg += fmt.Sprintf("%s fires at %s; ", attackingUnit, defendingUnit)
	}
	// Check for shot bounce
	if float64(attackingUnit.GetWeapon()) < 0.01*float64(defendingUnit.GetShield()) {
		if simulator.IsLogging {
			msg += "shot bounced"
			fmt.Println(msg)
		}
		return
	}

	// Attack target
	weapon := attackingUnit.GetWeapon()
	if defendingUnit.GetShield() < weapon {
		weapon -= defendingUnit.GetShield()
		defendingUnit.SetShield(0)
		defendingUnit.SetHullPlating(defendingUnit.GetHullPlating() - weapon)
	} else {
		defendingUnit.SetShield(defendingUnit.GetShield() - weapon)
	}
	if simulator.IsLogging {
		msg += fmt.Sprintf("result is %s", defendingUnit)
		fmt.Println(msg)
	}

	// Check for explosion
	hullPercentage := float64(defendingUnit.GetHullPlating()) / float64(defendingUnit.GetInitialHullPlating())
	if hullPercentage <= 0.7 && defendingUnit.GetHullPlating() > 0 {
		probabilityOfExploding := 1.0 - float64(defendingUnit.GetHullPlating())/float64(defendingUnit.GetInitialHullPlating())
		dice := rand.Float64()
		msg := ""
		if simulator.IsLogging {
			msg += fmt.Sprintf("probability of exploding of %1.3f%%: dice value of %1.3f comparing with %1.3f: ", probabilityOfExploding*100, dice, 1-probabilityOfExploding)
		}
		if dice >= 1-probabilityOfExploding {
			defendingUnit.SetHullPlating(0)
			if simulator.IsLogging {
				msg += "unit exploded."
			}
		} else {
			if simulator.IsLogging {
				msg += "unit didn't explode."
			}
		}
		if simulator.IsLogging {
			fmt.Println(msg)
		}
	}
}

func (simulator *CombatSimulator) unitsFires(attackingUnits, defendingUnits []ICombatUnit) {
	rand.Seed(time.Now().UnixNano())
	for _, unit := range attackingUnits {
		rapidFire := true
		for rapidFire {
			targetUnit := defendingUnits[rand.Intn(len(defendingUnits))]
			simulator.attack(unit, targetUnit)
			rf := unit.GetRapidfireAgainst(targetUnit.GetName())
			msg := ""
			if rf > 0 {
				chance := float64(rf-1) / float64(rf)
				dice := rand.Float64()
				if simulator.IsLogging {
					msg += fmt.Sprintf("dice was %1.3f, comparing with %1.3f: ", dice, chance)
				}
				if dice <= chance {
					if simulator.IsLogging {
						msg += fmt.Sprintf("%s gets another shot.", unit.GetName())
					}
				} else {
					if simulator.IsLogging {
						msg += fmt.Sprintf("%s does not get another shot.", unit.GetName())
					}
					rapidFire = false
				}
			} else {
				if simulator.IsLogging {
					msg += fmt.Sprintf("%s doesn't have rapid fire against %s.", unit.GetName(), targetUnit.GetName())
				}
				rapidFire = false
			}
			if simulator.IsLogging {
				fmt.Println(msg)
			}
		}
	}
}

func (simulator *CombatSimulator) AttackerFires() {
	if len(simulator.Defender.Units) == 0 {
		return
	}
	simulator.unitsFires(simulator.Attacker.Units, simulator.Defender.Units)
}

func (simulator *CombatSimulator) DefenderFires() {
	simulator.unitsFires(simulator.Defender.Units, simulator.Attacker.Units)
}

func isShip(unit ICombatUnit) bool {
	switch unit.(type) {
	case *units.SmallCargo, *units.LargeCargo, *units.LightFighter,
		*units.HeavyFighter, *units.Cruiser, *units.Battleship, *units.ColonyShip,
		*units.Recycler, *units.EspionageProbe, *units.Bomber, *units.SolarSatellite,
		*units.Destroyer, *units.Deathstar, *units.Battlecruiser:
		return true
	default:
		return false
	}
}

func (simulator *CombatSimulator) RemoveDestroyedUnits() {
	for i := len(simulator.Defender.Units) - 1; i >= 0; i-- {
		unit := simulator.Defender.Units[i]
		if unit.GetHullPlating() <= 0 {
			if isShip(unit) {
				simulator.Debris.Metal += int(simulator.FleetToDebris * float64(unit.GetPrice().Metal))
				simulator.Debris.Crystal += int(simulator.FleetToDebris * float64(unit.GetPrice().Crystal))
			}
			simulator.DefenderLosses.Metal += unit.GetPrice().Metal
			simulator.DefenderLosses.Crystal += unit.GetPrice().Crystal
			simulator.DefenderLosses.Deuterium += unit.GetPrice().Deuterium
			simulator.Defender.Units = append(simulator.Defender.Units[:i], simulator.Defender.Units[i+1:]...)
			if simulator.IsLogging {
				fmt.Println(fmt.Sprintf("%s lost all its integrity, remove from battle", unit.GetName()))
			}
		}
	}
	for i := len(simulator.Attacker.Units) - 1; i >= 0; i-- {
		unit := simulator.Attacker.Units[i]
		if unit.GetHullPlating() <= 0 {
			if isShip(unit) {
				simulator.Debris.Metal += int(simulator.FleetToDebris * float64(unit.GetPrice().Metal))
				simulator.Debris.Crystal += int(simulator.FleetToDebris * float64(unit.GetPrice().Crystal))
			}
			simulator.AttackerLosses.Metal += unit.GetPrice().Metal
			simulator.AttackerLosses.Crystal += unit.GetPrice().Crystal
			simulator.AttackerLosses.Deuterium += unit.GetPrice().Deuterium
			simulator.Attacker.Units = append(simulator.Attacker.Units[:i], simulator.Attacker.Units[i+1:]...)
			if simulator.IsLogging {
				fmt.Println(fmt.Sprintf("%s lost all its integrity, remove from battle", unit.GetName()))
			}
		}
	}
}

func (simulator *CombatSimulator) RestoreShields() {
	for _, unit := range simulator.Attacker.Units {
		unit.RestoreShield()
		if simulator.IsLogging {
			fmt.Println(fmt.Sprintf("%s still has integrity, restore its shield", unit.GetName()))
		}
	}
	for _, unit := range simulator.Defender.Units {
		unit.RestoreShield()
		if simulator.IsLogging {
			fmt.Println(fmt.Sprintf("%s still has integrity, restore its shield", unit.GetName()))
		}
	}
}

func (simulator *CombatSimulator) IsCombatDone() bool {
	return len(simulator.Attacker.Units) == 0 || len(simulator.Defender.Units) == 0
}

func (simulator *CombatSimulator) GetMoonchance() int {
	debris := float64(simulator.Debris.Metal) + float64(simulator.Debris.Crystal)
	return int(math.Min(debris/100000.0, 20.0))
}

func (simulator *CombatSimulator) PrintWinner() {
	if len(simulator.Attacker.Units) == 0 {
		simulator.Winner = "defender"
		if simulator.IsLogging {
			fmt.Println(fmt.Sprintf("The battle ended after %d rounds with %s winning", simulator.Rounds, simulator.Winner))
		}
	} else if len(simulator.Defender.Units) == 0 {
		simulator.Winner = "attacker"
		if simulator.IsLogging {
			fmt.Println(fmt.Sprintf("The battle ended after %d rounds with %s winning", simulator.Rounds, simulator.Winner))
		}
	} else {
		simulator.Winner = "draw"
		if simulator.IsLogging {
			fmt.Println("The battle ended draw.")
		}
	}
}

func (simulator *CombatSimulator) Simulate() {
	simulator.Attacker.Init()
	simulator.Defender.Init()
	for currentRound := 1; currentRound <= simulator.MaxRounds; currentRound++ {
		simulator.Rounds = currentRound
		if simulator.IsLogging {
			fmt.Println(strings.Repeat("-", 80))
			fmt.Println("ROUND ", currentRound)
			fmt.Println(strings.Repeat("-", 80))
		}
		simulator.AttackerFires()
		simulator.DefenderFires()
		simulator.RemoveDestroyedUnits()
		simulator.RestoreShields()
		if simulator.IsCombatDone() {
			break
		}
	}
	simulator.PrintWinner()
}

func NewCombatSimulator(attacker *Attacker, defender *Defender) *CombatSimulator {
	cs := new(CombatSimulator)
	cs.Attacker = *attacker
	cs.Defender = *defender
	cs.IsLogging = false
	cs.MaxRounds = 6
	cs.FleetToDebris = 0.3
	return cs
}

type Config struct {
	IsLogging   bool
	Simulations int
	Attacker    attackerInfo
	Defender    defenderInfo
}

type attackerInfo struct {
	Weapon         int
	Shield         int
	Armour         int
	SmallCargo     int
	LargeCargo     int
	LightFighter   int
	HeavyFighter   int
	Cruiser        int
	Battleship     int
	ColonyShip     int
	Recycler       int
	EspionageProbe int
	Bomber         int
	Destroyer      int
	Deathstar      int
	Battlecruiser  int
}

type defenderInfo struct {
	Weapon          int
	Shield          int
	Armour          int
	SmallCargo      int
	LargeCargo      int
	LightFighter    int
	HeavyFighter    int
	Cruiser         int
	Battleship      int
	ColonyShip      int
	Recycler        int
	EspionageProbe  int
	Bomber          int
	SolarSatellite  int
	Destroyer       int
	Deathstar       int
	Battlecruiser   int
	RocketLauncher  int
	LightLaser      int
	HeavyLaser      int
	GaussCannon     int
	IonCannon       int
	PlasmaTurret    int
	SmallShieldDome int
	LargeShieldDome int
}

func start(c *cli.Context) error {
	var conf Config
	if _, err := toml.DecodeFile("config.toml", &conf); err != nil {
		fmt.Println("Unable to decode config file", err)
		return err
	}

	attacker := NewAttacker()
	attacker.Weapon = conf.Attacker.Weapon
	attacker.Shield = conf.Attacker.Shield
	attacker.Armour = conf.Attacker.Armour
	attacker.SmallCargo = conf.Attacker.SmallCargo
	attacker.LargeCargo = conf.Attacker.LargeCargo
	attacker.LightFighter = conf.Attacker.LightFighter
	attacker.HeavyFighter = conf.Attacker.HeavyFighter
	attacker.Cruiser = conf.Attacker.Cruiser
	attacker.Battleship = conf.Attacker.Battleship
	attacker.ColonyShip = conf.Attacker.ColonyShip
	attacker.Recycler = conf.Attacker.Recycler
	attacker.EspionageProbe = conf.Attacker.EspionageProbe
	attacker.Bomber = conf.Attacker.Bomber
	attacker.Destroyer = conf.Attacker.Destroyer
	attacker.Deathstar = conf.Attacker.Deathstar
	attacker.Battlecruiser = conf.Attacker.Battlecruiser

	defender := NewDefender()
	defender.Weapon = conf.Defender.Weapon
	defender.Shield = conf.Defender.Shield
	defender.Armour = conf.Defender.Armour
	defender.SmallCargo = conf.Defender.SmallCargo
	defender.LargeCargo = conf.Defender.LargeCargo
	defender.LightFighter = conf.Defender.LightFighter
	defender.HeavyFighter = conf.Defender.HeavyFighter
	defender.Cruiser = conf.Defender.Cruiser
	defender.Battleship = conf.Defender.Battleship
	defender.ColonyShip = conf.Defender.ColonyShip
	defender.Recycler = conf.Defender.Recycler
	defender.EspionageProbe = conf.Defender.EspionageProbe
	defender.Bomber = conf.Defender.Bomber
	defender.Destroyer = conf.Defender.Destroyer
	defender.Deathstar = conf.Defender.Deathstar
	defender.Battlecruiser = conf.Defender.Battlecruiser
	defender.RocketLauncher = conf.Defender.RocketLauncher
	defender.LightLaser = conf.Defender.LightLaser
	defender.HeavyLaser = conf.Defender.HeavyLaser
	defender.GaussCannon = conf.Defender.GaussCannon
	defender.IonCannon = conf.Defender.IonCannon
	defender.PlasmaTurret = conf.Defender.PlasmaTurret
	defender.SmallShieldDome = conf.Defender.SmallShieldDome
	defender.LargeShieldDome = conf.Defender.LargeShieldDome

	nbSimulations := conf.Simulations
	attackerWin := 0
	defenderWin := 0
	draw := 0
	attackerLosses := units.Price{}
	defenderLosses := units.Price{}
	debris := units.Price{}
	rounds := 0
	moonchance := 0
	for i := 0; i < nbSimulations; i++ {
		cs := NewCombatSimulator(attacker, defender)
		cs.IsLogging = conf.IsLogging
		cs.Simulate()
		if cs.Winner == "attacker" {
			attackerWin++
		} else if cs.Winner == "defender" {
			defenderWin++
		} else {
			draw++
		}
		attackerLosses.Metal += cs.AttackerLosses.Metal
		attackerLosses.Crystal += cs.AttackerLosses.Crystal
		attackerLosses.Deuterium += cs.AttackerLosses.Deuterium
		defenderLosses.Metal += cs.DefenderLosses.Metal
		defenderLosses.Crystal += cs.DefenderLosses.Crystal
		defenderLosses.Deuterium += cs.DefenderLosses.Deuterium
		debris.Metal += cs.Debris.Metal
		debris.Crystal += cs.Debris.Crystal
		rounds += cs.Rounds
		moonchance += cs.GetMoonchance()
	}

	result := map[string]interface{}{}
	result["simulations"] = nbSimulations
	result["attacker_win"] = float64(attackerWin) / float64(nbSimulations) * 100
	result["defender_win"] = float64(defenderWin) / float64(nbSimulations) * 100
	result["draw"] = float64(draw) / float64(nbSimulations) * 100
	result["rounds"] = float64(rounds) / float64(nbSimulations)
	result["attacker_losses"] = map[string]interface{}{}
	result["attacker_losses"].(map[string]interface{})["metal"] = float64(attackerLosses.Metal) / float64(nbSimulations)
	result["attacker_losses"].(map[string]interface{})["crystal"] = float64(attackerLosses.Crystal) / float64(nbSimulations)
	result["attacker_losses"].(map[string]interface{})["deuterium"] = float64(attackerLosses.Deuterium) / float64(nbSimulations)
	result["defender_losses"] = map[string]interface{}{}
	result["defender_losses"].(map[string]interface{})["metal"] = float64(defenderLosses.Metal) / float64(nbSimulations)
	result["defender_losses"].(map[string]interface{})["crystal"] = float64(defenderLosses.Crystal) / float64(nbSimulations)
	result["defender_losses"].(map[string]interface{})["deuterium"] = float64(defenderLosses.Deuterium) / float64(nbSimulations)
	result["debris"] = map[string]interface{}{}
	result["debris"].(map[string]interface{})["metal"] = float64(debris.Metal) / float64(nbSimulations)
	result["debris"].(map[string]interface{})["crystal"] = float64(debris.Crystal) / float64(nbSimulations)
	result["recycler"] = math.Ceil((float64(debris.Metal+debris.Crystal) / float64(nbSimulations)) / 20000.0)
	result["moonchance"] = float64(moonchance) / float64(nbSimulations)

	b, err := json.MarshalIndent(result, "", "  ")
	if err != nil {
		fmt.Println("Failed to json marshal the result", err)
		return err
	}
	fmt.Println(string(b))
	return nil
}

func main() {
	app := cli.NewApp()
	app.EnableBashCompletion = true
	app.Name = "OGame Combat Simulator"
	app.Usage = "This is usage"
	app.Version = "0.0.1"
	app.Commands = []cli.Command{
		{
			Name:  "generate_config",
			Usage: "Generate config file",
			Action: func(c *cli.Context) error {
				if err := ioutil.WriteFile("config.toml", config.GenerateConfig(), 0644); err != nil {
					return err
				}
				fmt.Println("Config file generated")
				return nil
			},
		},
	}
	app.Action = start
	app.Run(os.Args)
}
