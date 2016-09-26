#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "./ini.h"

int SHOULD_LOG = 0;

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
  int HullPlating;
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
} Entity;

Price NewPrice(int metal, int crystal, int deuterium) {
  Price price;
  price.Metal = metal;
  price.Crystal = crystal;
  price.Deuterium = deuterium;
  return price;
}

CombatUnit NewUnit(int OgameID) {
  CombatUnit unit;
  unit.Id = 1;
  unit.OgameID = OgameID;
  switch(OgameID) {
    case DEATHSTAR:
      unit.ShieldPower = 50000;
      unit.WeaponPower = 200000;
      unit.Price = NewPrice(5000000, 4000000, 1000000);
      break;
    case ROCKET_LAUNCHER:
      unit.ShieldPower = 20;
      unit.WeaponPower = 80;
      unit.Price = NewPrice(2000, 0, 0);
      break;
    case CRUISER:
      unit.ShieldPower = 50;
      unit.WeaponPower = 400;
      unit.Price = NewPrice(20000, 7000, 2000);
      break;
    case HEAVY_LASER:
      unit.ShieldPower = 100;
      unit.WeaponPower = 250;
      unit.Price = NewPrice(6000, 2000, 0);
      break;
    case LARGE_SHIELD_DOME:
      unit.ShieldPower = 10000;
      unit.WeaponPower = 1;
      unit.Price = NewPrice(50000, 50000, 0);
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
  }
  unit.HullPlating = (1 + (0 / 10)) * ((unit.Price.Metal + unit.Price.Crystal) / 10);
  unit.Shield = unit.ShieldPower * (1 + 0.1*0);
  return unit;
}

void InitEntity(Entity *entity) {
  int nbDeathstar = entity->Deathstar;
  int nbCruiser = entity->Cruiser;
  int nbLightFighter = entity->LightFighter;
  int nbHeavyFighter = entity->HeavyFighter;
  int nbLargeShieldDome = entity->LargeShieldDome;
  int nbHeavyLaser = entity->HeavyLaser;
  int nbRocketLauncher = entity->RocketLauncher;
  entity->TotalUnits = nbDeathstar + nbCruiser + nbLightFighter + nbHeavyFighter;
  entity->TotalUnits += nbLargeShieldDome + nbHeavyLaser + nbRocketLauncher;
  CombatUnit *units = malloc(sizeof(CombatUnit) * entity->TotalUnits);
  int i;
  int idx = 0;
  for (i=0; i<nbDeathstar; i++)
    units[idx++] = NewUnit(DEATHSTAR);
  for (i=0; i<nbCruiser; i++)
    units[idx++] = NewUnit(CRUISER);
  for (i=0; i<nbLightFighter; i++)
    units[idx++] = NewUnit(LIGHT_FIGHTER);
  for (i=0; i<nbHeavyFighter; i++)
    units[idx++] = NewUnit(HEAVY_FIGHTER);
  for (i=0; i<nbRocketLauncher; i++)
    units[idx++] = NewUnit(ROCKET_LAUNCHER);
  for (i=0; i<nbHeavyLaser; i++)
    units[idx++] = NewUnit(HEAVY_LASER);
  for (i=0; i<nbLargeShieldDome; i++)
    units[idx++] = NewUnit(LARGE_SHIELD_DOME);
  entity->Units = units;
}

int IsAlive(CombatUnit *unit) {
  return unit->HullPlating > 0;
}

int GetInitialHullPlating(CombatUnit *unit) {
  return (1 + (0 / 10)) * ((unit->Price.Metal + unit->Price.Crystal) / 10);
}

char *UnitToString(CombatUnit *unit) {
  char *msg = malloc(sizeof(char) * 100);
  if (unit->OgameID == ROCKET_LAUNCHER)
    strcat(msg, "RocketLauncher with ");
  if (unit->OgameID == HEAVY_LASER)
    strcat(msg, "HeavyLaser with ");
  if (unit->OgameID == CRUISER)
    strcat(msg, "Cruiser with ");
  char buffer[20];
  sprintf(buffer, "%d:%d:%d", unit->HullPlating, unit->Shield, unit->WeaponPower);
  strcat(msg, buffer);
  return msg;
}

