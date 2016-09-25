from units.Units import CombatUnit
from units.Units import Price

class RocketLauncher(CombatUnit):
    OgameID = 401
    Name = "RocketLauncher"
    Price = Price(2000, 0, 0)
    ShieldPower = 20
    WeaponPower = 80
    RapidfireFrom = {"Bomber": 20, "Cruiser": 10, "Deathstar": 200}

    def __init__(self):
        self.InitHullPlating()
        self.RestoreShield()
