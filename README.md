# sunvox-max

Am Max/MSP external which embeds the [sunvox](https://www.warmplace.ru/soft/sunvox/sunvox_lib.php) engine.


## Status

Working proof-of-concept with the following methods:

- `load <song.sunvox>`: loads the sunvox song
- `play`: plays the sunvox song
- `stop`: stops playing
- `volume <val>`: set volume (0-256)

## To Build

```bash
make setup
make
```

