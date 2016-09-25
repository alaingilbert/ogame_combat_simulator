from units.Ship import Ship
from units.Units import Price

class Deathstar(Ship):
    OgameID = 214
    Name = "Deathstar"
    Price = Price(5000000, 4000000, 1000000)
    ShieldPower = 50000
    WeaponPower = 200000
    CargoCapacity = 1000000
    BaseSpeed = 100
    FuelComsumption = 1
    RapidfireAgainst = {"SmallCargo": 250,
        "LargeCargo": 250, "LightFighter": 200, "HeavyFighter": 100, "Cruiser": 33,
        "Battleship": 30, "ColonyShip": 250, "Recycler": 250, "EspionageProbe": 1250,
        "SolarSatellite": 1250, "Bomber": 25, "Destroyer": 5, "RocketLauncher": 200,
        "LightLaser": 200, "HeavyLaser": 100, "GaussCannon": 50, "IonCannon": 100,
        "Battlecruiser": 15}

    def __init__(self):
        self.InitHullPlating()
        self.RestoreShield()
