extern crate rand;
extern crate toml;
extern crate serde;
#[macro_use]
extern crate serde_derive;

use std::fmt;
use std::fs::File;
use std::io::Read;
use std::cmp;

const SHOULD_LOG: bool = false;

#[derive(Copy, Clone)]
enum Unit {
    SmallCargo,
    LargeCargo,
    LightFighter,
    HeavyFighter,
    Cruiser,
    Battleship,
    ColonyShip,
    Recycler,
    EspionageProbe,
    Bomber,
    SolarSatellite,
    Destroyer,
    Deathstar,
    Battlecruiser,
    RocketLauncher,
    LightLaser,
    HeavyLaser,
    GaussCannon,
    IonCannon,
    PlasmaTurret,
    SmallShieldDome,
    LargeShieldDome,
}

struct Price {
    metal: u64,
    crystal: u64,
    deuterium: u64,
}

//impl Price {
//    fn add(&self, metal: u64, crystal: u64, deuterium: u64) -> Price {
//        Price{metal: self.metal + metal, crystal: self.crystal + crystal, deuterium: self.deuterium + deuterium}
//    }
//}

#[derive(Copy, Clone)]
struct CombatUnit {
    packed_infos: u64,
}

const ID_MASK:     u64 = 0b00000000_00000000_00000000_00000000__00000000_00000000_00000000_00011111;
const SHIELD_MASK: u64 = 0b00000000_00000000_00000000_00000000__00000000_01111111_11111111_11100000;
const HULL_MASK:   u64 = 0b00000000_00000000_00011111_11111111__11111111_10000000_00000000_00000000;

fn get_id(unit: &CombatUnit) -> Unit {
    id_2_unit(((unit.packed_infos & ID_MASK) >> 0) as u8)
}

fn get_shield(unit: &CombatUnit) -> u32 {
    ((unit.packed_infos & SHIELD_MASK) >> 5) as u32
}

fn get_hull(unit: &CombatUnit) -> u32 {
    ((unit.packed_infos & HULL_MASK) >> 23) as u32
}

fn set_id(unit: &mut CombatUnit, u: Unit) {
    unit.packed_infos &= !ID_MASK;
    unit.packed_infos |= (u as u64) << 0;
}

fn set_shield(unit: &mut CombatUnit, shield: u32) {
    unit.packed_infos &= !SHIELD_MASK;
    unit.packed_infos |= (shield as u64) << 5;
}

fn set_hull(unit: &mut CombatUnit, hull: u64) {
    unit.packed_infos &= !HULL_MASK;
    unit.packed_infos |= hull << 23;
}

fn is_alive(unit: &CombatUnit) -> bool {
    get_hull(unit) > 0
}

//impl CombatUnit {
//    fn get_id(&self) -> Unit {
//        id_2_unit(((self.packed_infos & ID_MASK) >> 0) as u8)
//    }
//
//    fn get_shield(&self) -> u32 {
//        ((self.packed_infos & SHIELD_MASK) >> 5) as u32
//    }
//
//    fn get_hull(&self) -> u32 {
//        ((self.packed_infos & HULL_MASK) >> 23) as u32
//    }
//
//    fn set_id(&mut self, unit: Unit) {
//        self.packed_infos &= !ID_MASK;
//        self.packed_infos |= (unit as u64) << 0;
//    }
//
//    fn set_shield(&mut self, shield: u32) {
//        self.packed_infos &= !SHIELD_MASK;
//        self.packed_infos |= (shield as u64) << 5;
//    }
//
//    fn set_hull(&mut self, hull: u64) {
//        self.packed_infos &= !HULL_MASK;
//        self.packed_infos |= hull << 23;
//    }
//
//    fn is_alive(&self) -> bool {
//        self.get_hull() > 0
//    }
//}

struct Entity {
    // Technos
    weapon: u8,
    shield: u8,
    armour: u8,
    combustion: u8,
    impulse: u8,
    hyperspace: u8,

    // Ships
    small_cargo: u32,
    large_cargo: u32,
    light_fighter: u32,
    heavy_fighter: u32,
    cruiser: u32,
    battleship: u32,
    colony_ship: u32,
    recycler: u32,
    espionage_probe: u32,
    bomber: u32,
    solar_satellite: u32,
    destroyer: u32,
    deathstar: u32,
    battlecruiser: u32,

    // Defences
    rocket_launcher: u32,
    light_laser: u32,
    heavy_laser: u32,
    gauss_cannon: u32,
    ion_cannon: u32,
    plasma_turret: u32,
    small_shield_dome: u32,
    large_shield_dome: u32,

    total_units: u32,
    units: Vec<CombatUnit>,
    losses: Price,
}


impl fmt::Display for Entity {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "Entity({}, {})", self.total_units, self.rocket_launcher)
    }
}

impl Default for Entity {
    fn default() -> Entity {
        Entity {
            weapon: 0,
            shield: 0,
            armour: 0,
            combustion: 0,
            impulse: 0,
            hyperspace: 0,
            small_cargo: 0,
            large_cargo: 0,
            light_fighter: 0,
            heavy_fighter: 0,
            cruiser: 0,
            battleship: 0,
            colony_ship: 0,
            recycler: 0,
            espionage_probe: 0,
            bomber: 0,
            solar_satellite: 0,
            destroyer: 0,
            deathstar: 0,
            battlecruiser: 0,
            rocket_launcher: 0,
            light_laser: 0,
            heavy_laser: 0,
            gauss_cannon: 0,
            ion_cannon: 0,
            plasma_turret: 0,
            small_shield_dome: 0,
            large_shield_dome: 0,
            total_units: 0,
            losses: Price{metal: 0, crystal: 0, deuterium: 0},
            units: Vec::new(),
        }
    }
}

