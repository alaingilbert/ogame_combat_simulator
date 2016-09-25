from units.Deathstar import Deathstar
from units.RocketLauncher import RocketLauncher
from units.Units import Price
import random

class Entity(object):
    Weapon         = 0
    Shield         = 0
    Armour         = 0
    SmallCargo     = 0
    LargeCargo     = 0
    LightFighter   = 0
    HeavyFighter   = 0
    Cruiser        = 0
    Battleship     = 0
    ColonyShip     = 0
    Recycler       = 0
    EspionageProbe = 0
    Bomber         = 0
    SolarSatellite = 0
    Destroyer      = 0
    Deathstar      = 0
    Battlecruiser  = 0
    Units          = []

    def Init(self):
        for i in range(self.SmallCargo):
            self.Units.append(units.SmallCargo())
        #for i := 0; i < e.LargeCargo; i++ {
        #    e.Units = append(e.Units, units.NewLargeCargo())
        #}
        #for i := 0; i < e.LightFighter; i++ {
        #    e.Units = append(e.Units, units.NewLightFighter())
        #}
        #for i := 0; i < e.HeavyFighter; i++ {
        #    e.Units = append(e.Units, units.NewHeavyFighter())
        #}
        #for i := 0; i < e.Cruiser; i++ {
        #    e.Units = append(e.Units, units.NewCruiser())
        #}
        #for i := 0; i < e.Battleship; i++ {
        #    e.Units = append(e.Units, units.NewBattleship())
        #}
        #for i := 0; i < e.ColonyShip; i++ {
        #    e.Units = append(e.Units, units.NewColonyShip())
        #}
        #for i := 0; i < e.Recycler; i++ {
        #    e.Units = append(e.Units, units.NewRecycler())
        #}
        #for i := 0; i < e.EspionageProbe; i++ {
        #    e.Units = append(e.Units, units.NewEspionageProbe())
        #}
        #for i := 0; i < e.Bomber; i++ {
        #    e.Units = append(e.Units, units.NewBomber())
        #}
        #for i := 0; i < e.SolarSatellite; i++ {
        #    e.Units = append(e.Units, units.NewSolarSatellite())
        #}
        #for i := 0; i < e.Destroyer; i++ {
        #    e.Units = append(e.Units, units.NewDestroyer())
        #}
        for i in range(self.Deathstar):
            self.Units.append(Deathstar())
        #for i := 0; i < e.Battlecruiser; i++ {
        #    e.Units = append(e.Units, units.NewBattlecruiser())


class Attacker(Entity):
    Combustion = 0
    Impulse    = 0
    Hyperspace = 0

class Defender(Entity):
    RocketLauncher  = 0
    LightLaser      = 0
    HeavyLaser      = 0
    GaussCannon     = 0
    IonCannon       = 0
    PlasmaTurret    = 0
    SmallShieldDome = 0
    LargeShieldDome = 0

    def Init(self):
        super(Defender, self).Init()
        for i in range(self.RocketLauncher):
            self.Units.append(RocketLauncher())
        #for i := 0; i < e.LightLaser; i++ {
        #    e.Units = append(e.Units, units.NewLightLaser())
        #}
        #for i := 0; i < e.HeavyLaser; i++ {
        #    e.Units = append(e.Units, units.NewHeavyLaser())
        #}
        #for i := 0; i < e.GaussCannon; i++ {
        #    e.Units = append(e.Units, units.NewGaussCannon())
        #}
        #for i := 0; i < e.IonCannon; i++ {
        #    e.Units = append(e.Units, units.NewIonCannon())
        #}
        #for i := 0; i < e.PlasmaTurret; i++ {
        #    e.Units = append(e.Units, units.NewPlasmaTurret())
        #}
        #for i := 0; i < e.SmallShieldDome; i++ {
        #    e.Units = append(e.Units, units.NewSmallShieldDome())
        #}
        #for i := 0; i < e.LargeShieldDome; i++ {
        #    e.Units = append(e.Units, units.NewLargeShieldDome())


def isShip(unit):
    return True
    #switch unit.(type) {
    #case *units.SmallCargo, *units.LargeCargo, *units.LightFighter,
    #    *units.HeavyFighter, *units.Cruiser, *units.Battleship, *units.ColonyShip,
    #    *units.Recycler, *units.EspionageProbe, *units.Bomber, *units.SolarSatellite,
    #    *units.Destroyer, *units.Deathstar, *units.Battlecruiser:
    #    return true
    #default:
    #    return false


