#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <unistd.h>
#include "./ini.h"
#include "./parson.h"

bool SHOULD_LOG = false;

typedef enum {
  SMALL_CARGO       = 202,
  LARGE_CARGO       = 203,
  LIGHT_FIGHTER     = 204,
  HEAVY_FIGHTER     = 205,
  CRUISER           = 206,
  BATTLESHIP        = 207,
  COLONY_SHIP       = 208,
  RECYCLER          = 209,
  ESPIONAGE_PROBE   = 210,
  BOMBER            = 211,
  SOLAR_SATELLITE   = 212,
  DESTROYER         = 213,
  DEATHSTAR         = 214,
  BATTLECRUISER     = 215,
  ROCKET_LAUNCHER   = 401,
  LIGHT_LASER       = 402,
  HEAVY_LASER       = 403,
  GAUSS_CANNON      = 404,
  ION_CANNON        = 405,
  PLASMA_TURRET     = 406,
  SMALL_SHIELD_DOME = 407,
  LARGE_SHIELD_DOME = 408,
} ships;

typedef struct {
  int Metal;
  int Crystal;
  int Deuterium;
} Price;

typedef struct {
  int Id;
  int OgameID;
  int WeaponPower;
  int ShieldPower;
  int Shield;
  int InitialShield;
  int HullPlating;
  int InitialHullPlating;
  Price Price;
} CombatUnit;

typedef struct {
  // Technos
  int Weapon;
  int Shield;
  int Armour;
  int Combustion;
  int Impulse;
  int Hyperspace;

  // Ships
  int SmallCargo;
  int LargeCargo;
  int LightFighter;
  int HeavyFighter;
  int Cruiser;
  int Battleship;
  int ColonyShip;
  int Recycler;
  int EspionageProbe;
  int Bomber;
  int SolarSatellite;
  int Destroyer;
  int Deathstar;
  int Battlecruiser;

  // Defences
  int RocketLauncher;
  int LightLaser;
  int HeavyLaser;
  int GaussCannon;
  int IonCannon;
  int PlasmaTurret;
  int SmallShieldDome;
  int LargeShieldDome;

  int TotalUnits;
  CombatUnit *Units;
  Price Losses;
} Entity;

typedef struct {
  int Winner;
  int Rounds;
  int MaxRounds;
  float FleetToDebris;
  Entity *Attacker;
  Entity *Defender;
  Price Debris;
} Simulator;

Price NewPrice(int metal, int crystal, int deuterium) {
  Price price;
  price.Metal = metal;
  price.Crystal = crystal;
  price.Deuterium = deuterium;
  return price;
}

CombatUnit NewUnit(Entity *entity, int OgameID) {
  CombatUnit unit;
  unit.Id = 1;
  unit.OgameID = OgameID;
  switch(OgameID) {
    case SMALL_CARGO:
      unit.ShieldPower = 10;
      unit.WeaponPower = 5;
      unit.Price = NewPrice(2000, 2000, 0);
      break;
    case LARGE_CARGO:
      unit.ShieldPower = 25;
      unit.WeaponPower = 5;
      unit.Price = NewPrice(6000, 6000, 0);
      break;
    case LIGHT_FIGHTER:
      unit.ShieldPower = 10;
      unit.WeaponPower = 50;
      unit.Price = NewPrice(3000, 1000, 0);
      break;
    case HEAVY_FIGHTER:
      unit.ShieldPower = 25;
      unit.WeaponPower = 150;
      unit.Price = NewPrice(6000, 4000, 0);
      break;
    case CRUISER:
      unit.ShieldPower = 50;
      unit.WeaponPower = 400;
      unit.Price = NewPrice(20000, 7000, 2000);
      break;
    case BATTLESHIP:
      unit.ShieldPower = 200;
      unit.WeaponPower = 1000;
      unit.Price = NewPrice(45000, 15000, 0);
      break;
    case COLONY_SHIP:
      unit.ShieldPower = 100;
      unit.WeaponPower = 50;
      unit.Price = NewPrice(10000, 20000, 10000);
      break;
    case RECYCLER:
      unit.ShieldPower = 10;
      unit.WeaponPower = 1;
      unit.Price = NewPrice(10000, 6000, 2000);
      break;
    case ESPIONAGE_PROBE:
      unit.ShieldPower = 1; // 0.01
      unit.WeaponPower = 1; // 0.01
      unit.Price = NewPrice(0, 1000, 0);
      break;
    case BOMBER:
      unit.ShieldPower = 500;
      unit.WeaponPower = 1000;
      unit.Price = NewPrice(50000, 25000, 15000);
      break;
    case SOLAR_SATELLITE:
      unit.ShieldPower = 1;
      unit.WeaponPower = 1;
      unit.Price = NewPrice(0, 2000, 500);
      break;
    case DESTROYER:
      unit.ShieldPower = 500;
      unit.WeaponPower = 2000;
      unit.Price = NewPrice(60000, 50000, 15000);
      break;
    case DEATHSTAR:
      unit.ShieldPower = 50000;
      unit.WeaponPower = 200000;
      unit.Price = NewPrice(5000000, 4000000, 1000000);
      break;
    case BATTLECRUISER:
      unit.ShieldPower = 400;
      unit.WeaponPower = 700;
      unit.Price = NewPrice(30000, 40000, 15000);
      break;
    case ROCKET_LAUNCHER:
      unit.ShieldPower = 20;
      unit.WeaponPower = 80;
      unit.Price = NewPrice(2000, 0, 0);
      break;
    case LIGHT_LASER:
      unit.ShieldPower = 25;
      unit.WeaponPower = 100;
      unit.Price = NewPrice(1500, 500, 0);
      break;
    case HEAVY_LASER:
      unit.ShieldPower = 100;
      unit.WeaponPower = 250;
      unit.Price = NewPrice(6000, 2000, 0);
      break;
    case GAUSS_CANNON:
      unit.ShieldPower = 200;
      unit.WeaponPower = 1100;
      unit.Price = NewPrice(20000, 15000, 2000);
      break;
    case ION_CANNON:
      unit.ShieldPower = 500;
      unit.WeaponPower = 150;
      unit.Price = NewPrice(2000, 6000, 0);
      break;
    case PLASMA_TURRET:
      unit.ShieldPower = 300;
      unit.WeaponPower = 3000;
      unit.Price = NewPrice(50000, 50000, 30000);
      break;
    case SMALL_SHIELD_DOME:
      unit.ShieldPower = 2000;
      unit.WeaponPower = 1;
      unit.Price = NewPrice(10000, 10000, 0);
      break;
    case LARGE_SHIELD_DOME:
      unit.ShieldPower = 10000;
      unit.WeaponPower = 1;
      unit.Price = NewPrice(50000, 50000, 0);
      break;
  }
  unit.InitialHullPlating = (1 + (entity->Armour / 10)) * ((unit.Price.Metal + unit.Price.Crystal) / 10);
  unit.HullPlating = unit.InitialHullPlating;
  unit.InitialShield = unit.ShieldPower * (1 + 0.1 * entity->Shield);
  unit.Shield = unit.InitialShield;
  return unit;
}