struct Simulator {
    winner: u32,
    rounds: u32,
    max_rounds: u32,
    fleet_to_debris: f64,
    attacker: Entity,
    defender: Entity,
    debris: Price,
}

impl Default for Simulator {
    fn default() -> Simulator {
        Simulator {
            winner: 0,
            rounds: 0,
            max_rounds: 6,
            fleet_to_debris: 0.3,
            debris: Price{metal: 0, crystal: 0, deuterium: 0},
            attacker: Entity{..Default::default()},
            defender: Entity{..Default::default()},
        }
    }
}

fn id_2_unit(id: u8) -> Unit {
    match id {
        id if id == Unit::SmallCargo      as u8 => Unit::SmallCargo,
        id if id == Unit::LargeCargo      as u8 => Unit::LargeCargo,
        id if id == Unit::LightFighter    as u8 => Unit::LightFighter,
        id if id == Unit::HeavyFighter    as u8 => Unit::HeavyFighter,
        id if id == Unit::Cruiser         as u8 => Unit::Cruiser,
        id if id == Unit::Battleship      as u8 => Unit::Battleship,
        id if id == Unit::ColonyShip      as u8 => Unit::ColonyShip,
        id if id == Unit::Recycler        as u8 => Unit::Recycler,
        id if id == Unit::EspionageProbe  as u8 => Unit::EspionageProbe,
        id if id == Unit::Bomber          as u8 => Unit::Bomber,
        id if id == Unit::SolarSatellite  as u8 => Unit::SolarSatellite,
        id if id == Unit::Destroyer       as u8 => Unit::Destroyer,
        id if id == Unit::Deathstar       as u8 => Unit::Deathstar,
        id if id == Unit::Battlecruiser   as u8 => Unit::Battlecruiser,
        id if id == Unit::RocketLauncher  as u8 => Unit::RocketLauncher,
        id if id == Unit::LightLaser      as u8 => Unit::LightLaser,
        id if id == Unit::HeavyLaser      as u8 => Unit::HeavyLaser,
        id if id == Unit::GaussCannon     as u8 => Unit::GaussCannon,
        id if id == Unit::IonCannon       as u8 => Unit::IonCannon,
        id if id == Unit::PlasmaTurret    as u8 => Unit::PlasmaTurret,
        id if id == Unit::SmallShieldDome as u8 => Unit::SmallShieldDome,
        id if id == Unit::LargeShieldDome as u8 => Unit::LargeShieldDome,
        _ => panic!("Invalid ID"),
    }
}

fn get_rapid_fire_against(unit: &CombatUnit, target: &CombatUnit) -> u32 {
    match (get_id(unit), get_id(target)) {
        (Unit::SmallCargo,    Unit::EspionageProbe) =>     5,
        (Unit::SmallCargo,    Unit::SolarSatellite) =>     5,
        (Unit::LargeCargo,    Unit::EspionageProbe) =>     5,
        (Unit::LargeCargo,    Unit::SolarSatellite) =>     5,
        (Unit::LightFighter,  Unit::EspionageProbe) =>     5,
        (Unit::LightFighter,  Unit::SolarSatellite) =>     5,
        (Unit::HeavyFighter,  Unit::EspionageProbe) =>     5,
        (Unit::HeavyFighter,  Unit::SolarSatellite) =>     5,
        (Unit::HeavyFighter,  Unit::SmallCargo)     =>     3,
        (Unit::Cruiser,       Unit::EspionageProbe) =>     5,
        (Unit::Cruiser,       Unit::SolarSatellite) =>     5,
        (Unit::Cruiser,       Unit::LightFighter)   =>     6,
        (Unit::Cruiser,       Unit::RocketLauncher) =>    10,
        (Unit::Battleship,    Unit::EspionageProbe) =>     5,
        (Unit::Battleship,    Unit::SolarSatellite) =>     5,
        (Unit::ColonyShip,    Unit::EspionageProbe) =>     5,
        (Unit::ColonyShip,    Unit::SolarSatellite) =>     5,
        (Unit::Recycler,      Unit::EspionageProbe) =>     5,
        (Unit::Recycler,      Unit::SolarSatellite) =>     5,
        (Unit::Bomber,        Unit::EspionageProbe) =>     5,
        (Unit::Bomber,        Unit::SolarSatellite) =>     5,
        (Unit::Bomber,        Unit::IonCannon)      =>    10,
        (Unit::Bomber,        Unit::RocketLauncher) =>    20,
        (Unit::Bomber,        Unit::LightLaser)     =>    20,
        (Unit::Bomber,        Unit::HeavyLaser)     =>    10,
        (Unit::Destroyer,     Unit::EspionageProbe) =>     5,
        (Unit::Destroyer,     Unit::SolarSatellite) =>     5,
        (Unit::Destroyer,     Unit::LightLaser)     =>    10,
        (Unit::Destroyer,     Unit::Battlecruiser)  =>     2,
        (Unit::Deathstar,     Unit::SmallCargo)     =>   250,
        (Unit::Deathstar,     Unit::LargeCargo)     =>   250,
        (Unit::Deathstar,     Unit::LightFighter)   =>   200,
        (Unit::Deathstar,     Unit::HeavyFighter)   =>   100,
        (Unit::Deathstar,     Unit::Cruiser)        =>    33,
        (Unit::Deathstar,     Unit::Battleship)     =>    30,
        (Unit::Deathstar,     Unit::ColonyShip)     =>   250,
        (Unit::Deathstar,     Unit::Recycler)       =>   250,
        (Unit::Deathstar,     Unit::EspionageProbe) => 1_250,
        (Unit::Deathstar,     Unit::SolarSatellite) => 1_250,
        (Unit::Deathstar,     Unit::Bomber)         =>    25,
        (Unit::Deathstar,     Unit::Destroyer)      =>     5,
        (Unit::Deathstar,     Unit::RocketLauncher) =>   200,
        (Unit::Deathstar,     Unit::LightLaser)     =>   200,
        (Unit::Deathstar,     Unit::HeavyLaser)     =>   100,
        (Unit::Deathstar,     Unit::GaussCannon)    =>    50,
        (Unit::Deathstar,     Unit::IonCannon)      =>   100,
        (Unit::Deathstar,     Unit::Battlecruiser)  =>    15,
        (Unit::Battlecruiser, Unit::EspionageProbe) =>     5,
        (Unit::Battlecruiser, Unit::SolarSatellite) =>     5,
        (Unit::Battlecruiser, Unit::SmallCargo)     =>     3,
        (Unit::Battlecruiser, Unit::LargeCargo)     =>     3,
        (Unit::Battlecruiser, Unit::HeavyFighter)   =>     4,
        (Unit::Battlecruiser, Unit::Cruiser)        =>     4,
        (Unit::Battlecruiser, Unit::Battleship)     =>     7,
        _                                           =>     0,
    }
}

