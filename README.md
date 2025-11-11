# sunvox-max

A buggy Max/MSP external which embeds the [sunvox](https://www.warmplace.ru/soft/sunvox/sunvox_lib.php) engine.


## Status

Test outputs correct but crappy audio output which is probably due to incorrect audio processing (interleaving?), the audio bypasses Max, and this needs to be fixed.

## Usage


To test (only macOS):

```bash
make setup
make
```