void InitEntity(Entity *entity) {
  entity->Losses = NewPrice(0, 0, 0);

  entity->TotalUnits = 0;
  entity->TotalUnits += entity->SmallCargo;
  entity->TotalUnits += entity->LargeCargo;
  entity->TotalUnits += entity->LightFighter;
  entity->TotalUnits += entity->HeavyFighter;
  entity->TotalUnits += entity->Cruiser;
  entity->TotalUnits += entity->Battleship;
  entity->TotalUnits += entity->ColonyShip;
  entity->TotalUnits += entity->Recycler;
  entity->TotalUnits += entity->EspionageProbe;
  entity->TotalUnits += entity->Bomber;
  entity->TotalUnits += entity->SolarSatellite;
  entity->TotalUnits += entity->Destroyer;
  entity->TotalUnits += entity->Deathstar;
  entity->TotalUnits += entity->Battlecruiser;
  entity->TotalUnits += entity->RocketLauncher;
  entity->TotalUnits += entity->LightLaser;
  entity->TotalUnits += entity->HeavyLaser;
  entity->TotalUnits += entity->GaussCannon;
  entity->TotalUnits += entity->IonCannon;
  entity->TotalUnits += entity->PlasmaTurret;
  entity->TotalUnits += entity->SmallShieldDome;
  entity->TotalUnits += entity->LargeShieldDome;

  CombatUnit *units = malloc(sizeof(CombatUnit) * entity->TotalUnits);
  int i;
  int idx = 0;

  for (i=0; i<entity->SmallCargo; i++)
    units[idx++] = NewUnit(entity, SMALL_CARGO);
  for (i=0; i<entity->LargeCargo; i++)
    units[idx++] = NewUnit(entity, LARGE_CARGO);
  for (i=0; i<entity->LightFighter; i++)
    units[idx++] = NewUnit(entity, LIGHT_FIGHTER);
  for (i=0; i<entity->HeavyFighter; i++)
    units[idx++] = NewUnit(entity, HEAVY_FIGHTER);
  for (i=0; i<entity->Cruiser; i++)
    units[idx++] = NewUnit(entity, CRUISER);
  for (i=0; i<entity->Battleship; i++)
    units[idx++] = NewUnit(entity, BATTLESHIP);
  for (i=0; i<entity->ColonyShip; i++)
    units[idx++] = NewUnit(entity, COLONY_SHIP);
  for (i=0; i<entity->Recycler; i++)
    units[idx++] = NewUnit(entity, RECYCLER);
  for (i=0; i<entity->EspionageProbe; i++)
    units[idx++] = NewUnit(entity, ESPIONAGE_PROBE);
  for (i=0; i<entity->Bomber; i++)
    units[idx++] = NewUnit(entity, BOMBER);
  for (i=0; i<entity->SolarSatellite; i++)
    units[idx++] = NewUnit(entity, SOLAR_SATELLITE);
  for (i=0; i<entity->Destroyer; i++)
    units[idx++] = NewUnit(entity, DESTROYER);
  for (i=0; i<entity->Deathstar; i++)
    units[idx++] = NewUnit(entity, DEATHSTAR);
  for (i=0; i<entity->Battlecruiser; i++)
    units[idx++] = NewUnit(entity, BATTLECRUISER);
  for (i=0; i<entity->RocketLauncher; i++)
    units[idx++] = NewUnit(entity, ROCKET_LAUNCHER);
  for (i=0; i<entity->LightLaser; i++)
    units[idx++] = NewUnit(entity, LIGHT_LASER);
  for (i=0; i<entity->HeavyLaser; i++)
    units[idx++] = NewUnit(entity, HEAVY_LASER);
  for (i=0; i<entity->GaussCannon; i++)
    units[idx++] = NewUnit(entity, GAUSS_CANNON);
  for (i=0; i<entity->IonCannon; i++)
    units[idx++] = NewUnit(entity, ION_CANNON);
  for (i=0; i<entity->PlasmaTurret; i++)
    units[idx++] = NewUnit(entity, PLASMA_TURRET);
  for (i=0; i<entity->SmallShieldDome; i++)
    units[idx++] = NewUnit(entity, SMALL_SHIELD_DOME);
  for (i=0; i<entity->LargeShieldDome; i++)
    units[idx++] = NewUnit(entity, LARGE_SHIELD_DOME);

  entity->Units = units;
}