fn get_unit_price(unit: Unit) -> Price {
    match unit {
        Unit::SmallCargo      => Price{metal:     2_000, crystal:     2_000, deuterium:         0},
        Unit::LargeCargo      => Price{metal:     6_000, crystal:     6_000, deuterium:         0},
        Unit::LightFighter    => Price{metal:     3_000, crystal:     1_000, deuterium:         0},
        Unit::HeavyFighter    => Price{metal:     6_000, crystal:     4_000, deuterium:         0},
        Unit::Cruiser         => Price{metal:    20_000, crystal:     7_000, deuterium:     2_000},
        Unit::Battleship      => Price{metal:    45_000, crystal:    15_000, deuterium:         0},
        Unit::ColonyShip      => Price{metal:    10_000, crystal:    20_000, deuterium:    10_000},
        Unit::Recycler        => Price{metal:    10_000, crystal:     6_000, deuterium:     2_000},
        Unit::EspionageProbe  => Price{metal:         0, crystal:     1_000, deuterium:         0},
        Unit::Bomber          => Price{metal:    50_000, crystal:    25_000, deuterium:    15_000},
        Unit::SolarSatellite  => Price{metal:         0, crystal:     2_000, deuterium:       500},
        Unit::Destroyer       => Price{metal:    60_000, crystal:    50_000, deuterium:    15_000},
        Unit::Deathstar       => Price{metal: 5_000_000, crystal: 4_000_000, deuterium: 1_000_000},
        Unit::Battlecruiser   => Price{metal:    30_000, crystal:    40_000, deuterium:    15_000},
        Unit::RocketLauncher  => Price{metal:     2_000, crystal:         0, deuterium:         0},
        Unit::LightLaser      => Price{metal:     1_500, crystal:       500, deuterium:         0},
        Unit::HeavyLaser      => Price{metal:     6_000, crystal:     2_000, deuterium:         0},
        Unit::GaussCannon     => Price{metal:    20_000, crystal:    15_000, deuterium:     2_000},
        Unit::IonCannon       => Price{metal:     2_000, crystal:     6_000, deuterium:         0},
        Unit::PlasmaTurret    => Price{metal:    50_000, crystal:    50_000, deuterium:    30_000},
        Unit::SmallShieldDome => Price{metal:    10_000, crystal:    10_000, deuterium:         0},
        Unit::LargeShieldDome => Price{metal:    50_000, crystal:    50_000, deuterium:         0},
    }
}

fn get_unit_base_shield(unit: Unit) -> u32 {
    match unit {
        Unit::SmallCargo      =>     10,
        Unit::LargeCargo      =>     25,
        Unit::LightFighter    =>     10,
        Unit::HeavyFighter    =>     25,
        Unit::Cruiser         =>     50,
        Unit::Battleship      =>    200,
        Unit::ColonyShip      =>    100,
        Unit::Recycler        =>     10,
        Unit::EspionageProbe  =>      1, // 0.01
        Unit::Bomber          =>    500,
        Unit::SolarSatellite  =>      1,
        Unit::Destroyer       =>    500,
        Unit::Deathstar       => 50_000,
        Unit::Battlecruiser   =>    400,
        Unit::RocketLauncher  =>     20,
        Unit::LightLaser      =>     25,
        Unit::HeavyLaser      =>    100,
        Unit::GaussCannon     =>    200,
        Unit::IonCannon       =>    500,
        Unit::PlasmaTurret    =>    300,
        Unit::SmallShieldDome =>  2_000,
        Unit::LargeShieldDome => 10_000,
    }
}

fn get_unit_base_weapon(unit: Unit) -> u32 {
    match unit {
        Unit::SmallCargo      =>       5,
        Unit::LargeCargo      =>       5,
        Unit::LightFighter    =>      50,
        Unit::HeavyFighter    =>     150,
        Unit::Cruiser         =>     400,
        Unit::Battleship      =>   1_000,
        Unit::ColonyShip      =>      50,
        Unit::Recycler        =>       1,
        Unit::EspionageProbe  =>       1, // 0.01
        Unit::Bomber          =>   1_000,
        Unit::SolarSatellite  =>       1,
        Unit::Destroyer       =>   2_000,
        Unit::Deathstar       => 200_000,
        Unit::Battlecruiser   =>     700,
        Unit::RocketLauncher  =>      80,
        Unit::LightLaser      =>     100,
        Unit::HeavyLaser      =>     250,
        Unit::GaussCannon     =>   1_100,
        Unit::IonCannon       =>     150,
        Unit::PlasmaTurret    =>   3_000,
        Unit::SmallShieldDome =>       1,
        Unit::LargeShieldDome =>       1,
    }
}

