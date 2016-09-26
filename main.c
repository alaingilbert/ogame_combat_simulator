#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int SHOULD_LOG = 0;

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
  int Weapon;
  int Shield;
  int Armour;
  int Deathstar;
  int Cruiser;
  int TotalUnits;
  CombatUnit *Units;
} Entity;

typedef struct {
  Entity entity;
  int RocketLauncher;
  int HeavyLaser;
} Defender;

typedef struct {
  Entity entity;
  int Combustion;
  int Impulse;
  int Hyperspace;
} Attacker;

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
  if (OgameID == 214) { // Deathstar
    unit.ShieldPower = 50000;
    unit.WeaponPower = 200000;
    unit.Price = NewPrice(5000000, 4000000, 1000000);
  } else if (OgameID == 401) { // Rocket Launcher
    unit.ShieldPower = 20;
    unit.WeaponPower = 80;
    unit.Price = NewPrice(2000, 0, 0);
  } else if (OgameID == 206) { // Cruiser
    unit.ShieldPower = 50;
    unit.WeaponPower = 400;
    unit.Price = NewPrice(20000, 7000, 2000);
  } else if (OgameID == 403) { // HeavyLaser
    unit.ShieldPower = 100;
    unit.WeaponPower = 250;
    unit.Price = NewPrice(6000, 2000, 0);
  }
  unit.HullPlating = (1 + (0 / 10)) * ((unit.Price.Metal + unit.Price.Crystal) / 10);
  unit.Shield = unit.ShieldPower * (1 + 0.1*0);
  return unit;
}

int getNbAttackingUnits(const Attacker *attacker) {
  return attacker->entity.TotalUnits;
}

int getNbDefendingUnits(const Defender *defender) {
  return defender->entity.TotalUnits;
}

void InitAttacker(Attacker *attacker) {
  int nbDeathstar = attacker->entity.Deathstar;
  int nbCruiser = attacker->entity.Cruiser;
  attacker->entity.TotalUnits = nbDeathstar + nbCruiser;
  int nbUnits = getNbAttackingUnits(attacker);
  CombatUnit *units = malloc(sizeof(CombatUnit) * nbUnits);
  int i;
  int idx = 0;
  for (i=0; i<nbDeathstar; i++) {
    units[idx++] = NewUnit(214);
  }
  for (i=0; i<nbCruiser; i++) {
    units[idx++] = NewUnit(206);
  }
  attacker->entity.Units = units;
}

void InitDefender(Defender *defender) {
  int nbHeavyLaser = defender->HeavyLaser;
  int nbRocketLauncher = defender->RocketLauncher;
  defender->entity.TotalUnits = nbRocketLauncher + nbHeavyLaser;
  int nbUnits = getNbDefendingUnits(defender);
  CombatUnit *units = malloc(sizeof(CombatUnit) * nbUnits);
  int i;
  int idx = 0;
  for (i=0; i<nbRocketLauncher; i++) {
    units[idx++] = NewUnit(401);
  }
  for (i=0; i<nbHeavyLaser; i++) {
    units[idx++] = NewUnit(403);
  }
  defender->entity.Units = units;
}

int IsAlive(CombatUnit *unit) {
  return unit->HullPlating > 0;
}

int GetInitialHullPlating(CombatUnit *unit) {
  return (1 + (0 / 10)) * ((unit->Price.Metal + unit->Price.Crystal) / 10);
}

char *UnitToString(CombatUnit *unit) {
  char *msg = malloc(sizeof(char) * 100);
  if (unit->OgameID == 401) {
    strcat(msg, "RocketLauncher with ");
  }
  if (unit->OgameID == 403) {
    strcat(msg, "HeavyLaser with ");
  }
  if (unit->OgameID == 206) {
    strcat(msg, "Cruiser with ");
  }
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
  if (unit->OgameID == 206) {
    if (targetUnit->OgameID == 401) {
      return 10;
    }
  } else if (unit->OgameID == 214) {
    if (targetUnit->OgameID == 401) {
      return 200;
    }
  }
  return 0;
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
    printf("%s fires at %s; ", UnitToString(unit), UnitToString(targetUnit));
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
    printf("result is %s\n", UnitToString(targetUnit));
  }

  // Check for explosion
  if (IsAlive(targetUnit)) {
    if (HasExploded(targetUnit)) {
      targetUnit->HullPlating = 0;
    }
  }
}

