package config

func GenerateConfig() []byte {
	configFile := `# OGame Combat Simulator config file
Simulations     = 1
IsLogging       = false
Workers         = 4

[Attacker]
Weapon          = 0
Shield          = 0
Armour          = 0
SmallCargo      = 0
LargeCargo      = 0
LightFighter    = 0
HeavyFighter    = 0
Cruiser         = 0
Battleship      = 0
ColonyShip      = 0
Recycler        = 0
EspionageProbe  = 0
Bomber          = 0
Destroyer       = 0
Deathstar       = 0
Battlecruiser   = 0

[Defender]
Weapon          = 0
Shield          = 0
Armour          = 0
SmallCargo      = 0
LargeCargo      = 0
LightFighter    = 0
HeavyFighter    = 0
Cruiser         = 0
Battleship      = 0
ColonyShip      = 0
Recycler        = 0
EspionageProbe  = 0
Bomber          = 0
SolarSatellite  = 0
Destroyer       = 0
Deathstar       = 0
Battlecruiser   = 0
RocketLauncher  = 0
LightLaser      = 0
HeavyLaser      = 0
GaussCannon     = 0
IonCannon       = 0
PlasmaTurret    = 0
SmallShieldDome = 0
LargeShieldDome = 0`
	return []byte(configFile)
}