fn get_unit_name(unit: Unit) -> &'static str {
    match unit {
        Unit::SmallCargo      => "Small cargo",
        Unit::LargeCargo      => "Large cargo",
        Unit::LightFighter    => "Light fighter",
        Unit::HeavyFighter    => "Heavy fighter",
        Unit::Cruiser         => "Cruiser",
        Unit::Battleship      => "Battleship",
        Unit::ColonyShip      => "Colony ship",
        Unit::Recycler        => "Recycler",
        Unit::EspionageProbe  => "Espionage probe",
        Unit::Bomber          => "Bomber",
        Unit::SolarSatellite  => "Solar satellite",
        Unit::Destroyer       => "Destroyer",
        Unit::Deathstar       => "Deathstar",
        Unit::Battlecruiser   => "Battlecruiser",
        Unit::RocketLauncher  => "Rocket launcher",
        Unit::LightLaser      => "Light laser",
        Unit::HeavyLaser      => "Heavy laser",
        Unit::GaussCannon     => "Gauss cannon",
        Unit::IonCannon       => "Ion cannon",
        Unit::PlasmaTurret    => "Plasma turret",
        Unit::SmallShieldDome => "Small shield dome",
        Unit::LargeShieldDome => "Large shield dome",
    }
}

fn get_unit_weapon_power(unit: Unit, weapon_techno: u8) -> u32 {
    (get_unit_base_weapon(unit) as f64 * (1.0 + 0.1 * weapon_techno as f64)) as u32
}

fn get_unit_initial_shield(unit: Unit, shield_techno: u8) -> u32 {
    (get_unit_base_shield(unit) as f64 * (1.0 + 0.1 * shield_techno as f64)) as u32
}

fn get_unit_initial_hull_plating(armour_techno: u8, metal_price: u64, crystal_price: u64) -> u64 {
  (1 + (armour_techno as u64 / 10)) * ((metal_price + crystal_price) / 10)
}

fn is_ship(unit: Unit) -> bool {
    match unit {
        Unit::SmallCargo     => true,
        Unit::LargeCargo     => true,
        Unit::LightFighter   => true,
        Unit::HeavyFighter   => true,
        Unit::Cruiser        => true,
        Unit::Battleship     => true,
        Unit::ColonyShip     => true,
        Unit::Recycler       => true,
        Unit::EspionageProbe => true,
        Unit::Bomber         => true,
        Unit::SolarSatellite => true,
        Unit::Destroyer      => true,
        Unit::Deathstar      => true,
        Unit::Battlecruiser  => true,
        _                    => false,
    }
}

fn roll_dice() -> f64 {
    rand::random::<f64>()
}

#[derive(Debug, Deserialize)]
#[allow(non_snake_case)]
struct Config {
    Simulations: u32,
    Attacker: Attacker,
    Defender: Defender,
}

#[derive(Debug, Deserialize)]
#[allow(non_snake_case)]
struct Attacker {
    Weapon: u8,
    Shield: u8,
    Armour: u8,
    Combustion: u8,
    Impulse: u8,
    Hyperspace: u8,
    SmallCargo: Option<u32>,
    LargeCargo: Option<u32>,
    LightFighter: Option<u32>,
    HeavyFighter: Option<u32>,
    Cruiser: Option<u32>,
    Battleship: Option<u32>,
    ColonyShip: Option<u32>,
    Recycler: Option<u32>,
    EspionageProbe: Option<u32>,
    Bomber: Option<u32>,
    SolarSatellite: Option<u32>,
    Destroyer: Option<u32>,
    Deathstar: Option<u32>,
    Battlecruiser: Option<u32>,
    RocketLauncher: Option<u32>,
    LightLaser: Option<u32>,
    HeavyLaser: Option<u32>,
    GaussCannon: Option<u32>,
    IonCannon: Option<u32>,
    PlasmaTurret: Option<u32>,
    SmallShieldDome: Option<u32>,
    LargeShieldDome: Option<u32>,
}

#[derive(Debug, Deserialize)]
#[allow(non_snake_case)]
struct Defender {
    Weapon: u8,
    Shield: u8,
    Armour: u8,
    SmallCargo: Option<u32>,
    LargeCargo: Option<u32>,
    LightFighter: Option<u32>,
    HeavyFighter: Option<u32>,
    Cruiser: Option<u32>,
    Battleship: Option<u32>,
    ColonyShip: Option<u32>,
    Recycler: Option<u32>,
    EspionageProbe: Option<u32>,
    Bomber: Option<u32>,
    SolarSatellite: Option<u32>,
    Destroyer: Option<u32>,
    Deathstar: Option<u32>,
    Battlecruiser: Option<u32>,
    RocketLauncher: Option<u32>,
    LightLaser: Option<u32>,
    HeavyLaser: Option<u32>,
    GaussCannon: Option<u32>,
    IonCannon: Option<u32>,
    PlasmaTurret: Option<u32>,
    SmallShieldDome: Option<u32>,
    LargeShieldDome: Option<u32>,
}

fn new_attacker(config: &Config) -> Entity {
    let mut entity = Entity{..Default::default()};
    config_attacker(&mut entity, config);
    reset_entity(&mut entity);
    entity.units = Vec::new();
    entity
}

fn new_defender(config: &Config) -> Entity {
    let mut entity = Entity{..Default::default()};
    config_defender(&mut entity, config);
    reset_entity(&mut entity);
    entity.units = Vec::new();
    entity
}

