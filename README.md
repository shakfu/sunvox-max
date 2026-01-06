# sunvox-max

A Max/MSP external which embeds the [sunvox](https://www.warmplace.ru/soft/sunvox/sunvox_lib.php) engine.


## Status

Working macOS-only proof-of-concept with the following methods:

- `load <song.sunvox>`: loads the sunvox song
- `play`: plays the sunvox song
- `stop`: stops playing
- `volume <val>`: set volume (0-256)

There's a lot that can added (just look at the `source/projects/sunvox_tilde/examples` directory), hopefull more coming soonish.

## To Build

```bash
make setup
make
```