void unitsFires(Attacker *attacker, Defender *defender) {
  int i;
  CombatUnit *attackingUnits = attacker->entity.Units;
  CombatUnit *defendingUnits = defender->entity.Units;
  int nbUnits = getNbAttackingUnits(attacker);
  int nbDefendingUnits = getNbDefendingUnits(defender);
  for (i=0; i<nbUnits; i++) {
    CombatUnit *unit = &attackingUnits[i];
    int rapidFire = 1;
    while (rapidFire) {
      int random = 0;
      if (nbDefendingUnits > 1) {
        random = rand() % (nbDefendingUnits-1);
      }
      CombatUnit *targetUnit = &defendingUnits[random];
      if (IsAlive(targetUnit)) {
        Attack(unit, targetUnit);
      }
      rapidFire = GetAnotherShot(unit, targetUnit);
    }
  }
}

void defencesFires(Defender *defender, Attacker *attacker) {
  int i;
  CombatUnit *attackingUnits = attacker->entity.Units;
  CombatUnit *defendingUnits = defender->entity.Units;
  int nbUnits = getNbAttackingUnits(attacker);
  int nbDefendingUnits = getNbDefendingUnits(defender);
  for (i=0; i<nbDefendingUnits; i++) {
    CombatUnit *unit = &defendingUnits[i];
    int rapidFire = 1;
    while (rapidFire) {
      int random = 0;
      if (nbUnits > 1) {
        random = rand() % (nbUnits-1);
      }
      CombatUnit *targetUnit = &attackingUnits[random];
      if (IsAlive(targetUnit)) {
        Attack(unit, targetUnit);
      }
      rapidFire = GetAnotherShot(unit, targetUnit);
    }
  }
}

int IsCombatDone(Attacker *attacker, Defender *defender) {
  return defender->entity.TotalUnits <= 0 || attacker->entity.TotalUnits <= 0;
}

void RemoveDestroyedUnits(Attacker *attacker, Defender *defender) {
  int i;
  int l = attacker->entity.TotalUnits;
  for (i = l-1; i >= 0; i--) {
    CombatUnit *unit = &attacker->entity.Units[i];
    if (unit->HullPlating <= 0) {
      attacker->entity.Units[i] = attacker->entity.Units[attacker->entity.TotalUnits-1];
      attacker->entity.TotalUnits--;
    }
  }
  l = defender->entity.TotalUnits;
  for (i = l-1; i >= 0; i--) {
    CombatUnit *unit = &defender->entity.Units[i];
    if (unit->HullPlating <= 0) {
      defender->entity.Units[i] = defender->entity.Units[defender->entity.TotalUnits-1];
      defender->entity.TotalUnits--;
    }
  }
}

void RestoreShields(Attacker *attacker, Defender *defender) {
  int i;
  int l = attacker->entity.TotalUnits;
  for (i = l-1; i >= 0; i--) {
    CombatUnit *unit = &attacker->entity.Units[i];
    unit->Shield = unit->ShieldPower * (1 + 0.1*0);
    if (SHOULD_LOG) {
      printf("%s still has integrity, restore its shield\n", UnitToString(unit));
    }
  }
  l = defender->entity.TotalUnits;
  for (i = l-1; i >= 0; i--) {
    CombatUnit *unit = &defender->entity.Units[i];
    unit->Shield = unit->ShieldPower * (1 + 0.1*0);
    if (SHOULD_LOG) {
      printf("%s still has integrity, restore its shield\n", UnitToString(unit));
    }
  }
}

void PrintWinner(Attacker *attacker, Defender *defender) {
  if (defender->entity.TotalUnits <= 0) {
    printf("Attacker win\n");
  } else if (attacker->entity.TotalUnits <= 0) {
    printf("Defender win\n");
  } else {
    printf("Draw\n");
  }
}

void Simulate(Attacker *attacker, Defender *defender) {
  InitDefender(defender);
  InitAttacker(attacker);
  int i;
  for (i=0; i<6; i++) {
    if (SHOULD_LOG) {
      printf("-------------\n");
      printf("ROUND %d\n", i);
      printf("-------------\n");
    }
    unitsFires(attacker, defender);
    defencesFires(defender, attacker);
    RemoveDestroyedUnits(attacker, defender);
    RestoreShields(attacker, defender);
    if (IsCombatDone(attacker, defender)) {
      break;
    }
  }
  PrintWinner(attacker, defender);
}

int main(int argc, char *argv[]) {
  srand(time(NULL));
  Defender *defender = malloc(sizeof(Defender));
  Attacker *attacker = malloc(sizeof(Attacker));

  attacker->entity.Deathstar = 10000;
  attacker->entity.Cruiser = 0;

  defender->entity.Deathstar = 0;
  defender->entity.Cruiser = 0;
  defender->HeavyLaser = 0;
  defender->RocketLauncher = 1000000;

  Simulate(attacker, defender);
  return 0;
}