bool IsAlive(CombatUnit *unit) {
  return unit->HullPlating > 0;
}

char *UnitToString(CombatUnit *unit) {
  char *msg = malloc(sizeof(char) * 100);
  if (unit->OgameID == ROCKET_LAUNCHER)
    strcpy(msg, "RocketLauncher with ");
  if (unit->OgameID == HEAVY_LASER)
    strcpy(msg, "HeavyLaser with ");
  if (unit->OgameID == CRUISER)
    strcpy(msg, "Cruiser with ");
  char buffer[20];
  sprintf(buffer, "%d:%d:%d", unit->HullPlating, unit->Shield, unit->WeaponPower);
  strcat(msg, buffer);
  return msg;
}

float RollDice() {
  return (float)rand()/(float)(RAND_MAX/1);
}

bool HasExploded(CombatUnit *unit) {
  bool exploded = false;
  float hullPercentage = (float)unit->HullPlating / (float)unit->InitialHullPlating;
  if (hullPercentage <= 0.7) {
    float probabilityOfExploding = 1.0 - (float)unit->HullPlating / (float)unit->InitialHullPlating;
    float dice = RollDice();
    if (SHOULD_LOG) {
      printf("probability of exploding of %1.3f%%: dice value of %1.3f comparing with %1.3f: ", probabilityOfExploding*100, dice, 1-probabilityOfExploding);
    }
    if (dice >= 1-probabilityOfExploding) {
      exploded = true;
      if (SHOULD_LOG) {
        printf("unit exploded.\n");
      }
    } else {
      if (SHOULD_LOG) {
        printf("unit didn't explode.\n");
      }
    }
  }
  return exploded;
}

