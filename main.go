package main

import (
	"encoding/json"
	"errors"
	"fmt"
	"io/ioutil"
	"math"
	"math/rand"
	"ogame_combat_simulator/config"
	"ogame_combat_simulator/units"
	"os"
	"strings"
	"time"

	"github.com/BurntSushi/toml"
	"github.com/cheggaaa/pb"
	"github.com/olekukonko/tablewriter"
	"github.com/urfave/cli"
)

func Round(val float64) int {
	if val < 0 {
		return int(val - 0.5)
	}
	return int(val + 0.5)
}

type ICombatUnit interface {
	IsDead() bool
	IsAlive() bool
	GetPrice() units.Price
	GetName() string
	GetInitialHullPlating() int
	GetHullPlating() float64
	SetHullPlating(float64)
	GetShield() float64
	SetShield(float64)
	GetWeapon() float64
	GetRapidfireAgainst(string) int
	RestoreShield()
}

type Entity struct {
	Weapon          int
	Shield          int
	Armour          int
	Combustion      int
	Impulse         int
	Hyperspace      int
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
	TotalUnits      int
	Units           []ICombatUnit
}

func (e *Entity) Init() {
	idx := 0
	for i := 0; i < e.SmallCargo; i++ {
		e.Units[idx] = units.NewSmallCargo()
		idx++
	}
	for i := 0; i < e.LargeCargo; i++ {
		e.Units[idx] = units.NewLargeCargo()
		idx++
	}
	for i := 0; i < e.LightFighter; i++ {
		e.Units[idx] = units.NewLightFighter()
		idx++
	}
	for i := 0; i < e.HeavyFighter; i++ {
		e.Units[idx] = units.NewHeavyFighter()
		idx++
	}
	for i := 0; i < e.Cruiser; i++ {
		e.Units[idx] = units.NewCruiser()
		idx++
	}
	for i := 0; i < e.Battleship; i++ {
		e.Units[idx] = units.NewBattleship()
		idx++
	}
	for i := 0; i < e.ColonyShip; i++ {
		e.Units[idx] = units.NewColonyShip()
		idx++
	}
	for i := 0; i < e.Recycler; i++ {
		e.Units[idx] = units.NewRecycler()
		idx++
	}
	for i := 0; i < e.EspionageProbe; i++ {
		e.Units[idx] = units.NewEspionageProbe()
		idx++
	}
	for i := 0; i < e.Bomber; i++ {
		e.Units[idx] = units.NewBomber()
		idx++
	}
	for i := 0; i < e.SolarSatellite; i++ {
		e.Units[idx] = units.NewSolarSatellite()
		idx++
	}
	for i := 0; i < e.Destroyer; i++ {
		e.Units[idx] = units.NewDestroyer()
		idx++
	}
	for i := 0; i < e.Deathstar; i++ {
		e.Units[idx] = units.NewDeathstar()
		idx++
	}
	for i := 0; i < e.Battlecruiser; i++ {
		e.Units[idx] = units.NewBattlecruiser()
		idx++
	}
	for i := 0; i < e.RocketLauncher; i++ {
		e.Units[idx] = units.NewRocketLauncher()
		idx++
	}
	for i := 0; i < e.LightLaser; i++ {
		e.Units[idx] = units.NewLightLaser()
		idx++
	}
	for i := 0; i < e.HeavyLaser; i++ {
		e.Units[idx] = units.NewHeavyLaser()
		idx++
	}
	for i := 0; i < e.GaussCannon; i++ {
		e.Units[idx] = units.NewGaussCannon()
		idx++
	}
	for i := 0; i < e.IonCannon; i++ {
		e.Units[idx] = units.NewIonCannon()
		idx++
	}
	for i := 0; i < e.PlasmaTurret; i++ {
		e.Units[idx] = units.NewPlasmaTurret()
		idx++
	}
	for i := 0; i < e.SmallShieldDome; i++ {
		e.Units[idx] = units.NewSmallShieldDome()
		idx++
	}
	for i := 0; i < e.LargeShieldDome; i++ {
		e.Units[idx] = units.NewLargeShieldDome()
		idx++
	}
}

func NewEntity() *Entity {
	return new(Entity)
}

type CombatSimulator struct {
	Attacker       Entity
	Defender       Entity
	MaxRounds      int
	Rounds         int
	FleetToDebris  float64
	Winner         string
	IsLogging      bool
	AttackerLosses units.Price
	DefenderLosses units.Price
	Debris         units.Price
}

