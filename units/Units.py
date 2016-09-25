class Price(object):
    Metal = 0
    Crystal = 0
    Deuterium = 0

    def __init__(self, metal=0, crystal=0, deut=0):
        self.Metal = metal
        self.Crystal = crystal
        self.Deuterium = deut

    def Add(self, price):
        self.Metal += price.Metal
        self.Crystal += price.Crystal
        self.Deuterium += price.Deuterium


class CombatUnit(object):
    Price = Price()
    OgameID = 0
    Name = ""
    HullPlating = 0
    Shield = 0
    ShieldPower = 0
    WeaponPower = 0
    ArmourTechnology = 0
    WeaponTechnology = 0
    ShieldTechnology = 0
    RapidfireFrom = {}
    RapidfireAgainst = {}

    def IsDead(self):
        return self.HullPlating <= 0

    def IsAlive(self):
        return self.HullPlating > 0

    def GetPrice(self):
        return self.Price

    def GetRapidfireAgainst(self, name):
        return self.RapidfireAgainst.get(name, 0)

    def InitHullPlating(self):
        self.HullPlating = (1 + (float(self.ArmourTechnology) / 10)) * ((float(self.Price.Metal) + float(self.Price.Crystal)) / 10)

    def GetInitialHullPlating(self):
        return (1 + (self.ArmourTechnology / 10)) * ((self.Price.Metal + self.Price.Crystal) / 10)

    def GetHullPlating(self):
        return self.HullPlating

    def SetHullPlating(self, hullPlating):
        self.HullPlating = hullPlating

    def GetName(self):
        return self.Name

    def GetWeapon(self):
        return self.WeaponPower

    def GetShield(self):
        return self.Shield

    def SetShield(self, shield):
        self.Shield = shield

    def RestoreShield(self):
        self.Shield = self.ShieldPower * (1 + 0.1*0)

    def String(self):
        return "%s with %d:%d:%d" % (self.GetName(), self.GetHullPlating(), self.GetShield(), self.GetWeapon())