int GetRapidFireAgainst(CombatUnit *unit, CombatUnit *targetUnit) {
  int rf = 0;
  switch(unit->OgameID) {
    case SMALL_CARGO:
      switch (targetUnit->OgameID) {
        case ESPIONAGE_PROBE: rf = 5; break;
        case SOLAR_SATELLITE: rf = 5; break;
      }
      break;
    case LARGE_CARGO:
      switch (targetUnit->OgameID) {
        case ESPIONAGE_PROBE: rf = 5; break;
        case SOLAR_SATELLITE: rf = 5; break;
      }
      break;
    case LIGHT_FIGHTER:
      switch (targetUnit->OgameID) {
        case ESPIONAGE_PROBE: rf = 5; break;
        case SOLAR_SATELLITE: rf = 5; break;
      }
      break;
    case HEAVY_FIGHTER:
      switch (targetUnit->OgameID) {
        case ESPIONAGE_PROBE: rf = 5; break;
        case SOLAR_SATELLITE: rf = 5; break;
        case SMALL_CARGO:     rf = 3; break;
      }
      break;
    case CRUISER:
      switch (targetUnit->OgameID) {
        case ESPIONAGE_PROBE: rf = 5;  break;
        case SOLAR_SATELLITE: rf = 5;  break;
        case LIGHT_FIGHTER:   rf = 6;  break;
        case ROCKET_LAUNCHER: rf = 10; break;
      }
      break;
    case BATTLESHIP:
      switch (targetUnit->OgameID) {
        case ESPIONAGE_PROBE: rf = 5; break;
        case SOLAR_SATELLITE: rf = 5; break;
      }
      break;
    case COLONY_SHIP:
      switch (targetUnit->OgameID) {
        case ESPIONAGE_PROBE: rf = 5; break;
        case SOLAR_SATELLITE: rf = 5; break;
      }
      break;
    case RECYCLER:
      switch (targetUnit->OgameID) {
        case ESPIONAGE_PROBE: rf = 5; break;
        case SOLAR_SATELLITE: rf = 5; break;
      }
      break;
    case BOMBER:
      switch (targetUnit->OgameID) {
        case ESPIONAGE_PROBE: rf = 5;  break;
        case SOLAR_SATELLITE: rf = 5;  break;
        case ION_CANNON:      rf = 10; break;
        case ROCKET_LAUNCHER: rf = 20; break;
        case LIGHT_LASER:     rf = 20; break;
        case HEAVY_LASER:     rf = 10; break;
      }
      break;
    case DESTROYER:
      switch (targetUnit->OgameID) {
        case ESPIONAGE_PROBE: rf = 5;  break;
        case SOLAR_SATELLITE: rf = 5;  break;
        case LIGHT_LASER:     rf = 10; break;
        case BATTLECRUISER:   rf = 2;  break;
      }
      break;
    case DEATHSTAR:
      switch (targetUnit->OgameID) {
        case SMALL_CARGO:     rf = 250;  break;
        case LARGE_CARGO:     rf = 250;  break;
        case LIGHT_FIGHTER:   rf = 200;  break;
        case HEAVY_FIGHTER:   rf = 100;  break;
        case CRUISER:         rf = 33;   break;
        case BATTLESHIP:      rf = 30;   break;
        case COLONY_SHIP:     rf = 250;  break;
        case RECYCLER:        rf = 250;  break;
        case ESPIONAGE_PROBE: rf = 1250; break;
        case SOLAR_SATELLITE: rf = 1250; break;
        case BOMBER:          rf = 25;   break;
        case DESTROYER:       rf = 5;    break;
        case ROCKET_LAUNCHER: rf = 200;  break;
        case LIGHT_LASER:     rf = 200;  break;
        case HEAVY_LASER:     rf = 100;  break;
        case GAUSS_CANNON:    rf = 50;   break;
        case ION_CANNON:      rf = 100;  break;
        case BATTLECRUISER:   rf = 15;   break;
      }
      break;
    case BATTLECRUISER:
      switch (targetUnit->OgameID) {
        case ESPIONAGE_PROBE: rf = 5; break;
        case SOLAR_SATELLITE: rf = 5; break;
        case SMALL_CARGO:     rf = 3; break;
        case LARGE_CARGO:     rf = 3; break;
        case HEAVY_FIGHTER:   rf = 4; break;
        case CRUISER:         rf = 4; break;
        case BATTLESHIP:      rf = 7; break;
      }
      break;
  }
  return rf;
}

bool GetAnotherShot(CombatUnit *unit, CombatUnit *targetUnit) {
  bool rapidFire = true;
  int rf = GetRapidFireAgainst(unit, targetUnit);
  if (rf > 0) {
    float chance = (float)(rf-1) / (float)rf;
    float dice = RollDice();
    if (SHOULD_LOG) {
      printf("dice was %1.3f, comparing with %1.3f: ", dice, chance);
    }
    if (dice <= chance) {
      if (SHOULD_LOG) {
        printf("%d gets another shot.\n", unit->OgameID);
      }
    } else {
      if (SHOULD_LOG) {
        printf("%d does not get another shot.\n", unit->OgameID);
      }
      rapidFire = false;
    }
  } else {
    if (SHOULD_LOG) {
      printf("%d doesn't have rapid fire against %d.\n", unit->OgameID, targetUnit->OgameID);
    }
    rapidFire = false;
  }
  return rapidFire;
}

void Attack(CombatUnit *unit, CombatUnit *targetUnit) {
  if (SHOULD_LOG) {
    char *attackingString = UnitToString(unit);
    char *targetString = UnitToString(targetUnit);
    printf("%s fires at %s; ", attackingString, targetString);
    free(attackingString);
    free(targetString);
  }
  // Check for shot bounce
  if (unit->WeaponPower < 0.01*targetUnit->InitialShield) {
    if (SHOULD_LOG) {
      printf("shot bounced\n");
    }
    return;
  }

  // Attack target
  int weapon = unit->WeaponPower;
  if (targetUnit->Shield < weapon) {
    weapon -= targetUnit->Shield;
    targetUnit->Shield = 0;
    targetUnit->HullPlating -= weapon;
  } else {
    targetUnit->Shield -= weapon;
  }
  if (SHOULD_LOG) {
    char *targetString = UnitToString(targetUnit);
    printf("result is %s\n", targetString);
    free(targetString);
  }

  // Check for explosion
  if (IsAlive(targetUnit)) {
    if (HasExploded(targetUnit)) {
      targetUnit->HullPlating = 0;
    }
  }
}

void unitsFires(Entity *attacker, Entity *defender) {
  int i;
  CombatUnit *attackingUnits = attacker->Units;
  CombatUnit *defendingUnits = defender->Units;
  for (i=0; i<attacker->TotalUnits; i++) {
    CombatUnit *unit = &attackingUnits[i];
    bool rapidFire = true;
    while (rapidFire) {
      int random = rand() % defender->TotalUnits;
      CombatUnit *targetUnit = &defendingUnits[random];
      if (IsAlive(targetUnit)) {
        Attack(unit, targetUnit);
      }
      rapidFire = GetAnotherShot(unit, targetUnit);
    }
  }
}