func (simulator *CombatSimulator) hasExploded(defendingUnit ICombatUnit) bool {
	exploded := false
	hullPercentage := float64(defendingUnit.GetHullPlating()) / float64(defendingUnit.GetInitialHullPlating())
	if hullPercentage <= 0.7 {
		probabilityOfExploding := 1.0 - float64(defendingUnit.GetHullPlating())/float64(defendingUnit.GetInitialHullPlating())
		dice := rand.Float64()
		msg := ""
		if simulator.IsLogging {
			msg += fmt.Sprintf("probability of exploding of %1.3f%%: dice value of %1.3f comparing with %1.3f: ", probabilityOfExploding*100, dice, 1-probabilityOfExploding)
		}
		if dice >= 1-probabilityOfExploding {
			exploded = true
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
	return exploded
}

func (simulator *CombatSimulator) getAnotherShot(unit, targetUnit ICombatUnit) bool {
	rapidFire := true
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
	return rapidFire
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
	if defendingUnit.IsAlive() {
		if simulator.hasExploded(defendingUnit) {
			defendingUnit.SetHullPlating(0)
		}
	}
}

func (simulator *CombatSimulator) unitsFires(attacker Entity, defender Entity) {
	rand.Seed(time.Now().UnixNano())
	for i := 0; i < attacker.TotalUnits; i++ {
		unit := attacker.Units[i]
		rapidFire := true
		for rapidFire {
			if defender.TotalUnits == 0 {
				break
			}
			targetUnit := defender.Units[rand.Intn(defender.TotalUnits)]
			rapidFire = simulator.getAnotherShot(unit, targetUnit)
			if targetUnit.IsAlive() {
				simulator.attack(unit, targetUnit)
			}
		}
	}
}

func (simulator *CombatSimulator) AttackerFires() {
	if simulator.Defender.TotalUnits <= 0 {
		return
	}
	simulator.unitsFires(simulator.Attacker, simulator.Defender)
}

func (simulator *CombatSimulator) DefenderFires() {
	simulator.unitsFires(simulator.Defender, simulator.Attacker)
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
	l := simulator.Defender.TotalUnits
	for i := l - 1; i >= 0; i-- {
		unit := simulator.Defender.Units[i]
		if unit.GetHullPlating() <= 0 {
			if isShip(unit) {
				simulator.Debris.Metal += int(simulator.FleetToDebris * float64(unit.GetPrice().Metal))
				simulator.Debris.Crystal += int(simulator.FleetToDebris * float64(unit.GetPrice().Crystal))
			}
			simulator.DefenderLosses.Add(unit.GetPrice())
			simulator.Defender.Units[i] = simulator.Defender.Units[simulator.Defender.TotalUnits-1]
			simulator.Defender.TotalUnits--
			//simulator.Defender.Units = simulator.Defender.Units[:len(simulator.Defender.Units)-1]
			if simulator.IsLogging {
				fmt.Println(fmt.Sprintf("%s lost all its integrity, remove from battle", unit.GetName()))
			}
		}
	}
	l = simulator.Attacker.TotalUnits
	for i := l - 1; i >= 0; i-- {
		unit := simulator.Attacker.Units[i]
		if unit.GetHullPlating() <= 0 {
			if isShip(unit) {
				simulator.Debris.Metal += int(simulator.FleetToDebris * float64(unit.GetPrice().Metal))
				simulator.Debris.Crystal += int(simulator.FleetToDebris * float64(unit.GetPrice().Crystal))
			}
			simulator.AttackerLosses.Add(unit.GetPrice())
			simulator.Attacker.Units[i] = simulator.Attacker.Units[simulator.Attacker.TotalUnits-1]
			simulator.Attacker.TotalUnits--
			//simulator.Attacker.Units = simulator.Attacker.Units[:len(simulator.Attacker.Units)-1]
			if simulator.IsLogging {
				fmt.Println(fmt.Sprintf("%s lost all its integrity, remove from battle", unit.GetName()))
			}
		}
	}
}

func (simulator *CombatSimulator) RestoreShields() {
	for i := 0; i < simulator.Attacker.TotalUnits; i++ {
		unit := simulator.Attacker.Units[i]
		unit.RestoreShield()
		if simulator.IsLogging {
			fmt.Println(fmt.Sprintf("%s still has integrity, restore its shield", unit.GetName()))
		}
	}
	for i := 0; i < simulator.Defender.TotalUnits; i++ {
		unit := simulator.Defender.Units[i]
		unit.RestoreShield()
		if simulator.IsLogging {
			fmt.Println(fmt.Sprintf("%s still has integrity, restore its shield", unit.GetName()))
		}
	}
}

func (simulator *CombatSimulator) IsCombatDone() bool {
	return simulator.Attacker.TotalUnits <= 0 || simulator.Defender.TotalUnits <= 0
}

func (simulator *CombatSimulator) GetMoonchance() int {
	debris := float64(simulator.Debris.Metal) + float64(simulator.Debris.Crystal)
	return int(math.Min(debris/100000.0, 20.0))
}

func (simulator *CombatSimulator) PrintWinner() {
	if simulator.Defender.TotalUnits <= 0 && simulator.Attacker.TotalUnits <= 0 {
		simulator.Winner = "draw"
		if simulator.IsLogging {
			fmt.Println("The battle ended draw.")
		}
	} else if simulator.Attacker.TotalUnits <= 0 {
		simulator.Winner = "defender"
		if simulator.IsLogging {
			fmt.Println(fmt.Sprintf("The battle ended after %d rounds with %s winning", simulator.Rounds, simulator.Winner))
		}
	} else if simulator.Defender.TotalUnits <= 0 {
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

func NewCombatSimulator(attacker *Entity, defender *Entity) *CombatSimulator {
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
	Workers     int
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

func printResult(result map[string]interface{}) {
	fmt.Println(fmt.Sprintf("| Results (%d simulations | ~%d rounds)", result["simulations"], result["rounds"]))
	table := tablewriter.NewWriter(os.Stdout)
	table.SetBorders(tablewriter.Border{Left: true, Top: true, Right: true, Bottom: true})
	data2 := [][]string{
		[]string{"Attackers win", fmt.Sprintf("%d%%", result["attacker_win"])},
		[]string{"Defenders win", fmt.Sprintf("%d%%", result["defender_win"])},
		[]string{"Draw", fmt.Sprintf("%d%%", result["draw"])},
	}
	table.AppendBulk(data2)
	table.Render()

	fmt.Println("")
	table = tablewriter.NewWriter(os.Stdout)
	table.SetHeader([]string{"", "Metal", "Crystal", "Deuterium", "Recycler", "Moonchance"})
	table.SetBorders(tablewriter.Border{Left: true, Top: true, Right: true, Bottom: true})
	data1 := [][]string{
		[]string{
			"Attacker losses",
			fmt.Sprintf("%d", result["attacker_losses"].(map[string]int)["metal"]),
			fmt.Sprintf("%d", result["attacker_losses"].(map[string]int)["crystal"]),
			fmt.Sprintf("%d", result["attacker_losses"].(map[string]int)["deuterium"]),
			"", ""},
		[]string{
			"Defender losses",
			fmt.Sprintf("%d", result["defender_losses"].(map[string]int)["metal"]),
			fmt.Sprintf("%d", result["defender_losses"].(map[string]int)["crystal"]),
			fmt.Sprintf("%d", result["defender_losses"].(map[string]int)["deuterium"]),
			"", ""},
		[]string{
			"Debris",
			fmt.Sprintf("%d", result["debris"].(map[string]int)["metal"]),
			fmt.Sprintf("%d", result["debris"].(map[string]int)["crystal"]),
			"",
			fmt.Sprintf("%d", result["recycler"]),
			fmt.Sprintf("%d", result["moonchance"])},
	}
	table.AppendBulk(data1)
	table.Render()
}

func (e *Entity) Reset() {
	e.TotalUnits = 0
	e.TotalUnits += e.SmallCargo
	e.TotalUnits += e.SmallCargo
	e.TotalUnits += e.LargeCargo
	e.TotalUnits += e.LightFighter
	e.TotalUnits += e.HeavyFighter
	e.TotalUnits += e.Cruiser
	e.TotalUnits += e.Battleship
	e.TotalUnits += e.ColonyShip
	e.TotalUnits += e.Recycler
	e.TotalUnits += e.EspionageProbe
	e.TotalUnits += e.Bomber
	e.TotalUnits += e.SolarSatellite
	e.TotalUnits += e.Destroyer
	e.TotalUnits += e.Deathstar
	e.TotalUnits += e.Battlecruiser
	e.TotalUnits += e.RocketLauncher
	e.TotalUnits += e.LightLaser
	e.TotalUnits += e.HeavyLaser
	e.TotalUnits += e.GaussCannon
	e.TotalUnits += e.IonCannon
	e.TotalUnits += e.PlasmaTurret
	e.TotalUnits += e.SmallShieldDome
	e.TotalUnits += e.LargeShieldDome
}

func start(c *cli.Context) error {
	configPath := c.String("config")
	jsonOutput := c.Bool("json")

	if _, err := os.Stat(configPath); os.IsNotExist(err) {
		return errors.New("Config file not found")
	}

	var conf Config
	if _, err := toml.DecodeFile(configPath, &conf); err != nil {
		fmt.Println("Unable to decode config file", err)
		return err
	}

	nbSimulations := conf.Simulations

	bar := pb.New(nbSimulations)
	bar.ShowTimeLeft = false
	bar.SetWidth(80)
	if !jsonOutput {
		fmt.Println("Simulations:")
		bar.Start()
	}

	attackerWin := 0
	defenderWin := 0
	draw := 0
	attackerLosses := units.Price{}
	defenderLosses := units.Price{}
	debris := units.Price{}
	rounds := 0
	moonchance := 0

	attacker := NewEntity()
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
	attacker.SolarSatellite = 0
	attacker.Destroyer = conf.Attacker.Destroyer
	attacker.Deathstar = conf.Attacker.Deathstar
	attacker.Battlecruiser = conf.Attacker.Battlecruiser
	attacker.RocketLauncher = 0
	attacker.LightLaser = 0
	attacker.HeavyLaser = 0
	attacker.GaussCannon = 0
	attacker.IonCannon = 0
	attacker.PlasmaTurret = 0
	attacker.SmallShieldDome = 0
	attacker.LargeShieldDome = 0
	attacker.Reset()
	attacker.Units = make([]ICombatUnit, attacker.TotalUnits+1, attacker.TotalUnits+1)

	defender := NewEntity()
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
	defender.SolarSatellite = conf.Defender.SolarSatellite
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
	defender.Reset()
	defender.Units = make([]ICombatUnit, defender.TotalUnits+1, defender.TotalUnits+1)

	cs := NewCombatSimulator(attacker, defender)
	cs.IsLogging = conf.IsLogging

	for i := 0; i < nbSimulations; i++ {
		attacker.Reset()
		defender.Reset()

		cs.Simulate()
		bar.Increment()

		if cs.Winner == "attacker" {
			attackerWin++
		} else if cs.Winner == "defender" {
			defenderWin++
		} else {
			draw++
		}
		attackerLosses.Add(cs.AttackerLosses)
		defenderLosses.Add(cs.DefenderLosses)
		debris.Add(cs.Debris)
		rounds += cs.Rounds
		moonchance += cs.GetMoonchance()
	}

	if !jsonOutput {
		bar.Finish()
		fmt.Println("")
	}

	result := map[string]interface{}{}
	result["simulations"] = nbSimulations
	result["attacker_win"] = Round(float64(attackerWin) / float64(nbSimulations) * 100)
	result["defender_win"] = Round(float64(defenderWin) / float64(nbSimulations) * 100)
	result["draw"] = Round(float64(draw) / float64(nbSimulations) * 100)
	result["rounds"] = Round(float64(rounds) / float64(nbSimulations))
	result["attacker_losses"] = map[string]int{}
	result["attacker_losses"].(map[string]int)["metal"] = int(float64(attackerLosses.Metal) / float64(nbSimulations))
	result["attacker_losses"].(map[string]int)["crystal"] = int(float64(attackerLosses.Crystal) / float64(nbSimulations))
	result["attacker_losses"].(map[string]int)["deuterium"] = int(float64(attackerLosses.Deuterium) / float64(nbSimulations))
	result["defender_losses"] = map[string]int{}
	result["defender_losses"].(map[string]int)["metal"] = int(float64(defenderLosses.Metal) / float64(nbSimulations))
	result["defender_losses"].(map[string]int)["crystal"] = int(float64(defenderLosses.Crystal) / float64(nbSimulations))
	result["defender_losses"].(map[string]int)["deuterium"] = int(float64(defenderLosses.Deuterium) / float64(nbSimulations))
	result["debris"] = map[string]int{}
	result["debris"].(map[string]int)["metal"] = int(float64(debris.Metal) / float64(nbSimulations))
	result["debris"].(map[string]int)["crystal"] = int(float64(debris.Crystal) / float64(nbSimulations))
	result["recycler"] = int(math.Ceil((float64(debris.Metal+debris.Crystal) / float64(nbSimulations)) / 20000.0))
	result["moonchance"] = int(float64(moonchance) / float64(nbSimulations))

	if jsonOutput {
		b, err := json.MarshalIndent(result, "", "  ")
		if err != nil {
			fmt.Println("Failed to json marshal the result", err)
			return err
		}
		fmt.Println(string(b))
	} else {
		printResult(result)
	}

	return nil
}

func main() {
	app := cli.NewApp()
	app.EnableBashCompletion = true
	app.Flags = []cli.Flag{
		cli.StringFlag{
			Name:   "config, c",
			Value:  "config.toml",
			Usage:  "Path to config file",
			EnvVar: "CONFIG_PATH",
		},
		cli.BoolFlag{
			Name:   "json, j",
			Usage:  "Output json",
			EnvVar: "JSON_OUTPUT",
		},
	}
	app.Name = "OGame Combat Simulator"
	app.Usage = "This is usage"
	app.Version = "0.0.1"
	app.Authors = []cli.Author{
		cli.Author{
			Name:  "Alain Gilbert",
			Email: "alain.gilbert.15@gmail.com",
		},
	}
	app.Commands = []cli.Command{
		{
			Name:  "generate_config",
			Usage: "Generate config file",
			Action: func(c *cli.Context) error {
				if _, err := os.Stat("config.toml"); err == nil {
					fmt.Println("config.toml already exists")
					return nil
				}
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