fn config_attacker(e: &mut Entity, config: &Config) {
    e.weapon            = config.Attacker.Weapon;
    e.shield            = config.Attacker.Shield;
    e.armour            = config.Attacker.Armour;
    e.combustion        = config.Attacker.Combustion;
    e.impulse           = config.Attacker.Impulse;
    e.hyperspace        = config.Attacker.Hyperspace;
    e.small_cargo       = config.Attacker.SmallCargo.unwrap_or(0);
    e.large_cargo       = config.Attacker.LargeCargo.unwrap_or(0);
    e.light_fighter     = config.Attacker.LightFighter.unwrap_or(0);
    e.heavy_fighter     = config.Attacker.HeavyFighter.unwrap_or(0);
    e.cruiser           = config.Attacker.Cruiser.unwrap_or(0);
    e.battleship        = config.Attacker.Battleship.unwrap_or(0);
    e.colony_ship       = config.Attacker.ColonyShip.unwrap_or(0);
    e.recycler          = config.Attacker.Recycler.unwrap_or(0);
    e.espionage_probe   = config.Attacker.EspionageProbe.unwrap_or(0);
    e.bomber            = config.Attacker.Bomber.unwrap_or(0);
    e.solar_satellite   = config.Attacker.SolarSatellite.unwrap_or(0);
    e.destroyer         = config.Attacker.Destroyer.unwrap_or(0);
    e.deathstar         = config.Attacker.Deathstar.unwrap_or(0);
    e.battlecruiser     = config.Attacker.Battlecruiser.unwrap_or(0);
    e.rocket_launcher   = config.Attacker.RocketLauncher.unwrap_or(0);
    e.light_laser       = config.Attacker.LightLaser.unwrap_or(0);
    e.heavy_laser       = config.Attacker.HeavyLaser.unwrap_or(0);
    e.gauss_cannon      = config.Attacker.GaussCannon.unwrap_or(0);
    e.ion_cannon        = config.Attacker.IonCannon.unwrap_or(0);
    e.plasma_turret     = config.Attacker.PlasmaTurret.unwrap_or(0);
    e.small_shield_dome = config.Attacker.SmallShieldDome.unwrap_or(0);
    e.large_shield_dome = config.Attacker.LargeShieldDome.unwrap_or(0);
}

fn config_defender(e: &mut Entity, config: &Config) {
    e.weapon            = config.Defender.Weapon;
    e.shield            = config.Defender.Shield;
    e.armour            = config.Defender.Armour;
    e.combustion        = 0;
    e.impulse           = 0;
    e.hyperspace        = 0;
    e.small_cargo       = config.Defender.SmallCargo.unwrap_or(0);
    e.large_cargo       = config.Defender.LargeCargo.unwrap_or(0);
    e.light_fighter     = config.Defender.LightFighter.unwrap_or(0);
    e.heavy_fighter     = config.Defender.HeavyFighter.unwrap_or(0);
    e.cruiser           = config.Defender.Cruiser.unwrap_or(0);
    e.battleship        = config.Defender.Battleship.unwrap_or(0);
    e.colony_ship       = config.Defender.ColonyShip.unwrap_or(0);
    e.recycler          = config.Defender.Recycler.unwrap_or(0);
    e.espionage_probe   = config.Defender.EspionageProbe.unwrap_or(0);
    e.bomber            = config.Defender.Bomber.unwrap_or(0);
    e.solar_satellite   = config.Defender.SolarSatellite.unwrap_or(0);
    e.destroyer         = config.Defender.Destroyer.unwrap_or(0);
    e.deathstar         = config.Defender.Deathstar.unwrap_or(0);
    e.battlecruiser     = config.Defender.Battlecruiser.unwrap_or(0);
    e.rocket_launcher   = config.Defender.RocketLauncher.unwrap_or(0);
    e.light_laser       = config.Defender.LightLaser.unwrap_or(0);
    e.heavy_laser       = config.Defender.HeavyLaser.unwrap_or(0);
    e.gauss_cannon      = config.Defender.GaussCannon.unwrap_or(0);
    e.ion_cannon        = config.Defender.IonCannon.unwrap_or(0);
    e.plasma_turret     = config.Defender.PlasmaTurret.unwrap_or(0);
    e.small_shield_dome = config.Defender.SmallShieldDome.unwrap_or(0);
    e.large_shield_dome = config.Defender.LargeShieldDome.unwrap_or(0);
}

fn new_simulator(config: &Config) -> Simulator {
    let attacker = new_attacker(&config);
    let defender = new_defender(&config);
    Simulator{attacker, defender, ..Default::default()}
}

fn reset_entity(entity: &mut Entity) {
    entity.losses = Price{metal: 0, crystal: 0, deuterium: 0};
    entity.total_units = 0;
    entity.total_units += entity.small_cargo;
    entity.total_units += entity.large_cargo;
    entity.total_units += entity.light_fighter;
    entity.total_units += entity.heavy_fighter;
    entity.total_units += entity.cruiser;
    entity.total_units += entity.battleship;
    entity.total_units += entity.colony_ship;
    entity.total_units += entity.recycler;
    entity.total_units += entity.espionage_probe;
    entity.total_units += entity.bomber;
    entity.total_units += entity.solar_satellite;
    entity.total_units += entity.destroyer;
    entity.total_units += entity.deathstar;
    entity.total_units += entity.battlecruiser;
    entity.total_units += entity.rocket_launcher;
    entity.total_units += entity.light_laser;
    entity.total_units += entity.heavy_laser;
    entity.total_units += entity.gauss_cannon;
    entity.total_units += entity.ion_cannon;
    entity.total_units += entity.plasma_turret;
    entity.total_units += entity.small_shield_dome;
    entity.total_units += entity.large_shield_dome;
}

fn new_unit(armour: u8, shield: u8, unit: Unit) -> CombatUnit {
    let unit_price = get_unit_price(unit);
    let mut combat_unit = CombatUnit{packed_infos: 0};
    set_id(&mut combat_unit, unit);
    set_hull(&mut combat_unit, get_unit_initial_hull_plating(armour, unit_price.metal, unit_price.crystal));
    set_shield(&mut combat_unit, get_unit_initial_shield(unit, shield));
    combat_unit
}