bool IsCombatDone(Entity *attacker, Entity *defender) {
  return defender->TotalUnits <= 0 || attacker->TotalUnits <= 0;
}

void AddPrice(Price *losses, const Price price) {
  losses->Metal += price.Metal;
  losses->Crystal += price.Crystal;
  losses->Deuterium += price.Deuterium;
}

bool IsShip(CombatUnit *unit) {
  switch(unit->OgameID) {
    case SMALL_CARGO:
    case LARGE_CARGO:
    case LIGHT_FIGHTER:
    case HEAVY_FIGHTER:
    case CRUISER:
    case BATTLESHIP:
    case COLONY_SHIP:
    case RECYCLER:
    case ESPIONAGE_PROBE:
    case BOMBER:
    case SOLAR_SATELLITE:
    case DESTROYER:
    case DEATHSTAR:
    case BATTLECRUISER:
      return true;
    default:
      return false;
  }
}

void RemoveEntityDestroyedUnits(Simulator *simulator, Entity *entity) {
  int i;
  int l = entity->TotalUnits;
  for (i = l-1; i >= 0; i--) {
    CombatUnit *unit = &entity->Units[i];
    if (unit->HullPlating <= 0) {
      if (IsShip(unit)) {
        simulator->Debris.Metal += simulator->FleetToDebris * unit->Price.Metal;
        simulator->Debris.Crystal += simulator->FleetToDebris * unit->Price.Crystal;
      }
      AddPrice(&entity->Losses, unit->Price);
      entity->Units[i] = entity->Units[entity->TotalUnits-1];
      entity->TotalUnits--;
    }
  }
}

void RemoveDestroyedUnits(Simulator *simulator) {
  RemoveEntityDestroyedUnits(simulator, simulator->Attacker);
  RemoveEntityDestroyedUnits(simulator, simulator->Defender);
}

void RestoreShields(Entity *entity) {
  int i;
  int l = entity->TotalUnits;
  for (i = l-1; i >= 0; i--) {
    CombatUnit *unit = &entity->Units[i];
    if (SHOULD_LOG) {
      char *unitString = UnitToString(unit);
      printf("%s still has integrity, restore its shield\n", unitString);
      free(unitString);
    }
    unit->Shield = unit->InitialShield;
  }
}

void PrintWinner(Simulator *simulator) {
  if (simulator->Defender->TotalUnits <= 0 && simulator->Attacker->TotalUnits <= 0) {
    simulator->Winner = 2;
    if (SHOULD_LOG) {
      printf("The battle ended draw.\n");
    }
  } else if (simulator->Defender->TotalUnits <= 0) {
    simulator->Winner = 0;
    if (SHOULD_LOG) {
      printf("The battle ended after %d rounds with attacker winning\n",
          simulator->Rounds);
    }
  } else if (simulator->Attacker->TotalUnits <= 0) {
    simulator->Winner = 1;
    if (SHOULD_LOG) {
      printf("The battle ended after %d rounds with defender winning\n",
          simulator->Rounds);
    }
  } else {
    simulator->Winner = 2;
    if (SHOULD_LOG) {
      printf("The battle ended draw.\n");
    }
  }
}

void Simulate(Simulator *simulator) {
  Entity *attacker = simulator->Attacker;
  Entity *defender = simulator->Defender;
  InitEntity(defender);
  InitEntity(attacker);
  int currentRound;
  for (currentRound=1; currentRound<=simulator->MaxRounds; currentRound++) {
    simulator->Rounds = currentRound;
    if (SHOULD_LOG) {
      printf("-------------\n");
      printf("ROUND %d\n", currentRound);
      printf("-------------\n");
    }
    unitsFires(attacker, defender);
    unitsFires(defender, attacker);
    RemoveDestroyedUnits(simulator);
    RestoreShields(attacker);
    RestoreShields(defender);
    if (IsCombatDone(attacker, defender)) {
      break;
    }
  }
  PrintWinner(simulator);
}

typedef struct {
  int SimulatorLogging;
  int Simulations;

  int AttackerWeapon;
  int AttackerShield;
  int AttackerArmour;
  int AttackerCombustion;
  int AttackerImpulse;
  int AttackerHyperspace;
  int AttackerSmallCargo;
  int AttackerLargeCargo;
  int AttackerLightFighter;
  int AttackerHeavyFighter;
  int AttackerCruiser;
  int AttackerBattleship;
  int AttackerColonyShip;
  int AttackerRecycler;
  int AttackerEspionageProbe;
  int AttackerBomber;
  int AttackerDestroyer;
  int AttackerDeathstar;
  int AttackerBattlecruiser;

  int DefenderWeapon;
  int DefenderShield;
  int DefenderArmour;
  int DefenderSmallCargo;
  int DefenderLargeCargo;
  int DefenderLightFighter;
  int DefenderHeavyFighter;
  int DefenderCruiser;
  int DefenderBattleship;
  int DefenderColonyShip;
  int DefenderRecycler;
  int DefenderEspionageProbe;
  int DefenderBomber;
  int DefenderSolarSatellite;
  int DefenderDestroyer;
  int DefenderDeathstar;
  int DefenderBattlecruiser;
  int DefenderRocketLauncher;
  int DefenderLightLaser;
  int DefenderHeavyLaser;
  int DefenderGaussCannon;
  int DefenderIonCannon;
  int DefenderPlasmaTurret;
  int DefenderSmallShieldDome;
  int DefenderLargeShieldDome;
} configuration;


