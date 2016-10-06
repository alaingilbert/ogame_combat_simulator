# Ogame combat simulator

### Build

```sh
make
```

### Execute

```sh
./main -c config.ini
```

```
Simulations 20/20

Results (20 simulations | ~2 rounds)
Attacker win: 90%
Defender win: 5%
Draw: 5%
Attacker losses: 2,000, 700, 200
Defender losses: 9,900, 2,000, 0
Debris: 600, 210, 0
Recycler: 1
Moonchance: 0%
```

##### Json output

```sh
./main -c config.ini -j
```

```json
{
    "simulations": 20,
    "attacker_win": 90,
    "defender_win": 5,
    "draw": 5,
    "rounds": 2,
    "attacker_losses": {
        "metal": 2000,
        "crystal": 700,
        "deuterium": 200
    },
    "defender_losses": {
        "metal": 9700,
        "crystal": 1900,
        "deuterium": 0
    },
    "debris": {
        "metal": 600,
        "crystal": 210,
        "deuterium": 0
    },
    "recycler": 1,
    "moonchance": 0
}
```