fn init_entity(entity: &mut Entity) {
    println!("start INIT");
    reset_entity(entity);
    println!("alloc {}", entity.total_units);
    entity.units = Vec::with_capacity(entity.total_units as usize);
    println!("done alloc");

    let armour = entity.armour;
    let shield = entity.shield;

    for _ in 0..entity.small_cargo {
        entity.units.push(new_unit(armour, shield, Unit::SmallCargo));
    }
    for _ in 0..entity.large_cargo {
        entity.units.push(new_unit(armour, shield, Unit::LargeCargo));
    }
    for _ in 0..entity.light_fighter {
        entity.units.push(new_unit(armour, shield, Unit::LightFighter));
    }
    for _ in 0..entity.heavy_fighter {
        entity.units.push(new_unit(armour, shield, Unit::HeavyFighter));
    }
    for _ in 0..entity.cruiser {
        entity.units.push(new_unit(armour, shield, Unit::Cruiser));
    }
    for _ in 0..entity.battleship {
        entity.units.push(new_unit(armour, shield, Unit::Battleship));
    }
    for _ in 0..entity.colony_ship {
        entity.units.push(new_unit(armour, shield, Unit::ColonyShip));
    }
    for _ in 0..entity.recycler {
        entity.units.push(new_unit(armour, shield, Unit::Recycler));
    }
    for _ in 0..entity.espionage_probe {
        entity.units.push(new_unit(armour, shield, Unit::EspionageProbe));
    }
    for _ in 0..entity.bomber {
        entity.units.push(new_unit(armour, shield, Unit::Bomber));
    }
    for _ in 0..entity.solar_satellite {
        entity.units.push(new_unit(armour, shield, Unit::SolarSatellite));
    }
    for _ in 0..entity.destroyer {
        entity.units.push(new_unit(armour, shield, Unit::Destroyer));
    }
    for _ in 0..entity.deathstar {
        entity.units.push(new_unit(armour, shield, Unit::Deathstar));
    }
    for _ in 0..entity.battlecruiser {
        entity.units.push(new_unit(armour, shield, Unit::Battlecruiser));
    }
    for _ in 0..entity.rocket_launcher {
        entity.units.push(new_unit(armour, shield, Unit::RocketLauncher));
    }
    for _ in 0..entity.light_laser {
        entity.units.push(new_unit(armour, shield, Unit::LightLaser));
    }
    for _ in 0..entity.heavy_laser {
        entity.units.push(new_unit(armour, shield, Unit::HeavyLaser));
    }
    for _ in 0..entity.gauss_cannon {
        entity.units.push(new_unit(armour, shield, Unit::GaussCannon));
    }
    for _ in 0..entity.ion_cannon {
        entity.units.push(new_unit(armour, shield, Unit::IonCannon));
    }
    for _ in 0..entity.plasma_turret {
        entity.units.push(new_unit(armour, shield, Unit::PlasmaTurret));
    }
    for _ in 0..entity.small_shield_dome {
        entity.units.push(new_unit(armour, shield, Unit::SmallShieldDome));
    }
    for _ in 0..entity.large_shield_dome {
        entity.units.push(new_unit(armour, shield, Unit::LargeShieldDome));
    }
    println!("done INIT");
}

fn attack(attacker: &Entity, unit: &CombatUnit, defender_armour: u8, defender_weapon: u8, target_unit: &mut CombatUnit) {
    if SHOULD_LOG {
        let attacking_str = get_unit_name(get_id(unit));
        let target_str = get_unit_name(get_id(target_unit));
        print!("{} fires at {}; ", attacking_str, target_str);
    }
    let mut weapon = get_unit_weapon_power(get_id(unit), attacker.weapon);
    // Check for shot bounce
    if weapon * 100 < get_shield(unit) {
        if SHOULD_LOG {
            println!("shot bounced");
        }
        return;
    }

    // Attack target
    let current_hull = get_hull(target_unit);
    let current_shield = get_shield(target_unit);
    if current_shield < weapon {
        weapon -= current_shield;
        set_shield(target_unit, 0);
        if current_hull < weapon {
            set_hull(target_unit, 0);
        } else {
            set_hull(target_unit, (current_hull - weapon) as u64);
        }
    } else {
        set_shield(target_unit, current_shield - weapon);
    }
    if SHOULD_LOG {
        let target_str = unit_2_str(target_unit, defender_weapon);
        println!("result is {}", target_str);
    }
    // Check for explosion
    if is_alive(target_unit) {
        if has_exploded(defender_armour, target_unit) {
            set_hull(target_unit, 0);
        }
    }
}

fn has_exploded(armour: u8, unit: &CombatUnit) -> bool {
    let mut exploded = false;
    let price = get_unit_price(get_id(unit));
    let unit_initial_hull_plating = get_unit_initial_hull_plating(armour, price.metal, price.crystal);
    let hull_percentage = get_hull(unit) as f64 / unit_initial_hull_plating as f64;
    if hull_percentage < 0.7 {
        let probability_of_exploding = 1.0 - hull_percentage;
        let dice = roll_dice();
        if SHOULD_LOG {
            print!("probability of exploding of {:.3}%: dice value of {:.3} comparing with {:.3}: ", probability_of_exploding*100.0, dice, 1.0-probability_of_exploding);
        }
        if dice >= 1.0 - probability_of_exploding {
            exploded = true;
            if SHOULD_LOG {
                println!("unit exploded.");
            }
        } else {
            if SHOULD_LOG {
                println!("unit didn't explode.");
            }
        }
    }
    exploded
}

fn unit_2_str(unit: &CombatUnit, weapon: u8) -> String {
    let unit_name = get_unit_name(get_id(unit));
    let weapon = get_unit_weapon_power(get_id(unit), weapon);
    format!("{} with {} {} {}", unit_name, get_hull(unit), get_shield(unit), weapon)
}