static int handler(void* user, const char* section, const char* name,
                   const char* value)
{
  configuration* pconfig = (configuration*)user;

  #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
  if (MATCH("simulator", "logging")) {
    pconfig->SimulatorLogging= atoi(value);
  } else if (MATCH("simulator", "Simulations")) {
    pconfig->Simulations = atoi(value);

  } else if (MATCH("defender", "Weapon")) {
    pconfig->DefenderWeapon = atoi(value);
  } else if (MATCH("defender", "Shield")) {
    pconfig->DefenderShield = atoi(value);
  } else if (MATCH("defender", "Armour")) {
    pconfig->DefenderArmour = atoi(value);
  } else if (MATCH("defender", "SmallCargo")) {
    pconfig->DefenderSmallCargo = atoi(value);
  } else if (MATCH("defender", "LargeCargo")) {
    pconfig->DefenderLargeCargo = atoi(value);
  } else if (MATCH("defender", "LightFighter")) {
    pconfig->DefenderLightFighter = atoi(value);
  } else if (MATCH("defender", "HeavyFighter")) {
    pconfig->DefenderHeavyFighter = atoi(value);
  } else if (MATCH("defender", "Cruiser")) {
    pconfig->DefenderCruiser = atoi(value);
  } else if (MATCH("defender", "Battleship")) {
    pconfig->DefenderBattleship = atoi(value);
  } else if (MATCH("defender", "ColonyShip")) {
    pconfig->DefenderColonyShip = atoi(value);
  } else if (MATCH("defender", "Recycler")) {
    pconfig->DefenderRecycler = atoi(value);
  } else if (MATCH("defender", "EspionageProbe")) {
    pconfig->DefenderEspionageProbe = atoi(value);
  } else if (MATCH("defender", "Bomber")) {
    pconfig->DefenderBomber = atoi(value);
  } else if (MATCH("defender", "SolarSatellite")) {
    pconfig->DefenderSolarSatellite = atoi(value);
  } else if (MATCH("defender", "Destroyer")) {
    pconfig->DefenderDestroyer = atoi(value);
  } else if (MATCH("defender", "Deathstar")) {
    pconfig->DefenderDeathstar = atoi(value);
  } else if (MATCH("defender", "Battlecruiser")) {
    pconfig->DefenderBattlecruiser = atoi(value);
  } else if (MATCH("defender", "RocketLauncher")) {
    pconfig->DefenderRocketLauncher = atoi(value);
  } else if (MATCH("defender", "LightLaser")) {
    pconfig->DefenderLightLaser = atoi(value);
  } else if (MATCH("defender", "HeavyLaser")) {
    pconfig->DefenderHeavyLaser = atoi(value);
  } else if (MATCH("defender", "GaussCannon")) {
    pconfig->DefenderGaussCannon = atoi(value);
  } else if (MATCH("defender", "IonCannon")) {
    pconfig->DefenderIonCannon = atoi(value);
  } else if (MATCH("defender", "PlasmaTurret")) {
    pconfig->DefenderPlasmaTurret = atoi(value);
  } else if (MATCH("defender", "SmallShieldDome")) {
    pconfig->DefenderSmallShieldDome = atoi(value);
  } else if (MATCH("defender", "LargeShieldDome")) {
    pconfig->DefenderLargeShieldDome = atoi(value);

  } else if (MATCH("attacker", "Weapon")) {
    pconfig->AttackerWeapon = atoi(value);
  } else if (MATCH("attacker", "Shield")) {
    pconfig->AttackerShield = atoi(value);
  } else if (MATCH("attacker", "Armour")) {
    pconfig->AttackerArmour = atoi(value);
  } else if (MATCH("attacker", "Combustion")) {
    pconfig->AttackerCombustion = atoi(value);
  } else if (MATCH("attacker", "Impulse")) {
    pconfig->AttackerImpulse = atoi(value);
  } else if (MATCH("attacker", "Hyperspace")) {
    pconfig->AttackerHyperspace = atoi(value);
  } else if (MATCH("attacker", "SmallCargo")) {
    pconfig->AttackerSmallCargo = atoi(value);
  } else if (MATCH("attacker", "LargeCargo")) {
    pconfig->AttackerLargeCargo = atoi(value);
  } else if (MATCH("attacker", "LightFighter")) {
    pconfig->AttackerLightFighter = atoi(value);
  } else if (MATCH("attacker", "HeavyFighter")) {
    pconfig->AttackerHeavyFighter = atoi(value);
  } else if (MATCH("attacker", "Cruiser")) {
    pconfig->AttackerCruiser = atoi(value);
  } else if (MATCH("attacker", "Battleship")) {
    pconfig->AttackerBattleship = atoi(value);
  } else if (MATCH("attacker", "ColonyShip")) {
    pconfig->AttackerColonyShip = atoi(value);
  } else if (MATCH("attacker", "Recycler")) {
    pconfig->AttackerRecycler = atoi(value);
  } else if (MATCH("attacker", "EspionageProbe")) {
    pconfig->AttackerEspionageProbe = atoi(value);
  } else if (MATCH("attacker", "Bomber")) {
    pconfig->AttackerBomber = atoi(value);
  } else if (MATCH("attacker", "Destroyer")) {
    pconfig->AttackerDestroyer = atoi(value);
  } else if (MATCH("attacker", "Deathstar")) {
    pconfig->AttackerDeathstar = atoi(value);
  } else if (MATCH("attacker", "Battlecruiser")) {
    pconfig->AttackerBattlecruiser = atoi(value);
  } else {
    return 0;  /* unknown section/name, error */
  }
  return 1;
}

