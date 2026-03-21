# presetter

An enhanced preset manager external for Max, with naming, pagination, and filters.

<img src="https://github.com/hiamani/presetter/blob/main/resources/presetter.png?raw=true" width="400"/>

## Build

First clone the repository along with submodules:

```sh
cd /path/to/Max\ 9/Packages/
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

If you're on an M-class Mac, you'll need to codesign the presetter external:

```sh
# From the project root
codesign --force --deep -s - externals/presetter.mxo
```

## Support

Right now the build is only optimized for MacOS architecture. If you're a
Windows user please open an issue and we can work through building this for
your platform!

## Prototype

The initial prototype was build using v8ui. See [prototype-js](/prototype-js/).

## Roadmap

The main feature missing from presetter is interpolation between presets. An upcoming release focused on providing a smooth user interface for this is forthcoming!