fn get_another_shot(unit: &CombatUnit, target_unit: &CombatUnit) -> bool {
    let mut rapid_fire = true;
    let rf = get_rapid_fire_against(unit, target_unit);
    if rf > 0 {
        let chance = (rf - 1) as f64 / rf as f64;
        let dice = roll_dice();
        if SHOULD_LOG {
            print!("dice was {:.3}, comparing with {:.3}: ", dice, chance);
        }
        if dice <= chance {
            if SHOULD_LOG {
                println!("{} gets another shot.", get_unit_name(get_id(unit)));
            }
        } else {
            if SHOULD_LOG {
                println!("{} does not get another shot.", get_unit_name(get_id(unit)));
            }
            rapid_fire = false;
        }
    } else {
        if SHOULD_LOG {
            println!("{} doesn't have rapid fire against {}.", get_unit_name(get_id(unit)), get_unit_name(get_id(target_unit)));
        }
        rapid_fire = false;
    }
    rapid_fire
}

fn units_fire(attacker: &Entity, defender: &mut Entity) {
    for i in 0..attacker.total_units {
        let unit = &attacker.units[i as usize];
        let mut rapid_fire = true;
        while rapid_fire {
            let random = rand::random::<u32>() % defender.total_units;
            let target_unit = &mut defender.units[random as usize];
            if is_alive(target_unit) {
                attack(attacker, unit, defender.armour, defender.weapon, target_unit);
            }
            rapid_fire = get_another_shot(unit, target_unit);
        }
    }
}

fn simulate(simulator: &mut Simulator) {
    init_entity(&mut simulator.attacker);
    init_entity(&mut simulator.defender);
    for current_round in 0..simulator.max_rounds {
        simulator.rounds = current_round;
        println!("CALISSSS");
        if SHOULD_LOG {
            println!("-------------");
            println!("ROUND {}", current_round);
            println!("-------------");
        }
        println!("ATT");
        units_fire(&mut simulator.attacker, &mut simulator.defender);
        println!("DEF");
        units_fire(&mut simulator.defender, &mut simulator.attacker);
        println!("REM");
        remove_destroyed_units(simulator);
        println!("RESTORE");
        restore_shields(&mut simulator.attacker);
        restore_shields(&mut simulator.defender);
        if is_combat_done(&simulator.attacker, &simulator.defender) {
            break;
        }
    }
    print_winner(simulator);
}

fn print_winner(simulator: &mut Simulator) {
    if simulator.defender.total_units == 0 && simulator.attacker.total_units == 0 {
        simulator.winner = 2;
        println!("The battle ended draw.");
    } else if simulator.defender.total_units == 0 {
        simulator.winner = 0;
        println!("The battle ended after {} rounds with attacker winning", simulator.rounds);
    } else if simulator.attacker.total_units == 0 {
        simulator.winner = 1;
        println!("The battle ended after {} rounds with defender winning", simulator.rounds);
    } else {
        simulator.winner = 2;
        println!("The battle ended draw.");
    }
}

fn is_combat_done(attacker: &Entity, defender: &Entity) -> bool {
    defender.total_units == 0 || attacker.total_units == 0
}

fn restore_shields(entity: &mut Entity) {
    let l = entity.total_units;
    for i in (0..l).rev() {
        if SHOULD_LOG {
            let unit = &entity.units[i as usize];
            let unit_str = unit_2_str(unit, entity.weapon);
            println!("{} still has integrity, restore its shield", unit_str);
        }
        let unit = &mut entity.units[i as usize];
        let criss = get_id(unit);
        set_shield(unit, get_unit_initial_shield(criss, entity.shield));
    }
}

fn remove_entity_destroyed_units(simulator: &mut Simulator, criss: u32) {
    let entity: &mut Entity;
    if criss == 0 {
        entity = &mut simulator.attacker;
    } else {
        entity = &mut simulator.defender;
    }
    let l = entity.total_units;
    for i in (0..l).rev() {
        let unit = entity.units[i as usize];
        if get_hull(&unit) <= 0 {
            let unit_price = get_unit_price(get_id(&unit));
            if is_ship(get_id(&unit)) {
                simulator.debris.metal += (simulator.fleet_to_debris * unit_price.metal as f64) as u64;
                simulator.debris.crystal += (simulator.fleet_to_debris * unit_price.crystal as f64) as u64;
            }
            entity.losses.metal += unit_price.metal;
            entity.losses.crystal += unit_price.crystal;
            entity.losses.deuterium += unit_price.deuterium;
            entity.units.swap(i as usize, (entity.total_units - 1) as usize);
            entity.total_units -= 1;
        }
    }
}

fn remove_destroyed_units(simulator: &mut Simulator) {
    remove_entity_destroyed_units(simulator, 0);
    remove_entity_destroyed_units(simulator, 1);
}

fn pretty_print_results(result: &Result) {
    println!("Results ({} simulations | ~{} rounds)", result.simulations, result.rounds);
    println!("Attacker win: {}%", result.attacker_win);
    println!("Defender win: {}%", result.defender_win);
    println!("Draw: {}%", result.draw);
    println!("Attacker losses: {}, {}, {}", result.attacker_losses.metal, result.attacker_losses.crystal, result.attacker_losses.deuterium);
    println!("Defender losses: {}, {}, {}", result.defender_losses.metal, result.defender_losses.crystal, result.defender_losses.deuterium);
    println!("Debris: {}, {}, {}", result.debris.metal, result.debris.crystal, result.debris.deuterium);
    println!("Recycler: {}", result.recycler);
    println!("Moonchance: {}%\n", result.moon_chance);
}

struct Result {
    simulations: u32,
    rounds: u32,
    attacker_win: u32,
    defender_win: u32,
    draw: u32,
    recycler: u32,
    moon_chance: u32,
    attacker_losses: Price,
    defender_losses: Price,
    debris: Price,
}