void ConfigAttacker(Entity *attacker, const configuration *config) {
  // Attacker
  attacker->Weapon          = config->AttackerWeapon;
  attacker->Shield          = config->AttackerShield;
  attacker->Armour          = config->AttackerArmour;
  attacker->Combustion      = config->AttackerCombustion;
  attacker->Impulse         = config->AttackerImpulse;
  attacker->Hyperspace      = config->AttackerHyperspace;

  attacker->SmallCargo      = config->AttackerSmallCargo;
  attacker->LargeCargo      = config->AttackerLargeCargo;
  attacker->LightFighter    = config->AttackerLightFighter;
  attacker->HeavyFighter    = config->AttackerHeavyFighter;
  attacker->Cruiser         = config->AttackerCruiser;
  attacker->Battleship      = config->AttackerBattleship;
  attacker->ColonyShip      = config->AttackerColonyShip;
  attacker->Recycler        = config->AttackerRecycler;
  attacker->EspionageProbe  = config->AttackerEspionageProbe;
  attacker->Bomber          = config->AttackerBomber;
  attacker->SolarSatellite  = 0;
  attacker->Destroyer       = config->AttackerDestroyer;
  attacker->Deathstar       = config->AttackerDeathstar;
  attacker->Battlecruiser   = config->AttackerBattlecruiser;

  attacker->RocketLauncher  = 0;
  attacker->LightLaser      = 0;
  attacker->HeavyLaser      = 0;
  attacker->GaussCannon     = 0;
  attacker->IonCannon       = 0;
  attacker->PlasmaTurret    = 0;
  attacker->SmallShieldDome = 0;
  attacker->LargeShieldDome = 0;
}

void ConfigDefender(Entity *defender, const configuration *config) {
  // Defender
  defender->Weapon          = config->DefenderWeapon;
  defender->Shield          = config->DefenderShield;
  defender->Armour          = config->DefenderArmour;
  defender->Combustion      = 0;
  defender->Impulse         = 0;
  defender->Hyperspace      = 0;

  defender->SmallCargo      = config->DefenderSmallCargo;
  defender->LargeCargo      = config->DefenderLargeCargo;
  defender->LightFighter    = config->DefenderLightFighter;
  defender->HeavyFighter    = config->DefenderHeavyFighter;
  defender->Cruiser         = config->DefenderCruiser;
  defender->Battleship      = config->DefenderBattleship;
  defender->ColonyShip      = config->DefenderColonyShip;
  defender->Recycler        = config->DefenderRecycler;
  defender->EspionageProbe  = config->DefenderEspionageProbe;
  defender->Bomber          = config->DefenderBomber;
  defender->SolarSatellite  = config->DefenderSolarSatellite;
  defender->Destroyer       = config->DefenderDestroyer;
  defender->Deathstar       = config->DefenderDeathstar;
  defender->Battlecruiser   = config->DefenderBattlecruiser;

  defender->RocketLauncher  = config->DefenderRocketLauncher;
  defender->LightLaser      = config->DefenderLightLaser;
  defender->HeavyLaser      = config->DefenderHeavyLaser;
  defender->GaussCannon     = config->DefenderGaussCannon;
  defender->IonCannon       = config->DefenderIonCannon;
  defender->PlasmaTurret    = config->DefenderPlasmaTurret;
  defender->SmallShieldDome = config->DefenderSmallShieldDome;
  defender->LargeShieldDome = config->DefenderLargeShieldDome;
}