int HasExploded(CombatUnit *unit) {
  int exploded = 0;
  float hullPercentage = (float)unit->HullPlating / (float)GetInitialHullPlating(unit);
  if (hullPercentage <= 0.7) {
    float probabilityOfExploding = 1.0 - (float)unit->HullPlating / (float)GetInitialHullPlating(unit);
    float dice = (float)rand()/(float)(RAND_MAX/1);
    if (SHOULD_LOG) {
      printf("probability of exploding of %1.3f%%: dice value of %1.3f comparing with %1.3f: ", probabilityOfExploding*100, dice, 1-probabilityOfExploding);
    }
    if (dice >= 1-probabilityOfExploding) {
      exploded = 1;
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
  }
  return rf;
}

int GetAnotherShot(CombatUnit *unit, CombatUnit *targetUnit) {
  int rapidFire = 1;
  int rf = GetRapidFireAgainst(unit, targetUnit);
  if (rf > 0) {
    float chance = (float)(rf-1) / (float)rf;
    float dice = (float)rand()/(float)(RAND_MAX/1);
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
      rapidFire = 0;
    }
  } else {
    if (SHOULD_LOG) {
      printf("%d doesn't have rapid fire against %d.\n", unit->OgameID, targetUnit->OgameID);
    }
    rapidFire = 0;
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
  if (unit->WeaponPower < 0.01*targetUnit->Shield) {
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
    int rapidFire = 1;
    while (rapidFire) {
      int random = 0;
      if (defender->TotalUnits > 1) {
        random = rand() % (defender->TotalUnits-1);
      }
      CombatUnit *targetUnit = &defendingUnits[random];
      if (IsAlive(targetUnit)) {
        Attack(unit, targetUnit);
      }
      rapidFire = GetAnotherShot(unit, targetUnit);
    }
  }
}

int IsCombatDone(Entity *attacker, Entity *defender) {
  return defender->TotalUnits <= 0 || attacker->TotalUnits <= 0;
}

void RemoveDestroyedUnits(Entity *entity) {
  int i;
  int l = entity->TotalUnits;
  for (i = l-1; i >= 0; i--) {
    CombatUnit *unit = &entity->Units[i];
    if (unit->HullPlating <= 0) {
      entity->Units[i] = entity->Units[entity->TotalUnits-1];
      entity->TotalUnits--;
    }
  }
}

void RestoreShields(Entity *entity) {
  int i;
  int l = entity->TotalUnits;
  for (i = l-1; i >= 0; i--) {
    CombatUnit *unit = &entity->Units[i];
    unit->Shield = unit->ShieldPower * (1 + 0.1*0);
    if (SHOULD_LOG) {
      char *unitString = UnitToString(unit);
      printf("%s still has integrity, restore its shield\n", unitString);
      free(unitString);
    }
  }
}

void PrintWinner(Entity *attacker, Entity *defender) {
  if (defender->TotalUnits <= 0) {
    printf("Attacker win\n");
  } else if (attacker->TotalUnits <= 0) {
    printf("Defender win\n");
  } else {
    printf("Draw\n");
  }
}

void Simulate(Entity *attacker, Entity *defender) {
  InitEntity(defender);
  InitEntity(attacker);
  int i;
  for (i=0; i<6; i++) {
    if (SHOULD_LOG) {
      printf("-------------\n");
      printf("ROUND %d\n", i);
      printf("-------------\n");
    }
    unitsFires(attacker, defender);
    unitsFires(defender, attacker);
    RemoveDestroyedUnits(attacker);
    RemoveDestroyedUnits(defender);
    RestoreShields(attacker);
    RestoreShields(defender);
    if (IsCombatDone(attacker, defender)) {
      break;
    }
  }
  PrintWinner(attacker, defender);
}

typedef struct {
  int SimulatorLogging;

  int AttackerWeapon;
  int AttackerShield;
  int AttackerArmour;
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

int main(int argc, char *argv[]) {
  configuration config;

  if (ini_parse("test.ini", handler, &config) < 0) {
    printf("Can't load 'test.ini'\n");
    return 1;
  }

  SHOULD_LOG = config.SimulatorLogging;

  srand(time(NULL));
  Entity *defender = malloc(sizeof(Entity));
  Entity *attacker = malloc(sizeof(Entity));

  attacker->Deathstar = config.AttackerDeathstar;
  attacker->Cruiser = config.AttackerCruiser;
  attacker->LightFighter = config.AttackerLightFighter;
  attacker->HeavyFighter = config.AttackerHeavyFighter;
  attacker->HeavyLaser = 0;
  attacker->RocketLauncher = 0;
  attacker->LargeShieldDome = 0;

  defender->Deathstar = 0;
  defender->Cruiser = 0;
  defender->LightFighter = 0;
  defender->HeavyFighter = 0;
  defender->HeavyLaser = config.DefenderHeavyLaser;
  defender->RocketLauncher = config.DefenderRocketLauncher;
  defender->LargeShieldDome = config.DefenderLargeShieldDome;

  Simulate(attacker, defender);

  free(attacker->Units);
  free(defender->Units);
  free(attacker);
  free(defender);

  return 0;
}