fn main() {

    let mut file = match File::open("/Users/agilbert/go/src/ogame_combat_simulator/rust/src/big_attack.toml") {
        Ok(file) => file,
        Err(_)  => {
            panic!("Could not find config file, using default!");
        }
    };
    let mut config_toml = String::new();
    match file.read_to_string(&mut config_toml) {
        Ok(config_str) => config_str,
        Err(err) => {
            panic!("{}", err);
        }
    };

    let config: Config = match toml::from_str(&config_toml) {
        Ok(config) => config,
        Err(err) => {
            panic!("NOT OK {}", err);
        }
    };

    let mut s = new_simulator(&config);
    s.fleet_to_debris = 0.3;
    s.max_rounds = 6;

    let mut attacker_win = 0;
    let mut defender_win = 0;
    let mut draw = 0;
    let mut rounds = 0;
    let mut moon_chance = 0;
    let mut attacker_losses = Price{metal: 0, crystal: 0, deuterium: 0};
    let mut defender_losses = Price{metal: 0, crystal: 0, deuterium: 0};
    let mut debris = Price{metal: 0, crystal: 0, deuterium: 0};
    let nb_simulations = config.Simulations;

    for _ in 0..nb_simulations {
        s.rounds = 0;
        s.debris = Price{metal: 0, crystal: 0, deuterium: 0};
        simulate(&mut s);
        if s.winner == 0 {
            attacker_win += 1;
        } else if s.winner == 1 {
            defender_win += 1;
        } else {
            draw += 1;
        }
        attacker_losses.metal += s.attacker.losses.metal;
        attacker_losses.crystal += s.attacker.losses.crystal;
        attacker_losses.deuterium += s.attacker.losses.deuterium;

        defender_losses.metal += s.defender.losses.metal;
        defender_losses.crystal += s.defender.losses.crystal;
        defender_losses.deuterium += s.defender.losses.deuterium;

        debris.metal += s.debris.metal;
        debris.crystal += s.debris.crystal;
        debris.deuterium += s.debris.deuterium;

        rounds += s.rounds;

        let debris_total = s.debris.metal + s.debris.crystal;
        moon_chance += cmp::min((debris_total as f64 / 100000.0) as u64, 20) as u32;
    }

    let result = Result{
        simulations: nb_simulations,
        rounds: rounds / nb_simulations,
        attacker_win: (attacker_win as f64 / nb_simulations as f64 * 100.0) as u32,
        defender_win: (defender_win as f64 / nb_simulations as f64 * 100.0) as u32,
        draw: (draw as f64 / nb_simulations as f64 * 100.0) as u32,
        recycler: (((debris.metal + debris.crystal) as f64 / nb_simulations as f64) / 20000.0).ceil() as u32,
        moon_chance: (moon_chance as f64 / nb_simulations as f64) as u32,
        attacker_losses: Price{
            metal: (attacker_losses.metal as f64 / nb_simulations as f64) as u64,
            crystal: (attacker_losses.crystal as f64 / nb_simulations as f64) as u64,
            deuterium: (attacker_losses.deuterium as f64 / nb_simulations as f64) as u64
        },
        defender_losses: Price{
            metal: (defender_losses.metal as f64 / nb_simulations as f64) as u64,
            crystal: (defender_losses.crystal as f64 / nb_simulations as f64) as u64,
            deuterium: (defender_losses.deuterium as f64 / nb_simulations as f64) as u64
        },
        debris: Price{
            metal: (debris.metal as f64 / nb_simulations as f64) as u64,
            crystal: (debris.crystal as f64 / nb_simulations as f64) as u64,
            deuterium: (debris.deuterium as f64 / nb_simulations as f64) as u64
        },
    };

    pretty_print_results(&result);
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn set_hull() {
        let mut cu = CombatUnit{packed_infos: 0};
        cu.set_hull(123);
        assert_eq!(123, cu.get_hull());
    }

    #[test]
    fn set_id() {
        let mut cu = CombatUnit{packed_infos: 0};
        cu.set_id(Unit::HeavyFighter);
        assert_eq!(Unit::HeavyFighter as u8, cu.get_id() as u8);
    }

    #[test]
    fn set_shield() {
        let mut cu = CombatUnit{packed_infos: 0};
        cu.set_shield(123);
        assert_eq!(123, cu.get_shield());
    }

    #[test]
    fn is_alive() {
        let mut cu = CombatUnit{packed_infos: 0};
        assert_eq!(false, cu.is_alive());
        cu.set_hull(123);
        assert_eq!(true, cu.is_alive());
    }

    #[test]
    fn rocket_launcher_is_not_a_ship() {
        assert_eq!(false, is_ship(Unit::RocketLauncher));
    }

    #[test]
    fn small_cargo_is_a_ship() {
        assert_eq!(true, is_ship(Unit::SmallCargo));
    }

    #[test]
    fn get_unit_initial_shield_small_cargo() {
        assert_eq!(10, get_unit_initial_shield(Unit::SmallCargo, 0));
        assert_eq!(20, get_unit_initial_shield(Unit::SmallCargo, 10));
    }

    #[test]
    fn get_unit_initial_hull_plating_small_cargo() {
        assert_eq!(400, get_unit_initial_hull_plating(0, 2000, 2000));
    }

    #[test]
    fn get_rapid_fire_against_sc_sc() {
        let mut cu1 = CombatUnit{packed_infos: 0};
        let mut cu2 = CombatUnit{packed_infos: 0};
        cu1.set_id(Unit::SmallCargo);
        cu2.set_id(Unit::SmallCargo);
        assert_eq!(0, get_rapid_fire_against(&cu1, &cu2));
    }

    #[test]
    fn get_rapid_fire_against_sc_rl() {
        let mut cu1 = CombatUnit{packed_infos: 0};
        let mut cu2 = CombatUnit{packed_infos: 0};
        cu1.set_id(Unit::SmallCargo);
        cu2.set_id(Unit::EspionageProbe);
        assert_eq!(5, get_rapid_fire_against(&cu1, &cu2));
    }
}