class CombatSimulator(object):
    Attacker       = None
    Defender       = None
    MaxRounds       = 6
    Rounds         = 0
    FleetToDebris  = 0.3
    Winner         = ""
    IsLogging      = False
    AttackerLosses = Price()
    DefenderLosses = Price()
    Debris         = Price()

    def __init__(self, attacker, defender):
        self.Attacker = attacker
        self.Defender = defender

    def hasExploded(self, defendingUnit):
        exploded = False
        hullPercentage = float(defendingUnit.GetHullPlating()) / float(defendingUnit.GetInitialHullPlating())
        if hullPercentage <= 0.7:
            probabilityOfExploding = 1.0 - float(defendingUnit.GetHullPlating())/float(defendingUnit.GetInitialHullPlating())
            dice = random.uniform(0, 1)
            msg = ""
            if self.IsLogging:
                msg += "probability of exploding of %1.3f%%: dice value of %1.3f comparing with %1.3f: " % (probabilityOfExploding*100, dice, 1-probabilityOfExploding)
            if dice >= 1-probabilityOfExploding:
                exploded = True
                if self.IsLogging:
                    msg += "unit exploded."
            else:
                if self.IsLogging:
                    msg += "unit didn't explode."
            if self.IsLogging:
                fmt.Println(msg)
        return exploded

    def getAnotherShot(self, unit, targetUnit):
        rapidFire = True
        rf = unit.GetRapidfireAgainst(targetUnit.GetName())
        msg = ""
        if rf > 0:
            chance = float(rf-1) / float(rf)
            dice = random.uniform(0, 1)
            if self.IsLogging:
                msg += "dice was %1.3f, comparing with %1.3f: " % (dice, chance)
            if dice <= chance:
                if self.IsLogging:
                    msg += "%s gets another shot." % unit.GetName()
            else:
                if self.IsLogging:
                    msg += "%s does not get another shot." % unit.GetName()
                rapidFire = False
        else:
            if self.IsLogging:
                msg += "%s doesn't have rapid fire against %s." % (unit.GetName(), targetUnit.GetName())
            rapidFire = False
        if self.IsLogging:
            print(msg)
        return rapidFire

    def attack(self, attackingUnit, defendingUnit):
        msg = ""
        if self.IsLogging:
            msg += "%s fires at %s; " % (attackingUnit, defendingUnit)
        # Check for shot bounce
        if float(attackingUnit.GetWeapon()) < 0.01*float(defendingUnit.GetShield()):
            if self.IsLogging:
                msg += "shot bounced"
                print(msg)
            return

        # Attack target
        weapon = attackingUnit.GetWeapon()
        if defendingUnit.GetShield() < weapon:
            weapon -= defendingUnit.GetShield()
            defendingUnit.SetShield(0)
            defendingUnit.SetHullPlating(defendingUnit.GetHullPlating() - weapon)
        else:
            defendingUnit.SetShield(defendingUnit.GetShield() - weapon)
        if self.IsLogging:
            msg += "result is %s" % defendingUnit
            print(msg)

        # Check for explosion
        if defendingUnit.IsAlive():
            if self.hasExploded(defendingUnit):
                defendingUnit.SetHullPlating(0)

    def unitsFires(self, attackingUnits, defendingUnits):
        for unit in attackingUnits:
            rapidFire = True
            while rapidFire:
                targetUnit = random.choice(defendingUnits)
                rapidFire = self.getAnotherShot(unit, targetUnit)
                if targetUnit.IsAlive():
                    self.attack(unit, targetUnit)

    def AttackerFires(self):
        if len(self.Defender.Units) == 0:
            return
        self.unitsFires(self.Attacker.Units, self.Defender.Units)

    def DefenderFires(self):
        self.unitsFires(self.Defender.Units, self.Attacker.Units)

    def RemoveDestroyedUnits(self):
        for unit in self.Defender.Units:
            if unit.GetHullPlating() <= 0:
                if isShip(unit):
                    self.Debris.Metal += int(self.FleetToDebris * float(unit.GetPrice().Metal))
                    self.Debris.Crystal += int(self.FleetToDebris * float(unit.GetPrice().Crystal))
                self.DefenderLosses.Add(unit.GetPrice())
        self.Defender.Units = [unit for unit in self.Defender.Units if unit.GetHullPlating() > 0]
        for unit in self.Attacker.Units:
            if unit.GetHullPlating() <= 0:
                if isShip(unit):
                    self.Debris.Metal += int(self.FleetToDebris * float(unit.GetPrice().Metal))
                    self.Debris.Crystal += int(self.FleetToDebris * float(unit.GetPrice().Crystal))
                self.AttackerLosses.Add(unit.GetPrice())
        self.Attacker.Units = [unit for unit in self.Attacker.Units if unit.GetHullPlating() > 0]

    def RestoreShields(self):
        for unit in self.Attacker.Units:
            unit.RestoreShield()
            if self.IsLogging:
                fmt.Println(fmt.Sprintf("%s still has integrity, restore its shield", unit.GetName()))
        for unit in self.Defender.Units:
            unit.RestoreShield()
            if self.IsLogging:
                fmt.Println(fmt.Sprintf("%s still has integrity, restore its shield", unit.GetName()))

    def IsCombatDone(self):
        return len(self.Attacker.Units) == 0 or len(self.Defender.Units) == 0

    def GetMoonchance(self):
        debris = float(self.Debris.Metal) + float(self.Debris.Crystal)
        return int(math.Min(debris/100000.0, 20.0))

    def PrintWinner(self):
        if len(self.Attacker.Units) == 0:
            self.Winner = "defender"
            if self.IsLogging:
                print("The battle ended after %d rounds with %s winning" % (self.Rounds, self.Winner))
        elif len(self.Defender.Units) == 0:
            self.Winner = "attacker"
            if self.IsLogging:
                print("The battle ended after %d rounds with %s winning" % (self.Rounds, self.Winner))
        else:
            self.Winner = "draw"
            if self.IsLogging:
                print("The battle ended draw.")

    def Simulate(self):
        self.Attacker.Init()
        print("1")
        self.Defender.Init()
        print("2")
        for currentRound in range(self.MaxRounds):
            self.Rounds = currentRound
            if self.IsLogging:
                fmt.Println(strings.Repeat("-", 80))
                fmt.Println("ROUND ", currentRound)
                fmt.Println(strings.Repeat("-", 80))
            self.AttackerFires()
            print("3")
            self.DefenderFires()
            print("4")
            self.RemoveDestroyedUnits()
            print("5")
            self.RestoreShields()
            print("6")
            if self.IsCombatDone():
                break
        self.PrintWinner()

