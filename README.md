# presetter

An enhanced preset manager external for Max.

## Build

First clone the repository along with submodules:

```sh
# HTTP
git clone https://github.com/hiamani/presetter.git --recursive
# SSH
git clone git@github.com:hiamani/presetter.git --recursive
```

Then build:

```sh
cd build
cmake ..
cmake --build .
```

## Support

Right now the build is only optimized for MacOS architecture. If you're a Windows user please open an issue and we can work through building this for your platform!

## Prototype

The initial prototype was build using v8ui. See [prototype-js](/prototype-js/).

## TODO

- Filters!
- Interpolation(ish)!