int main(int argc, char *argv[]) {

  bool jsonOutput = false;
  char *configPath = "config.ini";
  int opt;

  while ((opt = getopt(argc, argv, "jc:")) != -1) {
    switch (opt) {
      case 'j': jsonOutput = true; break;
      case 'c': configPath = optarg; break;
      case '?':
        if (optopt == 'c') {
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
          exit(EXIT_FAILURE);
        }
        break;
      default:
        fprintf(stderr, "Usage: %s [-ilw] [file...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  }

  srand(time(NULL));
  configuration config;

  if (ini_parse(configPath, handler, &config) < 0) {
    printf("Can't load '%s'\n", configPath);
    return 1;
  }

  SHOULD_LOG = config.SimulatorLogging;
  int nbSimulations = config.Simulations;

  Entity *defender = malloc(sizeof(Entity));
  Entity *attacker = malloc(sizeof(Entity));

  ConfigAttacker(attacker, &config);
  ConfigDefender(defender, &config);

  Simulator *simulator = malloc(sizeof(Simulator));
  simulator->FleetToDebris = 0.3;
  simulator->MaxRounds = 6;
  simulator->Attacker = attacker;
  simulator->Defender = defender;

  int attackerWin = 0;
  int defenderWin = 0;
  int draw = 0;
  int rounds = 0;
  int moonchance = 0;
  Price attackerLosses = NewPrice(0, 0, 0);
  Price defenderLosses = NewPrice(0, 0, 0);
  Price debris = NewPrice(0, 0, 0);

  if (!jsonOutput) {
    printf("Simulations 0/%d\r", nbSimulations);
    fflush(stdout);
  }

  int i;
  for (i=0; i<nbSimulations; i++) {
    simulator->Rounds = 0;
    simulator->Debris = NewPrice(0, 0, 0);
    Simulate(simulator);

    if (!jsonOutput) {
      printf("Simulations %d/%d\r", i, nbSimulations);
      fflush(stdout);
    }

    if (simulator->Winner == 0)
      attackerWin++;
    else if (simulator->Winner == 1)
      defenderWin++;
    else
      draw++;
    AddPrice(&attackerLosses, simulator->Attacker->Losses);
    AddPrice(&defenderLosses, simulator->Defender->Losses);
    AddPrice(&debris, simulator->Debris);
    rounds += simulator->Rounds;
    int debris = (float)simulator->Debris.Metal + (float)simulator->Debris.Crystal;
    moonchance += (int)(fmin(debris/100000.0f, 20.0f));

    free(attacker->Units);
    free(defender->Units);
  }

  if (!jsonOutput) {
    printf("Simulations %d/%d\n\n", nbSimulations, nbSimulations);
  }

  rounds = rounds/nbSimulations;
  attackerWin = (int)roundf((float)attackerWin/(float)nbSimulations * 100);
  defenderWin = (int)roundf((float)defenderWin/(float)nbSimulations * 100);
  draw = (int)roundf((float)draw/(float)nbSimulations * 100);
  int recycler = (int)(ceil(((float)(debris.Metal + debris.Crystal) / (float)nbSimulations) / 20000.0));
  moonchance = (int)((float)moonchance / (float)nbSimulations);

  if (jsonOutput) {
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    char *serialized_string = NULL;
    json_object_set_number(root_object, "simulations", nbSimulations);
    json_object_set_number(root_object, "attacker_win", attackerWin);
    json_object_set_number(root_object, "defender_win", defenderWin);
    json_object_set_number(root_object, "draw", draw);
    json_object_set_number(root_object, "rounds", rounds);
    json_object_dotset_number(root_object, "attacker_losses.metal", attackerLosses.Metal/nbSimulations);
    json_object_dotset_number(root_object, "attacker_losses.crystal", attackerLosses.Crystal/nbSimulations);
    json_object_dotset_number(root_object, "attacker_losses.deuterium", attackerLosses.Deuterium/nbSimulations);
    json_object_dotset_number(root_object, "defender_losses.metal", defenderLosses.Metal/nbSimulations);
    json_object_dotset_number(root_object, "defender_losses.crystal", defenderLosses.Crystal/nbSimulations);
    json_object_dotset_number(root_object, "defender_losses.deuterium", defenderLosses.Deuterium/nbSimulations);
    json_object_dotset_number(root_object, "debris.metal", debris.Metal/nbSimulations);
    json_object_dotset_number(root_object, "debris.crystal", debris.Crystal/nbSimulations);
    json_object_dotset_number(root_object, "debris.deuterium", debris.Deuterium/nbSimulations);
    json_object_set_number(root_object, "recycler", recycler);
    json_object_set_number(root_object, "moonchance", moonchance);
    serialized_string = json_serialize_to_string_pretty(root_value);
    puts(serialized_string);
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
  } else {
    printf("Results (%d simulations | ~%d rounds)\n", nbSimulations, rounds);
    printf("Attacker win: %d%%\n", attackerWin);
    printf("Defender win: %d%%\n", defenderWin);
    printf("Draw: %d%%\n", draw);
    printf("Attacker losses: %d, %d, %d\n",
        attackerLosses.Metal/nbSimulations,
        attackerLosses.Crystal/nbSimulations,
        attackerLosses.Deuterium/nbSimulations);
    printf("Defender losses: %d, %d, %d\n",
        defenderLosses.Metal/nbSimulations,
        defenderLosses.Crystal/nbSimulations,
        defenderLosses.Deuterium/nbSimulations);
    printf("Debris: %d, %d, %d\n",
        debris.Metal/nbSimulations,
        debris.Crystal/nbSimulations,
        debris.Deuterium/nbSimulations);
    printf("Recycler: %d\n", recycler);
    printf("Moonchance: %d%%\n", moonchance);
  }


  free(attacker);
  free(defender);
  free(simulator);

  return 0;
}