attacker = Attacker()
attacker.Deathstar = 10000

defender = Defender()
defender.RocketLauncher = 1000000

cs = CombatSimulator(attacker, defender)
cs.Simulate()

#def printResult(result):
#    fmt.Println(fmt.Sprintf("| Results (%d simulations | ~%d rounds)", result["simulations"], result["rounds"]))
#    table := tablewriter.NewWriter(os.Stdout)
#    table.SetBorders(tablewriter.Border{Left: true, Top: true, Right: true, Bottom: true})
#    data2 := [][]string{
#        []string{"Attackers win", fmt.Sprintf("%d%%", result["attacker_win"])},
#        []string{"Defenders win", fmt.Sprintf("%d%%", result["defender_win"])},
#        []string{"Draw", fmt.Sprintf("%d%%", result["draw"])},
#    }
#    table.AppendBulk(data2)
#    table.Render()
#
#    fmt.Println("")
#    table = tablewriter.NewWriter(os.Stdout)
#    table.SetHeader([]string{"", "Metal", "Crystal", "Deuterium", "Recycler", "Moonchance"})
#    table.SetBorders(tablewriter.Border{Left: true, Top: true, Right: true, Bottom: true})
#    data1 := [][]string{
#        []string{
#            "Attacker losses",
#            fmt.Sprintf("%d", result["attacker_losses"].(map[string]int)["metal"]),
#            fmt.Sprintf("%d", result["attacker_losses"].(map[string]int)["crystal"]),
#            fmt.Sprintf("%d", result["attacker_losses"].(map[string]int)["deuterium"]),
#            "", ""},
#        []string{
#            "Defender losses",
#            fmt.Sprintf("%d", result["defender_losses"].(map[string]int)["metal"]),
#            fmt.Sprintf("%d", result["defender_losses"].(map[string]int)["crystal"]),
#            fmt.Sprintf("%d", result["defender_losses"].(map[string]int)["deuterium"]),
#            "", ""},
#        []string{
#            "Debris",
#            fmt.Sprintf("%d", result["debris"].(map[string]int)["metal"]),
#            fmt.Sprintf("%d", result["debris"].(map[string]int)["crystal"]),
#            "",
#            fmt.Sprintf("%d", result["recycler"]),
#            fmt.Sprintf("%d", result["moonchance"])},
#    }
#    table.AppendBulk(data1)
#    table.Render()
#}

#func start(c *cli.Context) error {
#    configPath := c.String("config")
#    jsonOutput := c.Bool("json")
#
#    if _, err := os.Stat(configPath); os.IsNotExist(err) {
#        return errors.New("Config file not found")
#    }
#
#    var conf Config
#    if _, err := toml.DecodeFile(configPath, &conf); err != nil {
#        fmt.Println("Unable to decode config file", err)
#        return err
#    }
#
#    nbSimulations := conf.Simulations
#    nbWorkers := conf.Workers
#    workChan := make(chan *CombatSimulator, nbSimulations)
#    resChan := make(chan *CombatSimulator, nbSimulations)
#
#    for i := 0; i < nbWorkers; i++ {
#        go func(ch chan *CombatSimulator) {
#            for cs := range ch {
#                cs.Simulate()
#                resChan <- cs
#            }
#        }(workChan)
#    }
#
#    for i := 0; i < nbSimulations; i++ {
#        attacker := Attacker()
#        attacker.Weapon = conf.Attacker.Weapon
#        attacker.Shield = conf.Attacker.Shield
#        attacker.Armour = conf.Attacker.Armour
#        attacker.SmallCargo = conf.Attacker.SmallCargo
#        attacker.LargeCargo = conf.Attacker.LargeCargo
#        attacker.LightFighter = conf.Attacker.LightFighter
#        attacker.HeavyFighter = conf.Attacker.HeavyFighter
#        attacker.Cruiser = conf.Attacker.Cruiser
#        attacker.Battleship = conf.Attacker.Battleship
#        attacker.ColonyShip = conf.Attacker.ColonyShip
#        attacker.Recycler = conf.Attacker.Recycler
#        attacker.EspionageProbe = conf.Attacker.EspionageProbe
#        attacker.Bomber = conf.Attacker.Bomber
#        attacker.Destroyer = conf.Attacker.Destroyer
#        attacker.Deathstar = conf.Attacker.Deathstar
#        attacker.Battlecruiser = conf.Attacker.Battlecruiser
#
#        defender := Defender()
#        defender.Weapon = conf.Defender.Weapon
#        defender.Shield = conf.Defender.Shield
#        defender.Armour = conf.Defender.Armour
#        defender.SmallCargo = conf.Defender.SmallCargo
#        defender.LargeCargo = conf.Defender.LargeCargo
#        defender.LightFighter = conf.Defender.LightFighter
#        defender.HeavyFighter = conf.Defender.HeavyFighter
#        defender.Cruiser = conf.Defender.Cruiser
#        defender.Battleship = conf.Defender.Battleship
#        defender.ColonyShip = conf.Defender.ColonyShip
#        defender.Recycler = conf.Defender.Recycler
#        defender.EspionageProbe = conf.Defender.EspionageProbe
#        defender.Bomber = conf.Defender.Bomber
#        defender.SolarSatellite = conf.Defender.SolarSatellite
#        defender.Destroyer = conf.Defender.Destroyer
#        defender.Deathstar = conf.Defender.Deathstar
#        defender.Battlecruiser = conf.Defender.Battlecruiser
#        defender.RocketLauncher = conf.Defender.RocketLauncher
#        defender.LightLaser = conf.Defender.LightLaser
#        defender.HeavyLaser = conf.Defender.HeavyLaser
#        defender.GaussCannon = conf.Defender.GaussCannon
#        defender.IonCannon = conf.Defender.IonCannon
#        defender.PlasmaTurret = conf.Defender.PlasmaTurret
#        defender.SmallShieldDome = conf.Defender.SmallShieldDome
#        defender.LargeShieldDome = conf.Defender.LargeShieldDome
#
#        cs := CombatSimulator(attacker, defender)
#        cs.IsLogging = conf.IsLogging
#
#        workChan <- cs
#    }
#
#    bar := pb.New(nbSimulations)
#    bar.ShowTimeLeft = false
#    bar.SetWidth(80)
#    if !jsonOutput {
#        fmt.Println("Simulations:")
#        bar.Start()
#    }
#
#    attackerWin := 0
#    defenderWin := 0
#    draw := 0
#    attackerLosses := units.Price{}
#    defenderLosses := units.Price{}
#    debris := units.Price{}
#    rounds := 0
#    moonchance := 0
#
#    for i := 0; i < nbSimulations; i++ {
#        cs := <-resChan
#        bar.Increment()
#        if cs.Winner == "attacker" {
#            attackerWin++
#        } else if cs.Winner == "defender" {
#            defenderWin++
#        } else {
#            draw++
#        }
#        attackerLosses.Add(cs.AttackerLosses)
#        defenderLosses.Add(cs.DefenderLosses)
#        debris.Add(cs.Debris)
#        rounds += cs.Rounds
#        moonchance += cs.GetMoonchance()
#    }
#    if !jsonOutput {
#        bar.Finish()
#        fmt.Println("")
#    }
#
#    result := map[string]interface{}{}
#    result["simulations"] = nbSimulations
#    result["attacker_win"] = Round(float64(attackerWin) / float64(nbSimulations) * 100)
#    result["defender_win"] = Round(float64(defenderWin) / float64(nbSimulations) * 100)
#    result["draw"] = Round(float64(draw) / float64(nbSimulations) * 100)
#    result["rounds"] = Round(float64(rounds) / float64(nbSimulations))
#    result["attacker_losses"] = map[string]int{}
#    result["attacker_losses"].(map[string]int)["metal"] = int(float64(attackerLosses.Metal) / float64(nbSimulations))
#    result["attacker_losses"].(map[string]int)["crystal"] = int(float64(attackerLosses.Crystal) / float64(nbSimulations))
#    result["attacker_losses"].(map[string]int)["deuterium"] = int(float64(attackerLosses.Deuterium) / float64(nbSimulations))
#    result["defender_losses"] = map[string]int{}
#    result["defender_losses"].(map[string]int)["metal"] = int(float64(defenderLosses.Metal) / float64(nbSimulations))
#    result["defender_losses"].(map[string]int)["crystal"] = int(float64(defenderLosses.Crystal) / float64(nbSimulations))
#    result["defender_losses"].(map[string]int)["deuterium"] = int(float64(defenderLosses.Deuterium) / float64(nbSimulations))
#    result["debris"] = map[string]int{}
#    result["debris"].(map[string]int)["metal"] = int(float64(debris.Metal) / float64(nbSimulations))
#    result["debris"].(map[string]int)["crystal"] = int(float64(debris.Crystal) / float64(nbSimulations))
#    result["recycler"] = int(math.Ceil((float64(debris.Metal+debris.Crystal) / float64(nbSimulations)) / 20000.0))
#    result["moonchance"] = int(float64(moonchance) / float64(nbSimulations))
#
#    #if jsonOutput {
#    #    b, err := json.MarshalIndent(result, "", "  ")
#    #    if err != nil {
#    #        fmt.Println("Failed to json marshal the result", err)
#    #        return err
#    #    }
#    #    fmt.Println(string(b))
#    #} else {
#    #    printResult(result)
#    #}
#
#    return nil
#}
#
#func main() {
#    app := cli.NewApp()
#    app.EnableBashCompletion = true
#    app.Flags = []cli.Flag{
#        cli.StringFlag{
#            Name:   "config, c",
#            Value:  "config.toml",
#            Usage:  "Path to config file",
#            EnvVar: "CONFIG_PATH",
#        },
#        cli.BoolFlag{
#            Name:   "json, j",
#            Usage:  "Output json",
#            EnvVar: "JSON_OUTPUT",
#        },
#    }
#    app.Name = "OGame Combat Simulator"
#    app.Usage = "This is usage"
#    app.Version = "0.0.1"
#    app.Authors = []cli.Author{
#        cli.Author{
#            Name:  "Alain Gilbert",
#            Email: "alain.gilbert.15@gmail.com",
#        },
#    }
#    app.Commands = []cli.Command{
#        {
#            Name:  "generate_config",
#            Usage: "Generate config file",
#            Action: func(c *cli.Context) error {
#                if _, err := os.Stat("config.toml"); err == nil {
#                    fmt.Println("config.toml already exists")
#                    return nil
#                }
#                if err := ioutil.WriteFile("config.toml", config.GenerateConfig(), 0644); err != nil {
#                    return err
#                }
#                fmt.Println("Config file generated")
#                return nil
#            },
#        },
#    }
#    app.Action = start
#    app.Run(os.Args)
#}
